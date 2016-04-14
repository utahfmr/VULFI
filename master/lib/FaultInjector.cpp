
#include "FaultInjector.h"

using namespace llvm;
using namespace std;

map<BasicBlock*,Instruction*> FaultInjector::getExitBBs(CLData* Cl, FunctionList *Fl){
  map<BasicBlock*,Instruction*> exitBBList;
  Function* fnMain = Fl->getFiFnMain();
  for(inst_iterator it=inst_begin(fnMain),itEnd=inst_end(fnMain);
      it!=itEnd;it++){
    Instruction* currentInstr = &*it;
    if(dyn_cast<ReturnInst>(currentInstr)){
		#ifdef DEBUG
      errs() << "\nFound a new exit basic blocks!\n";
      #endif
      BasicBlock* currentexitBB = currentInstr->getParent();
      if(exitBBList.find(currentexitBB) == exitBBList.end()){
	exitBBList.insert(pair<BasicBlock*,Instruction*>(currentexitBB,currentInstr));
      } else { // Error!! One basic block can't have more than one return inst
	errs() << "\nError:A single basic block can't have more than one return inst!\n";
	exit(-1);
      }
    }
  }
  return exitBBList;
}

bool FaultInjector::insertCalltoPrintinMain(CLData* Cl, FunctionList *Fl){
  bool insertFlag=false;
  map<BasicBlock*,Instruction*> exitBBList = this->getExitBBs(Cl,Fl);
  for(map<BasicBlock*,Instruction*>::iterator mi = exitBBList.begin();
      mi!=exitBBList.end();++mi){
    BasicBlock* currentexitBB = mi->first;
    Instruction* currentInstr = mi->second;
    Function* callFn = NULL;
    string FnName;
    vector<Value*> arglist; //create a dummy arglist
    if(currentexitBB && currentInstr){
      // insert call to printFaultSitesStat()
      callFn = Fl->getFiFnPrintStats();
      FnName = "printFaultSitesStat";
      CallInst* callInst1 = CallInst::Create(callFn,arglist,Twine(FnName),currentInstr);
      if(callInst1){
	callInst1->setCallingConv(CallingConv::C);
      }

      // insert call to printFaultInjectionStat()
      callFn = Fl->getFiFnPrintInjStats();
      FnName = "printFaultInjectionStat";
      CallInst* callInst2 = CallInst::Create(callFn,arglist,Twine(FnName),currentInstr);
      if(callInst2){
	callInst2->setCallingConv(CallingConv::C);
      }

      // if both call creation succeed then set the flag to true
      if(callInst1 && callInst2){
	insertFlag=true;
      }
    }
  }
  if(insertFlag){
    return true;
  }
  return false;
}

Value* FaultInjector::getMaskRegValue(Instruction* instr, CLData* Cl){
  Type::TypeID tyID = instr->getType()->getTypeID();
  if(tyID == Type::VectorTyID){
    CallInst* callinst = dyn_cast<CallInst>(instr);
    if(callinst){
      IntrinsicsDef intdef = IntrinsicsDef(Cl->getArch());
      Value* maskval = intdef.getMask(callinst);
      return maskval;
    }
  }
  return NULL;
}

Value* FaultInjector::getAllMaskOn(Module* M, Instruction* instr){
  Value* allMaskOn = NULL;
  Type* ty = instr->getType();
  if(ty->isIntegerTy(1)){
    allMaskOn = ConstantInt::getAllOnesValue(Type::getIntNTy(getGlobalContext(),32));
  } else if(ty->isIntegerTy() && !ty->isIntegerTy(1)){
    allMaskOn = ConstantInt::getAllOnesValue(ty);
  } else if(ty->isFloatTy() || ty->isDoubleTy()){
    allMaskOn = ConstantFP::getAllOnesValue(ty);
  } else if(ty->isPointerTy()){
    DataLayout dl = DataLayout(M);
    unsigned addrsz = dl.getPointerSizeInBits();
    if(addrsz==32){
      allMaskOn = ConstantInt::getAllOnesValue(Type::getIntNTy(getGlobalContext(),32));
    } else if(addrsz==64){
      allMaskOn = ConstantInt::getAllOnesValue(Type::getIntNTy(getGlobalContext(),64));
    }
  }
  return allMaskOn;
}

