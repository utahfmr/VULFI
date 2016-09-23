#include "InstrList.h"

using namespace llvm;
using namespace std;

set<Instruction*> InstrList::genInstrList(set<Instruction*> instrList,
					  CLData *Cl, Common::FSAlgo fsTy){
  //	set<Common::FSAlgo> fsTy = Cl->getFSAlgo();
  set<Instruction*> prunedInstrList;
  map<Instruction*,set<Instruction*> > dulist,udlist;
  Slice* prgSlice = new Slice();

  prgSlice->fillExtdLists(instrList,Cl,false);
  dulist = prgSlice->getExtdDefUseList();
  udlist = prgSlice->getExtdUseDefList();

  #ifdef DEBUG
  errs() << "\n===================Def-Use List==============================\n";
  #endif
  for(map<Instruction*,set<Instruction*> >::iterator mi=dulist.begin();
      mi!=dulist.end();++mi){
    Instruction* currentDefn = &*(mi->first);
    if(dyn_cast<TerminatorInst>(currentDefn)) continue; // fix for the bug reported by Joeph (PNNL) 9/17/2015
    #ifdef DEBUG
    errs() << "\n\nCurrent Definition: " << *(currentDefn);
    errs() << "\n---------------------------------------\n";
    #endif
    set<Instruction*> userlist = mi->second;
    if(fsTy == Common::FS_DATA || fsTy == Common::FS_DINT || fsTy == Common::FS_DFLO){
      // add the current instruction to the pruned list if none of its users are
      // either a control or an address instruction
      bool isValidData = true;
      for(set<Instruction*>::iterator si=userlist.begin();si!=userlist.end();++si){
	Instruction* currentUser = *si;
	if(!InstructionType::isInstrDesiredType(currentUser,fsTy,Common::USER)){
	  #ifdef DEBUG
	  errs() << "\n\nDef Instruction: " << *(currentDefn);
	  errs() << "\nCulprit User Instruction: " << *currentUser;
	  errs() << "\nCulprit User TypeID: " << (*si)->getType()->getTypeID();
	  errs() << "\nCulprit User Val TypeID: " << currentUser->getType()->getTypeID();
	  #endif
	  isValidData = false;
	  break;
	}
	#ifdef DEBUG
	errs() << "\nUser: " << *currentUser;
	errs() << "\nUser TypeID: " << currentUser->getType()->getTypeID();
	#endif
      }
      // add it to the pruned list
      if(isValidData){
		#ifdef DEBUG
		errs() << "\nAccepted Instruction: " << *(currentDefn);
		#endif
	prunedInstrList.insert(currentDefn);
      }
      #ifdef DEBUG
      errs() << "\n---------------------------------------\n";
      #endif
    } else if(fsTy == Common::FS_CTRL || fsTy == Common::FS_VCTL ||fsTy == Common::FS_ADDR){
      // add the current instruction to the pruned list if at least one of its users are
      // either a control / an address instruction
      bool isValidCtrlOrAddr = false;
      for(set<Instruction*>::iterator si=userlist.begin();si!=userlist.end();++si){
	Instruction* currentUser = *si;
	if(InstructionType::isInstrDesiredType(currentDefn,fsTy,Common::DEFN) ||
	   InstructionType::isInstrDesiredType(currentUser,fsTy,Common::USER)){		   
		   #ifdef DEBUG
			errs() << "\n\nCurrent Defn      : " << *(currentDefn);
			errs() << "\nCulprit User        : " << *currentUser;
			errs() << "\nCulprit User TypeID : " << currentUser->getType()->getTypeID();
		  #endif
	  isValidCtrlOrAddr = true;
	  break;
	}
	#ifdef DEBUG
	errs() << "\nUser: " << *currentUser;
	errs() << "\nUser TypeID: " << currentUser->getType()->getTypeID();
	#endif
      }
      // add it to the pruned list
      if(isValidCtrlOrAddr){		  
		  #ifdef DEBUG
		  errs() << "\nAccepted Instruction: " << *(currentDefn);
		  #endif
          prunedInstrList.insert(currentDefn);
      }
      #ifdef DEBUG
      errs() << "\n---------------------------------------\n";
      #endif
    }
  }
  return prunedInstrList;
}
