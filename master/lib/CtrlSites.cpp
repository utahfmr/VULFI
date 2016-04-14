#include "CtrlSites.h"
#include "CtrlInstrList.h"

using namespace llvm;
using namespace std;

CtrlSites::CtrlSites(){
  ctrlInstrCount=0;
  vctrlInstrCount=0;
}

set<Instruction*> CtrlSites::getfaultSites(Module *M, CLData *Cl, FunctionList *Fl){

  Common::FSAlgo fsalgo = Cl->getFSAlgo();
  // sanity check
  if(fsalgo!=Common::FS_CTRL &&
     fsalgo!=Common::FS_VCTL){
    // we shouldn't be here
    // at least scalar or vector fault site selection algo should be selected
    errs() <<"\nError:At least scalar or vector fault site selection algo should be selected\n";
    exit(-1);
  }

  // 1st Pass
  vector<Function*> fnList = Fl->getTargetFnList();

  // Iterative over instructions of the target functions
  for(unsigned i=0;i<fnList.size();i++){
    for(inst_iterator it=inst_begin(fnList[i]),itEnd=inst_end(fnList[i]);
	it!=itEnd;it++){
      Instruction* currentInstr = &*it;      
      //Skip PHI Nodes
      if(dyn_cast<PHINode>(currentInstr) ||
	 dyn_cast<LandingPadInst>(currentInstr)){
	continue;
      }
      
      if(currentInstr->getType()->isIntegerTy(1)) ctrlInstrCount++;
      if(currentInstr->getType()->isVectorTy())
	if(currentInstr->getType()->getScalarType()->isIntegerTy(1)) vctrlInstrCount++;

      // Avoid IntegerTy(8) as there is not such type in C/C++ or ISPC
      // Also avoid directly injecting faults into compare instructions
      Type* ty = currentInstr->getType();
      Type::TypeID tyID = ty->getTypeID();
      if(tyID == Type::VectorTyID){
	Type* ety = currentInstr->getType()->getVectorElementType();
	if(ety->isIntegerTy(8) || ety->isIntegerTy(1)){
	  continue;
	}
      } else if(ty->isIntegerTy(8) || ty->isIntegerTy(1)) {
	continue;
      }

      // Build initial list of fault sites
      this->instrList.insert(currentInstr);
      // Update instruction to function hash table
      this->instrToFuncTable.insert(pair<Instruction*, Function*>(currentInstr,fnList[i]));
      // Print to screen initial target instructions
      #ifdef DEBUG
      errs() << "\nInstruction: "<< *currentInstr;
      errs() << "\nTypeID: "<< currentInstr->getType()->getTypeID();
      #endif
    }
  }

  // 2nd (final) pass - prune the initial instruction list using program slice
  CtrlInstrList cInstrList;
  set<Instruction*> ctrlsites;

  // insert scalar control-flow sites
  if(fsalgo==Common::FS_CTRL){
    set<Instruction*> tempset = cInstrList.genInstrList(this->instrList,Cl,Common::FS_CTRL);
    ctrlsites.insert(tempset.begin(),tempset.end()) ;
  }

  // insert vector control-flow sites
  if(fsalgo==Common::FS_VCTL){
    set<Instruction*> tempset = cInstrList.genInstrList(this->instrList,Cl,Common::FS_VCTL);
    ctrlsites.insert(tempset.begin(),tempset.end()) ;
  }

  //write debug data if dbgfile name is provided
  string dbgfile = Cl->getDbgFile();
  if(!dbgfile.empty()){
    for(set<Instruction*>::iterator si = ctrlsites.begin();si!=ctrlsites.end();++si){
      Instruction* currentInstr = *si;
      // print info for each instruction
      this->writeDbgData(currentInstr,Cl,"ctrl");
    }
  }
  return ctrlsites;
}