// @brief - populates the argument list with params to fault injection function
// called at runtime. Also, returns a clone of tha target function to be
// instrumented.
Instruction* FaultInjector::prepareInstrForFaultInjection(Module *M, CLData* Cl,
							  vector<Value*> &arglist,
							  Instruction* instr){
  // Clone the target instruction. This is required because when
  // replaceAllUses() is applied on the target instruction \var instr,
  // it does not create a self-reference loop on the call to injectSoftError()
  // instruction.
  Instruction* instrClone = instr->clone();
  instrClone->insertAfter(instr);
  Value* data = instrClone;

  // push the data in which faults has to be injected
  arglist.push_back(data);

  // arg#2 - name of the instruction
  string instrString;
  llvm::raw_string_ostream rso(instrString);
  instr->print(rso);
  Constant *instrName = ConstantDataArray::getString(getGlobalContext(),instrString,true);
  GlobalVariable *instrNameGlobal = new GlobalVariable(*M,instrName->getType(),true,
						       GlobalValue::InternalLinkage,instrName,"instrname");
  Constant *nullvalue = Constant::getNullValue(IntegerType::getInt32Ty(getGlobalContext()));
  vector<Constant*> gepParams;
  gepParams.push_back(nullvalue);
  gepParams.push_back(nullvalue);
  Constant *instrNamePtr = ConstantExpr::getGetElementPtr(instrName->getType(),instrNameGlobal,gepParams,false,NULL);  
  arglist.push_back(instrNamePtr);
  // arg#3 - all mask on for scalars OR add a place holder for vectors
  #ifdef DEBUG
  errs() <<"\nCurrent instruction: " << *instr << "\n";
  #endif
  Type* ty = instr->getType();
  Type::TypeID tyID = ty->getTypeID();
  if(tyID == Type::IntegerTyID ||
     tyID == Type::FloatTyID   ||
     tyID == Type::DoubleTyID  ||
     tyID == Type::PointerTyID){
    Value* allMaskOn = this->getAllMaskOn(M,instr);
    arglist.push_back(allMaskOn); // mask always enabled for scalars
  } else if(tyID == Type::VectorTyID){
    Type* ety = instr->getType()->getVectorElementType();
    Type::TypeID etyID = ety->getTypeID();
    if(etyID == Type::IntegerTyID ||
       etyID == Type::FloatTyID   ||
       etyID == Type::DoubleTyID  ||
       etyID == Type::PointerTyID){
      Value* allMaskOn = this->getAllMaskOn(M,instr);
      arglist.push_back(allMaskOn); // just a place holder
    }
  }

  // if language in OpenCL, add the 4th, 5th, and 6th arguments
  // which are the last three arguments of the OCL kernel function
  string lang = Cl->getLang();
  if(lang.compare("OCL")==0){
    Function* parentFn = instr->getParent()->getParent();
    if(parentFn->arg_empty() || parentFn->arg_size() < 4){
      errs() << "Error: A target OpenCL kernel should have at least three arguments!";
      errs() << "Function name: " << parentFn->getName() << "\n";
      exit(-1);
    }

    size_t argsz = parentFn->arg_size();
    unsigned counter = 0;
    for(Function::arg_iterator argit = parentFn->arg_begin(); argit != parentFn->arg_end();argit++){
      Value* lastarg = &*argit;
      if(counter==argsz-4 || counter==argsz-3 || counter==argsz-2 || counter==argsz-1){
	arglist.push_back(lastarg);
	#ifdef DEBUG
	errs() << "\n Found argument: " << *lastarg << "\n";
	#endif
      }
      counter++;
    }
  }
  return instrClone;
}

