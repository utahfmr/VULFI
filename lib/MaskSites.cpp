
#include "DataSites.h"
#include "MaskSites.h"
#include "DataInstrList.h"
#include "IntrinsicsDef.h"

using namespace llvm;
using namespace std;

MaskSites::MaskSites(){
  this->maskInstrCount=0;
  this->shufInstrCount=0;
  this->extInstrCount=0;
  this->insInstrCount=0;
  this->vcmpInstrCount=0;
}

set<Instruction*> MaskSites::getfaultSites(Module *M, CLData *Cl, FunctionList *Fl){
  Cl->setFSAlgo("data");
  set<Instruction*> dataInstrList = DataSites::getfaultSites(M,Cl,Fl);
  Cl->setFSAlgo("mask");
  // Iterative over instructions of the target functions
  for(set<Instruction*>::iterator it=dataInstrList.begin();it!=dataInstrList.end();it++){
    Instruction* currentInstr = *it;
    #ifdef DEBUG
    errs() << "\nCurrent instruction: " << *currentInstr;
    #endif
    Function* parentFn = currentInstr->getParent()->getParent();
    //Skip PHI Nodes
    if(dyn_cast<PHINode>(currentInstr) ||
       dyn_cast<LandingPadInst>(currentInstr)){
      continue;
    }

    // Case 1: Status Mask
    // Check for CallInst since mask registers are always used
    // by making a function call to intrinsics
    CallInst* callinst = dyn_cast<CallInst>(currentInstr);
    if(callinst!=NULL){
      IntrinsicsDef intrin = IntrinsicsDef(Cl->getArch());
      Value* __mask = intrin.getMask(callinst);
      if(__mask == NULL){
	continue;
      } else {
	Instruction* maskInstr=Common::getInstrByValue(callinst->getParent()->getParent(),__mask);
	if(maskInstr==NULL){
	  continue;
	}

	this->maskInstrCount++;
	// Build fault site list
	this->instrList.insert(maskInstr);
	// Update instruction to function hash table
	this->instrToFuncTable.insert(pair<Instruction*, Function*>(maskInstr,parentFn));
      }

      // Print to screen initial target instructions
      #ifdef DEBUG
      errs() << "\nInstruction: "<< *currentInstr;
      errs() << "\nTypeID: "<< currentInstr->getType()->getTypeID();
      #endif
    }

    // Case 2: Selection Mask (Shuffle Vector)
    ShuffleVectorInst* shufinstr = dyn_cast<ShuffleVectorInst>(currentInstr);
    if(shufinstr!=NULL){
      this->shufInstrCount++;
      // Build fault site list
      this->instrList.insert(shufinstr);
      // Update instruction to function hash table
      this->instrToFuncTable.insert(pair<Instruction*, Function*>(shufinstr,parentFn));
    }

    // Case 3: ExtractElement index
    ExtractElementInst* extractinstr = dyn_cast<ExtractElementInst>(currentInstr);
    if(extractinstr!=NULL){
      this->extInstrCount++;
      // Build fault site list
      this->instrList.insert(extractinstr);
      // Update instruction to function hash table
      this->instrToFuncTable.insert(pair<Instruction*, Function*>(extractinstr,parentFn));
    }

    // Case 4: InsertElement index
    InsertElementInst* insertinstr = dyn_cast<InsertElementInst>(currentInstr);
    if(insertinstr!=NULL){
      this->insInstrCount++;
      // Build fault site list
      this->instrList.insert(insertinstr);
      // Update instruction to function hash table
      this->instrToFuncTable.insert(pair<Instruction*, Function*>(insertinstr,parentFn));
    }
  }

#ifdef DEBUG
  Slice slice = Slice();
  errs() << "\nInfo: Writing forward slice of each fault site to a file.\n";
  slice.fillExtdLists(instrList,Cl,true);
#endif

  //write debug data if dbgfile name is provided
  string dbgfile = Cl->getDbgFile();
  if(!dbgfile.empty()){
    for(set<Instruction*>::iterator si = instrList.begin();si!=instrList.end();++si){
      Instruction* currentInstr = *si;
      // print info for each instruction
      this->writeDbgData(currentInstr,Cl,"mask");
    }
  }

  errs() << "\n----------------------------------------------------\n";
  errs() << "Number of mask instructions selected:" << this->maskInstrCount << "\n";
  errs() << "Number of shufflevector instructions selected:" << this->shufInstrCount << "\n";
  errs() << "Number of extractelement instructions selected:" << this->extInstrCount << "\n";
  errs() << "Number of insertelement instructions selected:" << this->insInstrCount << "\n";
  errs() << "Number of vector compare instructions selected:" << this->vcmpInstrCount << "\n";
  errs() << "----------------------------------------------------\n";
  return instrList;
}

