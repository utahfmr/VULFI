
#include "AddrSites.h"
#include "AddrInstrList.h"

using namespace llvm;
using namespace std;

set<Instruction*> AddrSites::getfaultSites(Module *M, CLData *Cl, FunctionList *Fl){  
  // 1st Pass - Build the initial instruction list
  // Get list of target functions
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
      
      // Avoid IntegerTy(8) as there is not such type in C/C++ or ISPC
      Type* ty = currentInstr->getType();
      Type::TypeID tyID = ty->getTypeID();
      if(tyID == Type::VectorTyID){
	Type* ety = currentInstr->getType()->getVectorElementType();
	if(ety->isIntegerTy(8)){
	  continue;
	}
      } else if(ty->isIntegerTy(8)) {
	continue;
      }
      
      // Build initial list of fault sites
      this->instrList.insert(currentInstr);
      // Update instruction to function hash table
      this->instrToFuncTable.insert(pair<Instruction*, Function*>(currentInstr,fnList[i]));
      // Print to screen initial target instructions
      DEBUG(errs() << "\nInstruction: "<< *currentInstr);
      DEBUG(errs() << "\nTypeID: "<< currentInstr->getType()->getTypeID());
    }
  }
  
  // 2nd/Final Pass - Update the initial instruction list
  AddrInstrList aInstrList;
  set<Instruction*> addrsites = aInstrList.genInstrList(this->instrList,Cl,Common::FS_ADDR);
  //write debug data if dbgfile name is provided
  string dbgfile = Cl->getDbgFile();
  if(!dbgfile.empty()){
    for(set<Instruction*>::iterator si = addrsites.begin();si!=addrsites.end();++si){
      Instruction* currentInstr = *si;
      // print info for each instruction
      this->writeDbgData(currentInstr,Cl,"addr");
    }
  }
  return addrsites;
}