// @brief - perform instrumentation to insert a call instruction which will call
// the fault injection function at runtime.
bool FaultInjector::injectNonVectorFaults(Module *M, Instruction* instr, CLData* Cl, FunctionList *Fl){
  #ifdef DEBUG
  errs() << "\nInjecting fault in:\n" << *instr << "\n\n";
  #endif
  vector<Value*> arglist;
  Instruction* instrClone = this->prepareInstrForFaultInjection(M,Cl,arglist,instr);
  Instruction* nextInstr = this->getNextInstr(M,instrClone);
  Type* ty = instrClone->getType();
  if(ty->isIntegerTy(1)){
    CastInst* castInst=NULL;
    if( nextInstr != NULL){
      castInst = this->createCastInstByInst(M,instrClone,nextInstr,Fl,arglist,"injectError");
    } else {
      castInst = this->createCastInstByBB(M,instrClone,Fl,arglist,"injectError");
    }
    if(castInst){
      instr->replaceAllUsesWith(castInst);
      instr->eraseFromParent();
      return true;
    }
  } else if(ty->isPointerTy()){
    IntToPtrInst* castInst = NULL;
    if(nextInstr){
      castInst = this->createCastPtrInstByInst(M,instrClone,nextInstr,Fl,arglist,"injectError");
    } else{
      castInst = this->createCastPtrInstByBB(M,instrClone,Fl,arglist,"injectError");
    }
    if(castInst){
      instr->replaceAllUsesWith(castInst);
      instr->eraseFromParent();
      return true;
    }
  } else {
    CallInst* callInst=NULL;
    if( nextInstr != NULL){
      callInst = this->createCallInstByInst(M,instrClone,nextInstr,Fl,arglist,"injectError");
    } else {
      callInst = this->createCallInstByBB(M,instrClone,Fl,arglist,"injectError");
    }

    if(callInst){
      instr->replaceAllUsesWith(callInst);
      instr->eraseFromParent();
      return true;
    }
  }
  return false;
}


bool FaultInjector::matchType(Instruction* instr, Value* __mask){
  #ifdef DEBUG
  errs() << "\nInstruction is: " << *instr;
  errs() << "\nMask is: " << *__mask;
  #endif
  // Take care of below scenarios:
  // 1. __mask is NULL
  // 2. __mask is a vector and should have same width and element type as instr
  // 3. __mask is a scalar and should be of some fixed size (e.g. 1 byte)

  // if no mask found always return true
  if(__mask == NULL){
    return true;
  }
  Type::TypeID vtyID1 = instr->getType()->getTypeID();
  Type::TypeID vtyID2 = __mask->getType()->getTypeID();
  if(vtyID1 != vtyID2){
    return false;
  }

  if(vtyID1 == Type::VectorTyID && vtyID2 == Type::VectorTyID){
    unsigned numelm1 = instr->getType()->getVectorNumElements();
    unsigned numelm2 = __mask->getType()->getVectorNumElements();
    if(numelm1 != numelm2){
      return false;
    }

    Type::TypeID etyID1 = instr->getType()->getVectorElementType()->getTypeID();
    Type::TypeID etyID2 = __mask->getType()->getVectorElementType()->getTypeID();
    if(etyID1 != etyID2){
      return false;
    }
  }
  return true;
}

Value* FaultInjector::getScalarMaskValByInst(Value* __mask, Value* idx, Instruction* instr){
  ExtractElementInst* mextInst = ExtractElementInst::Create(__mask,idx,"extractmask",instr);
  return mextInst;
}

Value* FaultInjector::getScalarMaskValByBB(Value* __mask, Value* idx, BasicBlock* bb){
  ExtractElementInst* mextInst = ExtractElementInst::Create(__mask,idx,"extractmask",bb);
  return mextInst;
}

