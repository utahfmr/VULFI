
#include "DataSites.h"
#include "DataInstrList.h"

using namespace llvm;
using namespace std;

set<Instruction*> DataSites::getfaultSites(Module *M, CLData *Cl, FunctionList *Fl){

  // 1st Pass - Build the initial instruction list
  // Get list of target functions
  vector<Function*> fnList = Fl->getTargetFnList();
  Common::FSAlgo fsalg = Common::FS_DATA;
  // Get user's choice for fault site selection
  Common::FSAlgo fsTy = Cl->getFSAlgo();

  // Iterative over instructions of the target functions
  for(unsigned i=0;i<fnList.size();i++){
    for(inst_iterator it=inst_begin(fnList[i]),itEnd=inst_end(fnList[i]);
	it!=itEnd;it++){
      bool updateFlag = false;
      Instruction* currentInstr = &*it;
      //Skip PHI Nodes
      if(dyn_cast<PHINode>(currentInstr) ||
	 dyn_cast<LandingPadInst>(currentInstr)){
	continue;
      }
      // Build initial list of fault sites
      if(fsTy==Common::FS_DATA){ //
	if(InstructionType::isInstrDesiredType(currentInstr,Common::FS_DATA, Common::DEFN)){
	  updateFlag = true;
	  fsalg = Common::FS_DATA;
	}
      } else if(fsTy==Common::FS_DINT){
	if(InstructionType::isInstrDesiredType(currentInstr,Common::FS_DINT, Common::DEFN)){
	  updateFlag = true;
	  fsalg = Common::FS_DINT;
	}
      } else if(fsTy==Common::FS_DFLO){
	if(InstructionType::isInstrDesiredType(currentInstr,Common::FS_DFLO, Common::DEFN)){
	  updateFlag = true;
	  fsalg = Common::FS_DFLO;
	}
      }

      if(updateFlag){
	this->instrList.insert(currentInstr);
	// Update instruction to function hash table
	this->instrToFuncTable.insert(pair<Instruction*, Function*>(currentInstr,fnList[i]));
      }
      // Print to screen initial target instructions
      #ifdef DEBUG
      errs() << "\nInstruction: "<< *currentInstr;
      errs() << "\nTypeID: "<< currentInstr->getType()->getTypeID();
      #endif
    }
  }
  // 2nd/Final Pass - Prune the initial instruction list based on target type
  DataInstrList dInstrList;
  set<Instruction*> datasites = dInstrList.genInstrList(this->instrList,Cl,fsalg);
  //write debug data if dbgfile name is provided
  string dbgfile = Cl->getDbgFile();
  if(!dbgfile.empty()){
    for(set<Instruction*>::iterator si = datasites.begin();si!=datasites.end();++si){
      Instruction* currentInstr = *si;
      // print info for each instruction
      this->writeDbgData(currentInstr,Cl,"data");
    }
  }

#ifdef DEBUG
  Slice slice = Slice();
  errs() << "\nInfo: Writing forward slice of each fault site to a file.\n";
  slice.fillExtdLists(datasites,Cl,true);
#endif

  vector<Instruction*> datasiteslist;
  for(set<Instruction*>::iterator sit = datasites.begin();sit!=datasites.end();sit++){
    datasiteslist.push_back(*sit);
  }

  int staticInstrIdx = Cl->getTargetInstrIdx();
  if(staticInstrIdx <= -1              ||
     staticInstrIdx >= (int) datasiteslist.size()){
    return datasites;
  } else {
    set<Instruction*> idxdatasites;
    idxdatasites.insert(datasiteslist[staticInstrIdx]);
    errs() << "\nInstruction with static id: " << staticInstrIdx << " is: \n"
	   << *datasiteslist[staticInstrIdx];

    return idxdatasites;
  }
}

