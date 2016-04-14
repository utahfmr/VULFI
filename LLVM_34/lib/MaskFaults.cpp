
#include <time.h>
#include <math.h>
#include "Common.h"
#include "MaskFaults.h"

using namespace llvm;
using namespace std;

Instruction* MaskFaults::performInstrumentation(Module *M, CLData* Cl, FunctionList* Fl, Instruction* instr, Common::VectorFSType vftype, int idxLimit){
  // Clone the target instruction. This is required because when
  // replaceAllUses() is applied on the target instruction \var instr,
  // it does not create a self-reference loop on the call to injectSoftError()
  // instruction.
  vector<Value*> arglist;
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
  Constant *instrNamePtr = ConstantExpr::getGetElementPtr(instrNameGlobal,gepParams);
  arglist.push_back(instrNamePtr);

  // arg# 3
  if(vftype == Common::FST_EXIDX || vftype == Common::FST_INSIDX){
    Value* arg3 = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),idxLimit,false);
    arglist.push_back(arg3);
  } else if(vftype == Common::FST_SHUFIDX){
    Value* arg3 = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),idxLimit,false);
    arglist.push_back(arg3);
  } else {
    Value* arg3 = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),0,false);
    arglist.push_back(arg3);
  }


  // if language in OpenCL, add the 4th, 5th, and 6th arguments
  // which are the last three arguments of the OCL kernel function
  string lang = Cl->getLang();
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
      DEBUG(errs() << "\n Found argument: " << *lastarg << "\n");
    }
    counter++;
  }


  // perform instrumentation
  if(vftype == Common::FST_INSIDX || vftype == Common::FST_EXIDX){
    Function* fnRef = Fl->getFiFnVectorIdx();
    if(fnRef){
      Instruction* instrClone = instr->clone();
      instrClone->insertAfter(instr);
      Value* data = instrClone;
      // arg1
      arglist.insert(arglist.begin(),data);

      CallInst* callInst = NULL;
      Instruction* nextInstr = FaultInjector::getNextInstr(M,instrClone);
      if(nextInstr){
	callInst = CallInst::Create(fnRef,arglist,"injectErrorVectorIdx",nextInstr);
	callInst->setCallingConv(CallingConv::C);
      } else {
	callInst = CallInst::Create(fnRef,arglist,"injectErrorVectorIdx",instr->getParent());
	callInst->setCallingConv(CallingConv::C);
      }

      if(!callInst){
	errs() << "\nError: reference to call instruction can't be null here\n";
	exit(-1);
      }

      instr->replaceAllUsesWith(instrClone);
      instr->eraseFromParent();
      return callInst;
    }
  }

  // TODO: implementation for shufflevector
  if(vftype == Common::FST_SHUFIDX){
    ShuffleVectorInst* shufinstr = dyn_cast<ShuffleVectorInst>(instr);
    Value *op0 = shufinstr->getOperand(0);
    Constant* mask = shufinstr->getMask();
    int veclen = op0->getType()->getVectorNumElements();
    int masklen = (int) mask->getType()->getVectorNumElements();

    // create clone
    Instruction* shuffleclone = shufinstr->clone();
    shuffleclone->setName("shuffclone");
    shuffleclone->insertAfter(shufinstr);

    // create the shufflevector which returns all the elements from both the operands inorder
    vector<Constant*> constList;
    for(int i=0;i<2*veclen;i++){
      Constant* tempMask = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),i,false);
      constList.push_back(tempMask);
    }
    Constant* newmask = ConstantVector::get(constList);
    Instruction* newshuffle = shuffleclone->clone();
    newshuffle->setOperand(2,newmask);
    newshuffle->setName("shuffall");
    newshuffle->insertAfter(shuffleclone);

    // 1. instrument the original mask for fault injection
    vector<Instruction*> corruptMaskList;
    vector<Value*>::iterator it = arglist.begin();
    Constant* dummy = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),0,false);
    arglist.insert(it,dummy); // arg1 - a placeholder, gets updated in the loop

    // arg# 3
    int tempIdxLimit = (int)log2((float)2*veclen);
    Constant* idxLimitVal = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),tempIdxLimit,false);
    arglist[2]=idxLimitVal;
    Instruction* nextInstr = FaultInjector::getNextInstr(M,newshuffle);
    CallInst* callInst = NULL;
    Function* fnRef = Fl->getFiFnVectorIdx();
    for(int i=0;i<masklen;i++){
      if(i>0 && isBroadcast(shufinstr)) continue;
      int tempMaskVal = shufinstr->getMaskValue(mask,i);
      Constant* tempMask = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),tempMaskVal,false);
      arglist[0]=tempMask;
      if(fnRef){
	if(nextInstr){
	  callInst = CallInst::Create(fnRef,arglist,"injectErrorVectorIdx",nextInstr);
	  callInst->setCallingConv(CallingConv::C);
	} else {
	  callInst = CallInst::Create(fnRef,arglist,"injectErrorVectorIdx",instr->getParent());
	  callInst->setCallingConv(CallingConv::C);
	}
	if(!callInst){
	  errs() << "\nError: reference to call instruction can't be null here\n";
	  exit(-1);
	}
	corruptMaskList.push_back(callInst);
      }
    }

    // 2. extract the new element from the "allelements" based on new mask
    // 3. insert the new element in the shufflevector
    Instruction* insertinto = shuffleclone;
    for(int i=0;i<masklen;i++){
      ExtractElementInst* newextract;
      if(nextInstr){
	if(i>0 && isBroadcast(shufinstr)) {
	  newextract = \
	    ExtractElementInst::Create(newshuffle,corruptMaskList[0],"extractusingcorruptmask",nextInstr);
	} else {
	  newextract = \
	    ExtractElementInst::Create(newshuffle,corruptMaskList[i],"extractusingcorruptmask",nextInstr);
	}

	Constant* idx = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),i,false);
	insertinto = \
	  InsertElementInst::Create(insertinto,newextract,idx,"newinsert",nextInstr);
      } else {
	if(i>0 && isBroadcast(shufinstr)) {
	  newextract = \
	    ExtractElementInst::Create(newshuffle,corruptMaskList[0],"extractusingcorruptmask",instr->getParent());
	} else {
	  newextract = \
	    ExtractElementInst::Create(newshuffle,corruptMaskList[i],"extractusingcorruptmask",instr->getParent());
	}
	Constant* idx = ConstantInt::get(IntegerType::getInt32Ty(getGlobalContext()),i,false);
	insertinto = \
	  InsertElementInst::Create(insertinto,newextract,idx,"newinsert",instr->getParent());
      }
    }
    shufinstr->replaceAllUsesWith(insertinto);
    shufinstr->eraseFromParent();
  }
  return NULL;
}