// @brief - perform instrumentation to insert a call instruction which will call
// the fault injection function at runtime.
bool FaultInjector::injectVectorFaults(Module *M, Instruction* instr, CLData* Cl, FunctionList *Fl){
  #ifdef DEBUG
  errs() << "\nInjecting fault in:\n" << *instr << "\n\n";
  #endif
  vector<Value*> arglist;
  Instruction* instrClone = this->prepareInstrForFaultInjection(M,Cl,arglist,instr);
  Value* __mask = this->getMaskRegValue(instr,Cl);
  Instruction* nextInstr = this->getNextInstr(M,instrClone);
  BasicBlock* parent = instr->getParent();

  // Confirm that instruction is a vector type
  // and also get its element type
  Type* vTy = instrClone->getType();
  Type* eTy = NULL;
  unsigned numElements=0;

  // check if target instruction is indeed a vector type!
  if(vTy->isVectorTy()){
    eTy = vTy->getVectorElementType();
    if(!eTy->isIntegerTy() &&
       !eTy->isFloatTy()   &&
       !eTy->isDoubleTy()  &&
       !eTy->isPointerTy()){
      return false;
    }
    numElements = vTy->getVectorNumElements();
  } else{
    return false;
  }

  // check if mask reg and intruction type matches
  // if no mask then always true
  if(!matchType(instr,__mask)){
    return false;
  }

  // At this point, both instr and __mask are of vector type
  ExtractElementInst* extInst = NULL;
  InsertElementInst*  inInst = NULL;
  Value* appendInstr = instrClone;
  for(unsigned i=0;i<numElements&&vTy->isVectorTy();i++){
    // For each element insert below instructions:
    // 1. Extract element
    // 2. Corrupt Element
    // 3. Insert back the element
    Value* idx = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),i);
    if(eTy->isIntegerTy(1)){
      if(nextInstr){
	extInst = ExtractElementInst::Create(appendInstr,idx,"extract",nextInstr);
	if(__mask){
	  arglist[2] = this->getScalarMaskValByInst(__mask,idx,nextInstr);
	} else {
	  arglist[2] = this->getAllMaskOn(M,extInst);
	}
	CastInst* castInst = this->createCastInstByInst(M,extInst,nextInstr,Fl,arglist,"injectError");
	inInst  = InsertElementInst::Create(appendInstr,castInst,idx,"insert",nextInstr);
      } else{
	extInst = ExtractElementInst::Create(appendInstr,idx,"extract",parent);
	if(__mask){
	  arglist[2] = this->getScalarMaskValByBB(__mask,idx,parent);
	} else {
	  arglist[2] = this->getAllMaskOn(M,extInst);
	}
	CastInst* castInst = this->createCastInstByBB(M,extInst,Fl,arglist,"injectError");
	inInst  = InsertElementInst::Create(appendInstr,castInst,idx,"insert",parent);
      }
    } else if(eTy->isPointerTy()){
      if(nextInstr){
	extInst = ExtractElementInst::Create(appendInstr,idx,"extract",nextInstr);
	if(__mask){
	  arglist[2] = this->getScalarMaskValByInst(__mask,idx,nextInstr);
	} else {
	  arglist[2] = this->getAllMaskOn(M,extInst);
	}
	IntToPtrInst* castInst = this->createCastPtrInstByInst(M,extInst,nextInstr,Fl,arglist,"injectError");
	inInst  = InsertElementInst::Create(appendInstr,castInst,idx,"insert",nextInstr);
      } else{
	extInst = ExtractElementInst::Create(appendInstr,idx,"extract",parent);
	if(__mask){
	  arglist[2] = this->getScalarMaskValByBB(__mask,idx,parent);
	} else {
	  arglist[2] = this->getAllMaskOn(M,extInst);
	}
	IntToPtrInst* castInst = this->createCastPtrInstByBB(M,extInst,Fl,arglist,"injectError");
	inInst  = InsertElementInst::Create(appendInstr,castInst,idx,"insert",parent);
      }
    } else{
      if(nextInstr){
	extInst = ExtractElementInst::Create(appendInstr,idx,"extract",nextInstr);
	if(__mask){
	  arglist[2] = this->getScalarMaskValByInst(__mask,idx,nextInstr);
	} else {
	  arglist[2] = this->getAllMaskOn(M,extInst);
	}
	CallInst* callInst = this->createCallInstByInst(M,extInst,nextInstr,Fl,arglist,"injectError");
	inInst  = InsertElementInst::Create(appendInstr,callInst,idx,"insert",nextInstr);
      } else{
	extInst = ExtractElementInst::Create(appendInstr,idx,"extract",parent);
	if(__mask){
	  arglist[2] = this->getScalarMaskValByBB(__mask,idx,parent);
	} else {
	  arglist[2] = this->getAllMaskOn(M,extInst);
	}
	CallInst* callInst = this->createCallInstByBB(M,extInst,Fl,arglist,"injectError");
	inInst  = InsertElementInst::Create(appendInstr,callInst,idx,"insert",parent);
      }
    }
    appendInstr = inInst;
  }

  instr->replaceAllUsesWith(appendInstr);
  instr->eraseFromParent();
  return true;
}

