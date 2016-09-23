#include "Dispatcher.h"

using namespace llvm;
using namespace std;

Dispatcher::Dispatcher(Module *M, CLData *Cl){
  FunctionList *fl = new FunctionList(M,Cl);
  if(fl != NULL){
    fl->runFnPtrValidator(Cl);
  } else {
    errs() << "\nError:Creation of FunctionList object failed!\n";
    exit(-1);
  }

  this->Fl = fl;

  if(Cl->getMode() == Common::INJECT){
    this->populateFaultSites(M,Cl);
    this->runFaultInjector(M,Cl);
  }
}


/*
 * \brief iterates through all the fault site selection algorithm
 * chosen by the user and populates the fault sites corresponding
 * to them.
 *
 * */
void Dispatcher::populateFaultSites(Module *M, CLData *Cl){
  Common::FSAlgo fsalgo = Cl->getFSAlgo();
  if((fsalgo == Common::FS_DATA ||
      fsalgo == Common::FS_DINT ||
      fsalgo == Common::FS_DFLO ) &&
     !Cl->getRangeFlag()){
    DataSites ds;
    set<Instruction*> templist = ds.getfaultSites(M,Cl,this->Fl);
    datafaultSites.insert(templist.begin(),templist.end());
    Common::printFSInfo("pure-data",datafaultSites.size());
  } else if((fsalgo == Common::FS_DATA ||
	     fsalgo == Common::FS_DINT ||
	     fsalgo == Common::FS_DFLO ) &&
	    Cl->getRangeFlag()){
    DataSitesRange dsr;
    set<Instruction*> templist = dsr.getfaultSites(M,Cl,this->Fl);
    datafaultSites.insert(templist.begin(),templist.end());
    Common::printFSInfo("pure-data",datafaultSites.size());
  } else if(fsalgo == Common::FS_CTRL &&
	    !Cl->getRangeFlag()){
    CtrlSites ctrls;
    set<Instruction*> templist = ctrls.getfaultSites(M,Cl,this->Fl);
    ctrlfaultSites.insert(templist.begin(),templist.end());
    Common::printFSInfo("control",ctrlfaultSites.size());
  } else if(fsalgo == Common::FS_VCTL &&
	    !Cl->getRangeFlag()){
    CtrlSites vctrls;
    set<Instruction*> templist = vctrls.getfaultSites(M,Cl,this->Fl);
    vctrlfaultSites.insert(templist.begin(),templist.end());
    Common::printFSInfo("vector control",vctrlfaultSites.size());
  } else if(fsalgo == Common::FS_CTRL &&
	    Cl->getRangeFlag()){
    CtrlSitesRange ctrlsr;
    set<Instruction*> templist = ctrlsr.getfaultSites(M,Cl,this->Fl);
    ctrlfaultSites.insert(templist.begin(),templist.end());
    Common::printFSInfo("control",ctrlfaultSites.size());
  } else if((fsalgo == Common::FS_ADDR ||
		     fsalgo == Common::FS_ADDG ||
			 fsalgo == Common::FS_ADDI) &&
	    !Cl->getRangeFlag()){
    AddrSites addrs;
    set<Instruction*> templist = addrs.getfaultSites(M,Cl,this->Fl);
    addrfaultSites.insert(templist.begin(),templist.end());
    if(fsalgo == Common::FS_ADDR)
    	Common::printFSInfo("address",addrfaultSites.size());
    if(fsalgo == Common::FS_ADDG)
        Common::printFSInfo("address-gep",addrfaultSites.size());
    if(fsalgo == Common::FS_ADDI)
        Common::printFSInfo("address-idx",addrfaultSites.size());
  } else if((fsalgo == Common::FS_ADDR ||
		  	 fsalgo == Common::FS_ADDG ||
			 fsalgo == Common::FS_ADDI) &&
	    Cl->getRangeFlag()){
    AddrSitesRange addrsr;
    set<Instruction*> templist = addrsr.getfaultSites(M,Cl,this->Fl);
    addrfaultSites.insert(templist.begin(),templist.end());
    if(fsalgo == Common::FS_ADDR)
    	Common::printFSInfo("address",addrfaultSites.size());
    if(fsalgo == Common::FS_ADDG)
        Common::printFSInfo("address-gep",addrfaultSites.size());
    if(fsalgo == Common::FS_ADDI)
        Common::printFSInfo("address-idx",addrfaultSites.size());
  } else if((fsalgo == Common::FS_MASK)){
    MaskSites masks;
    set<Instruction*> templist = masks.getfaultSites(M,Cl,this->Fl);
    maskfaultSites.insert(templist.begin(),templist.end());
    Common::printFSInfo("mask",maskfaultSites.size());
  }
  
  return;
}

void Dispatcher::runFaultInjector(Module *M, CLData *Cl){
  Common::FSAlgo fsalgo = Cl->getFSAlgo();
  if(fsalgo == Common::FS_DATA ||
     fsalgo == Common::FS_DINT ||
     fsalgo == Common::FS_DFLO ){
    if(!datafaultSites.empty()){
      DataFaults df;
      df.injectFaults(M,datafaultSites,Cl,this->Fl);
    }
  } else if(fsalgo == Common::FS_CTRL){
    if(!ctrlfaultSites.empty()){
      CtrlFaults cf;
      cf.injectFaults(M,ctrlfaultSites,Cl,this->Fl);
    }
  } else if(fsalgo == Common::FS_VCTL){
    if(!ctrlfaultSites.empty()){
      CtrlFaults cf;
      cf.injectFaults(M,vctrlfaultSites,Cl,this->Fl);
    }
  } else if(fsalgo == Common::FS_ADDR ||
	    fsalgo == Common::FS_ADDG ||
	    fsalgo == Common::FS_ADDI){
    if(!addrfaultSites.empty()){
      AddrFaults af;
      af.injectFaults(M,addrfaultSites,Cl,this->Fl);
    }
  } else if(fsalgo == Common::FS_MASK){
    if(!maskfaultSites.empty()){
      MaskFaults mf;
      mf.injectFaults(M,maskfaultSites,Cl,this->Fl);
    }
  }
  return;
}