int flipBit(void* data, int bytesz, int bitPos){
  long long dest = 0;

  // Copy source data to a 64-bit integer
  memcpy((void*)&dest,data,bytesz);

  if ((dest>>bitPos)&0x1){
    dest = dest & (~((long long)0x1 << (bitPos)));
  } else{
    dest = dest |  ((long long) 0x1 << (bitPos));
  }

  // Copy back the data with a random bit flipped into the source
  memcpy(data,(void*)&dest,bytesz);

  return bitPos; // A single-bit error successfully injected!!
}

bool MaskFaults::isBroadcast(ShuffleVectorInst* shufInstr){
  if(shufInstr){
    if(dyn_cast<Constant>(shufInstr->getMask())){
      Constant* constMask = shufInstr->getMask();
      if(constMask->getType()->isVectorTy()){
	int len = (int) constMask->getType()->getVectorNumElements();
	int maskValatZeroIdx = shufInstr->getMaskValue(constMask,0);
	for(int i=1;i<len;i++){
	  int maskVal = shufInstr->getMaskValue(constMask,i);
	  DEBUG(errs() << "\nThe value of the mask element at position: " << i << " is: " << maskVal);
	  if(maskVal != maskValatZeroIdx){
	    return false;
	  }
	}
      }
    }
  }
  return true;
}