Instruction* FaultInjector::getPreviousInstr(Module *M, Instruction* instr){
  BasicBlock* parent = instr->getParent();
  Instruction* prevInstr = NULL;
  BasicBlock::iterator bi;
  for(bi=parent->begin();bi!=parent->end();++bi){
    Instruction* currentInstr = &*bi;
    if(instr == currentInstr){
      break;
    }
  }

  if(bi != parent->begin()){
    bi--;
    prevInstr = &*bi;
  }
  return prevInstr;
}

// Get the next instruction for a given instruction in
// a basic block. Returns NULL is the current instruction
// is the last instruction.
Instruction* FaultInjector::getNextInstr(Module *M, Instruction* instr){
  BasicBlock* parent = instr->getParent();
  Instruction* nextInstr = NULL;
  if(dyn_cast<PHINode>(instr)){
    nextInstr = parent->getFirstNonPHI();
  } else {
    BasicBlock::iterator bi;
    for(bi=parent->begin();bi!=parent->end();++bi){
      Instruction* currentInstr = &*bi;
      if(instr == currentInstr){
	break;
      }
    }
    bi++;
    if(bi != parent->end()){
      nextInstr = &*bi;
    }
  }
  return nextInstr;
}

// Create a call instruction which calls the error injector
// to inject error in currentInstr
CallInst* FaultInjector::createCallInstByInst(Module *M, Instruction* currentInstr,
					      Instruction* nextInstr,
					      FunctionList* Fl,
					      vector<Value*> arglist,
					      string callFnName){
  Value* fnRef=NULL;
  Type::TypeID tyID = currentInstr->getType()->getTypeID();
  Type* ty = currentInstr->getType();
  if(tyID == Type::IntegerTyID){
    //		if(ty->isIntegerTy(8)){
    //			fnRef = Fl->getFiFnIntTy8();
    //			errs() << "\nInfo:found a Ty8 data for instr: " << *currentInstr;
    //			errs() << "\nInfo:name of the instrumented function call: " << fnRef->getName() <<"\n";
    //		} else
    if(ty->isIntegerTy(16)){
      fnRef = Fl->getFiFnIntTy16();
    } else if(ty->isIntegerTy(32)){
      fnRef = Fl->getFiFnIntTy32();
    } else if(ty->isIntegerTy(64)){
      fnRef = Fl->getFiFnIntTy64();
    }
  } else if(tyID == Type::FloatTyID){
    fnRef = Fl->getFiFnFloatTy();
  } else if(tyID == Type::DoubleTyID){
    fnRef = Fl->getFiFnDoubleTy();
  }
  arglist[0] = currentInstr;
  if(fnRef){
    CallInst* callInst = CallInst::Create(fnRef,arglist,callFnName,nextInstr);
    callInst->setCallingConv(CallingConv::C);
    return callInst;
  }
  return NULL;
}


