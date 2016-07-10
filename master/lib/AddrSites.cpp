
#include "AddrSites.h"
#include "AddrInstrList.h"
#include <stack>

using namespace llvm;
using namespace std;

Instruction* getInstrByVal(Value* val, Function* fn){
  Instruction* targetInstr=NULL;
  for (inst_iterator I=inst_begin(fn),E=inst_end(fn);I!=E;++I){
    Instruction* instr = &*I;
    Value* instrVal = &*instr;
    if(instrVal==val){
      targetInstr=instr;
      break;
    }
  }
  return targetInstr;
}

set<Instruction*> getBkwdSlice(Instruction* instr){
  Function *fn=instr->getParent()->getParent();
  set<Instruction*> visited;
  set<Instruction*>::iterator si;
  stack<Instruction*> S;
  S.push(instr);
  while (!S.empty()){
    // Pop a vertex from stack and print it
    Instruction* ci = S.top();
    S.pop();

    for (Use &U : ci->operands()) {
      Value *v = U.get();
      Instruction *vi=getInstrByVal(v,fn);
      if(vi){
	if(visited.find(vi)==visited.end()){
	  visited.insert(vi);
	  S.push(vi);
	}
      }
    }
  } 
  return visited;
}

set<Value*> getArgList(Function *fn){
  set<Value*> arglist;
  for(Function::arg_iterator I=fn->arg_begin();I!=fn->arg_end();I++){
    Value *argVal=&*I;
    if(arglist.find(argVal)==arglist.end())
      arglist.insert(argVal);
  }  
  return arglist;
}

bool isBaseAdrInArgLst(GetElementPtrInst* gep){
  Function* parent=gep->getParent()->getParent();
  set<Value*> arglist=getArgList(parent);    
  Value* gepPtrOp = gep->getPointerOperand();
  if(arglist.find(gepPtrOp)!=arglist.end())
    return true;

  return false;
}