void MaskFaults::processMaskInstr(Module* M, CLData* Cl, FunctionList* Fl, Instruction* instr){
  srand(time(NULL));
  Instruction* maskInstr = NULL;
  ExtractElementInst* extractinstr=dyn_cast<ExtractElementInst>(instr);
  InsertElementInst* insertinstr=dyn_cast<InsertElementInst>(instr);
  ShuffleVectorInst* shufinstr=dyn_cast<ShuffleVectorInst>(instr);

  if(extractinstr){
    Type* OpTy =  extractinstr->getOperand(0)->getType();
    if(OpTy->isVectorTy()){
      Value* exOpVal = extractinstr->getIndexOperand();
      maskInstr = BinaryOperator::Create(BinaryOperator::Or,exOpVal,exOpVal,"extractIdx",instr);
      if(maskInstr==NULL){
	errs() << "\nError:Creation of Or instruction failed!\n";
	exit(-1);
      }
      int idxLimit = (int)log2((float)OpTy->getVectorNumElements());
      DEBUG(errs () << "\nNumber of element in the ExtractElement instruction is: " << idxLimit);
      Instruction* callinst = this->performInstrumentation(M,Cl,Fl,maskInstr,Common::FST_EXIDX,idxLimit);
      if(callinst) extractinstr->setOperand(1,callinst);
    } else {
      errs() << "\nFatal Error: First operand of an ExtractElement should always be of vector type!\n";
      exit(-1);
    }

  } else if(insertinstr){
    Type* OpTy =  insertinstr->getOperand(0)->getType();
    if(OpTy->isVectorTy()){
      Value* insOpVal = insertinstr->getOperand(2);
      maskInstr = BinaryOperator::Create(BinaryOperator::Or,insOpVal,insOpVal,"insertIdx",instr);
      if(maskInstr==NULL){
	errs() << "\nError:Creation of Or instruction failed!\n";
	exit(-1);
      }
      int idxLimit = (int)log2((float)OpTy->getVectorNumElements());
      Instruction* callinst = this->performInstrumentation(M,Cl,Fl,maskInstr,Common::FST_INSIDX,idxLimit);
      if(callinst) insertinstr->setOperand(2,callinst);
    } else {
      errs() << "\nFatal Error: First operand of an InsertElement should always be of vector type!\n";
      exit(-1);
    }
  }
  else if(shufinstr){
    // check if it is a broadcast shuffle
    this->performInstrumentation(M,Cl,Fl,instr,Common::FST_SHUFIDX,0);
  }
}

bool MaskFaults::injectFaults(Module *M, set<Instruction*> instrList,
			      CLData* Cl, FunctionList *Fl){
  errs() << "\nInitiating mask fault injection instrumentation..\n";
  this->module = M;
  for(set<Instruction*>::iterator si=instrList.begin();
      si!=instrList.end();++si){
    Instruction* currentInstr = *si;
    if(dyn_cast<ExtractElementInst>(currentInstr) ||
       dyn_cast<InsertElementInst>(currentInstr)  ||
       dyn_cast<ShuffleVectorInst>(currentInstr)){
      processMaskInstr(M,Cl,Fl,currentInstr);
    } else {
      Type::TypeID tyID = currentInstr->getType()->getTypeID();
      if(tyID==Type::IntegerTyID ||
	 tyID==Type::FloatTyID   ||
	 tyID==Type::DoubleTyID){
	FaultInjector::injectNonVectorFaults(M,currentInstr,Cl,Fl);
      } else if(tyID==Type::VectorTyID){
	FaultInjector::injectVectorFaults(M,currentInstr,Cl,Fl);
      }
    }
  }

  if(Cl->getInstMainFlag()){
    FaultInjector::insertCalltoPrintinMain(Cl,Fl);
  }
  return true;
}