// This function does the same thing as createCallInstByInst()
// with one difference that it is specifically written to handle
// a corner case of IntegerTy(1) as C/C++ does not support it.
// C++ does support boolean but LLVM does not treat it as IntegerTy(1)
// The solution is : a) zext IntegerTy(1)->IntegerTy(32), b)
// call injector on IntegerTy(32), c) zext  again
// IntegerTy(32)->IntegerTy(1)
CastInst* FaultInjector::createCastInstByInst(Module *M, Instruction* instrClone,
					      Instruction* nextInstr,
					      FunctionList* Fl,
					      vector<Value*> arglist,
					      string callFnName){

  Function* fnRef=NULL;
  CastInst* castCmpToInt=NULL;
  Type::TypeID tyID = instrClone->getType()->getTypeID();
  Type* ty = instrClone->getType();
  if(tyID == Type::IntegerTyID && ty->isIntegerTy(1)){
    fnRef = Fl->getFiFnIntTy1();
    castCmpToInt = CastInst::CreateZExtOrBitCast(instrClone,
						 Type::getInt32Ty(getGlobalContext()),"castcmptoint",nextInstr);
    if(castCmpToInt && fnRef){
      arglist[0]=castCmpToInt; //overwrite with the actual value
      CallInst* callInst = CallInst::Create(fnRef,arglist,callFnName,nextInstr);
      callInst->setCallingConv(CallingConv::C);
      CastInst* castInttoCmp = CastInst::CreateTruncOrBitCast(callInst,
							      Type::getInt1Ty(getGlobalContext()),"castinttocmp",nextInstr);
      return castInttoCmp;
    }
  }
  return NULL;
}

IntToPtrInst* FaultInjector::createCastPtrInstByInst(Module *M, Instruction* instrClone,
						     Instruction* nextInstr,
						     FunctionList* Fl,
						     vector<Value*> arglist,
						     string callFnName){

  Function* fnRef=NULL;
  Type::TypeID tyID = instrClone->getType()->getTypeID();
  Type* ty = instrClone->getType();
  Type* castTy=NULL;
  if(tyID == Type::PointerTyID){
    DataLayout dl(M);
    unsigned addrsz = dl.getPointerSizeInBits();
    if(addrsz == 32){ // 32-bit addresses
	  #ifdef DEBUG
      errs() << "\n32-bit addressing mode found!\n";
      #endif
      fnRef = Fl->getFiFnIntTy32();
      castTy = Type::getInt32Ty(getGlobalContext());
    } else if(addrsz == 64){ // 64-bit addresses
	  #ifdef DEBUG
      errs() << "\n64-bit addressing mode found!\n";
      #endif
      fnRef = Fl->getFiFnIntTy64();
      castTy = Type::getInt64Ty(getGlobalContext());
    } else {
      errs() << "Error:addressing mode should either be 32-bit or 64-bit!!\n";
      exit(-1);
    }
    PtrToIntInst* castPtrToInt = new PtrToIntInst(instrClone,castTy,"castptrtoint",nextInstr);
    if(castPtrToInt && fnRef){
      arglist[0]=castPtrToInt;
      CallInst* callInst = CallInst::Create(fnRef,arglist,callFnName,nextInstr);
      callInst->setCallingConv(CallingConv::C);
      IntToPtrInst* castInttoPtr = new IntToPtrInst(callInst,ty,"castinttoptr",nextInstr);
      return castInttoPtr;
    }
  }
  return NULL;
}