set<Instruction*> AddrSites::getfaultSites(Module *M, CLData *Cl, FunctionList *Fl){  
  // 1st Pass - Build the initial instruction list
  // Get list of target functions
  vector<Function*> fnList = Fl->getTargetFnList();
  enum Common::FSAlgo fsalg=Cl->getFSAlgo();
  set<Instruction*> restlst;
  unsigned instcount=0;
  // Iterative over instructions of the target functions
  for(unsigned i=0;i<fnList.size();i++){
    for(inst_iterator it=inst_begin(fnList[i]),itEnd=inst_end(fnList[i]);
	it!=itEnd;it++){
      Instruction* currentInstr = &*it;
      //Skip PHI Nodes
      if(isa<PHINode>(currentInstr)    ||
         isa<AllocaInst>(currentInstr) ||
	 isa<LandingPadInst>(currentInstr)){
	continue;
      }

      Type* ty = currentInstr->getType();
      Type::TypeID tyID = ty->getTypeID();
      if(tyID == Type::VectorTyID){
	Type* ety = currentInstr->getType()->getVectorElementType();
	if(!ety->isPointerTy()){
	  restlst.insert(currentInstr);
	  continue;
	}
      } else if(!ty->isPointerTy()) {
	restlst.insert(currentInstr);
	continue;
      }

      if((fsalg==Common::FS_ADDG || fsalg==Common::FS_ADDI) &&
         !isa<GetElementPtrInst>(currentInstr)){
	restlst.insert(currentInstr);
	continue;
      }

      // Build initial list of fault sites
      if(instrList.find(currentInstr)==instrList.end())
	instrList.insert(currentInstr);

      // Update instruction to function hash table
      this->instrToFuncTable.insert(pair<Instruction*, Function*>(currentInstr,fnList[i]));

#ifdef DEBUG
      // Print to screen initial target instructions
      errs() << "\nInstruction: "<< *currentInstr;
      errs() << "\nTypeID: "<< currentInstr->getType()->getTypeID();
#endif
      
    }
  }
  
  // 2nd/Final Pass - Update the initial instruction list

  //write debug data if dbgfile name is provided
  string dbgfile = Cl->getDbgFile();
  set<Instruction*> bcksliceInstrList;
  if(!dbgfile.empty()){
    for(set<Instruction*>::iterator si = instrList.begin();si!=instrList.end();++si){
      Instruction* currentInstr = *si;
      set<Instruction*> bckslice=getBkwdSlice(currentInstr);
      for(set<Instruction*>::iterator si1 = bckslice.begin();si1!=bckslice.end();++si1){
      	Instruction* bckInstr= *si1;
      	if(isa<PHINode>(bckInstr) ||
      	   isa<AllocaInst>(bckInstr) ||
      	   isa<LandingPadInst>(bckInstr)){
      	  continue;
      	} else if(bcksliceInstrList.find(bckInstr)==bcksliceInstrList.end()){
	  Type* ty = bckInstr->getType();
	  if(ty->isIntegerTy(16) ||
	     ty->isIntegerTy(32) ||
	     ty->isIntegerTy(64) ||
	     ty->isFloatTy()     ||
	     ty->isDoubleTy()){
	    bcksliceInstrList.insert(bckInstr);
	  } else if(ty->isVectorTy()){
	    Type* ety = ty->getVectorElementType();
	    if(ety->isIntegerTy(16) ||
	       ety->isIntegerTy(32) ||
	       ety->isIntegerTy(64) ||
	       ety->isFloatTy()     ||
	       ety->isDoubleTy()){
	      bcksliceInstrList.insert(bckInstr);
	    }
	  }	  
      	}
      }     
    }
  }

  if(fsalg==Common::FS_ADDR){
    for(set<Instruction*>::iterator si=bcksliceInstrList.begin();
	si!=bcksliceInstrList.end();++si){
      Instruction* bcksliceInstr=*si;
      if(instrList.find(bcksliceInstr)==instrList.end()){	
	instrList.insert(bcksliceInstr);
      }
    }
  }

  set<Instruction*> resultlst;  
  if(fsalg==Common::FS_ADDI) resultlst=bcksliceInstrList;
  else resultlst=instrList;

  for(set<Instruction*>::iterator si=resultlst.begin();
      si!=resultlst.end();++si){
    Instruction* instr=*si;    
    // print info for each instruction
    if(fsalg==Common::FS_ADDI)
      this->writeDbgData(instr,Cl,"addi");
    else if(fsalg==Common::FS_ADDG)
      this->writeDbgData(instr,Cl,"addg");
    else if(fsalg==Common::FS_ADDR)
      this->writeDbgData(instr,Cl,"addr");
  }

  for(set<Instruction*>::iterator si = restlst.begin();
      si!=restlst.end();++si){
    Instruction* currentInstr=*si;
    if(resultlst.find(currentInstr)!=resultlst.end())
      continue;
    Type::TypeID tyID = currentInstr->getType()->getTypeID();
    Type* ty=currentInstr->getType();      
    if(bcksliceInstrList.find(currentInstr)==
       bcksliceInstrList.end()){
      if(ty->isIntegerTy(16) ||
  	 ty->isIntegerTy(32) ||
  	 ty->isIntegerTy(64) ||
  	 tyID==Type::FloatTyID   ||
  	 tyID==Type::DoubleTyID){
  	if(isa<BinaryOperator>(currentInstr))
  	  instcount++;
      } else if(tyID==Type::VectorTyID){
  	Type* ety=ty->getVectorElementType();
  	Type::TypeID etyID = ety->getTypeID();
  	if(ety->isIntegerTy(16) ||
  	   ety->isIntegerTy(32) ||
  	   ety->isIntegerTy(64) ||
  	   etyID==Type::FloatTyID   ||
  	   etyID==Type::DoubleTyID){
  	  if(isa<BinaryOperator>(currentInstr))
  	    instcount++;
  	}
      } else if(isa<StoreInst>(currentInstr)){
  	instcount++;
      }
    }    
  }  

  errs() << "\n\n\n\n===================================";
  errs() << "\nRest over instruction count is: " 
	 << instcount;
  errs() << "\n===================================\n\n\n";
  return resultlst;
}