CallInst* FaultInjector::createCallInstByBB(Module *M, Instruction* instr, FunctionList* Fl,
					    vector<Value*> arglist,
					    string callFnName){
  Value* fnRef=NULL;
  BasicBlock* parent = instr->getParent();
  Type::TypeID tyID = instr->getType()->getTypeID();
  Type* ty = instr->getType();
  if(tyID == Type::IntegerTyID){
    //		if(ty->isIntegerTy(8)){
    //			fnRef = Fl->getFiFnIntTy8();
    //		} else
    if(ty->isIntegerTy(16)){
      fnRef = Fl->getFiFnIntTy16();
    } else if(ty->isIntegerTy(32)){
      fnRef = Fl->getFiFnIntTy32();
    } else if(ty->isIntegerTy(64)){
      fnRef = Fl->getFiFnIntTy64();
    }
  } else if(tyID == Type::FloatTyID){
    fnRef = Fl->getFiFnFloatTy();
  } else if(tyID == Type::DoubleTyID){
    fnRef = Fl->getFiFnDoubleTy();
  }
  CallInst* callInst = NULL;
  if(fnRef){
    callInst = CallInst::Create(fnRef,arglist,callFnName,parent);
    callInst->setCallingConv(CallingConv::C);
  }
  return callInst;
}

CastInst* FaultInjector::createCastInstByBB(Module *M, Instruction* instrClone,
					    FunctionList* Fl,
					    vector<Value*> arglist,
					    string callFnName){
  Value* fnRef=NULL;
  BasicBlock* parent = instrClone->getParent();
  CastInst* castCmpToInt=NULL;
  Type::TypeID tyID = instrClone->getType()->getTypeID();
  Type* ty = instrClone->getType();
  if(tyID == Type::IntegerTyID && ty->isIntegerTy(1)){
    fnRef = Fl->getFiFnIntTy1();
    castCmpToInt = CastInst::CreateZExtOrBitCast(instrClone,
						 Type::getInt32Ty(getGlobalContext()),"castcmptoint",parent);
    if(castCmpToInt && fnRef){
      arglist[0]=castCmpToInt;
      CallInst* callInst = CallInst::Create(fnRef,arglist,callFnName,parent);
      callInst->setCallingConv(CallingConv::C);
      CastInst* castInttoCmp = CastInst::CreateTruncOrBitCast(callInst,
							      Type::getInt1Ty(getGlobalContext()),"castinttocmp",parent);
      return castInttoCmp;
    }
  }
  return NULL;
}

IntToPtrInst* FaultInjector::createCastPtrInstByBB(Module *M, Instruction* instrClone,
						   FunctionList* Fl,
						   vector<Value*> arglist,
						   string callFnName){
  Value* fnRef=NULL;
  BasicBlock* parent = instrClone->getParent();
  Type::TypeID tyID = instrClone->getType()->getTypeID();
  Type* ty = instrClone->getType();
  Type* castTy=NULL;
  if(tyID == Type::PointerTyID){
    DataLayout dl(M);
    unsigned addrsz = dl.getPointerSizeInBits();
    if(addrsz == 32){ // 32-bit addresses
      #ifdef DEBUG
      errs() << "\n32-bit addressing mode found!\n";
      #endif
      fnRef = Fl->getFiFnIntTy32();
      castTy = Type::getInt32Ty(getGlobalContext());
    } else if(addrsz == 64){ // 64-bit addresses
      #ifdef DEBUG
      errs() << "\n64-bit addressing mode found!\n";
      #endif
      fnRef = Fl->getFiFnIntTy64();
      castTy = Type::getInt64Ty(getGlobalContext());
    } else {
      errs() << "Error:addressing mode should either be 32-bit or 64-bit!!\n";
      exit(-1);
    }
    PtrToIntInst* castPtrToInt = new PtrToIntInst(instrClone,castTy,"castptrtoint",parent);
    if(castPtrToInt && fnRef){
      arglist[0]=castPtrToInt;
      CallInst* callInst = CallInst::Create(fnRef,arglist,callFnName,parent);
      callInst->setCallingConv(CallingConv::C);
      IntToPtrInst* castInttoPtr = new IntToPtrInst(callInst,ty,"castinttoptr",parent);
      return castInttoPtr;
    }
  }
  return NULL;
}
