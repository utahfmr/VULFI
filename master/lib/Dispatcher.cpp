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
  errs() << "\n==================================================";
  errs() << "\nEnumerating fault sites..";
  if((fsalgo == Common::FS_DATA ||
      fsalgo == Common::FS_DINT ||
      fsalgo == Common::FS_DFLO ) &&
     !Cl->getRangeFlag()){
    DataSites ds;
    set<Instruction*> templist = ds.getfaultSites(M,Cl,this->Fl);
    datafaultSites.insert(templist.begin(),templist.end());
    errs() << "\nTotal no. of pure-data fault sites enumerated: "
	   << datafaultSites.size() << "\n";
  } else if((fsalgo == Common::FS_DATA ||
	     fsalgo == Common::FS_DINT ||
	     fsalgo == Common::FS_DFLO ) &&
	    Cl->getRangeFlag()){
    DataSitesRange dsr;
    set<Instruction*> templist = dsr.getfaultSites(M,Cl,this->Fl);
    datafaultSites.insert(templist.begin(),templist.end());
    errs() << "\nTotal no. of pure-data fault sites enumerated in the "
	   << "user provided range: "
	   << datafaultSites.size() << "\n";
  } else if(fsalgo == Common::FS_CTRL &&
	    !Cl->getRangeFlag()){
    CtrlSites ctrls;
    set<Instruction*> templist = ctrls.getfaultSites(M,Cl,this->Fl);
    ctrlfaultSites.insert(templist.begin(),templist.end());
    errs() << "\nTotal no. of control fault sites enumerated: "
	   << ctrlfaultSites.size() << "\n";
  } else if(fsalgo == Common::FS_VCTL &&
	    !Cl->getRangeFlag()){
    CtrlSites vctrls;
    set<Instruction*> templist = vctrls.getfaultSites(M,Cl,this->Fl);
    vctrlfaultSites.insert(templist.begin(),templist.end());
    errs() << "\nTotal no. of vector control fault sites enumerated: "
	   << vctrlfaultSites.size() << "\n";
  } else if(fsalgo == Common::FS_CTRL &&
	    Cl->getRangeFlag()){
    CtrlSitesRange ctrlsr;
    set<Instruction*> templist = ctrlsr.getfaultSites(M,Cl,this->Fl);
    ctrlfaultSites.insert(templist.begin(),templist.end());
    errs() << "\nTotal no. of control fault sites enumerated in the "
	   << "user provided range: "
	   << ctrlfaultSites.size() << "\n";
  } else if((fsalgo == Common::FS_ADDR) &&
	    !Cl->getRangeFlag()){
    AddrSites addrs;
    set<Instruction*> templist = addrs.getfaultSites(M,Cl,this->Fl);
    addrfaultSites.insert(templist.begin(),templist.end());
    errs() << "\nTotal no. of address fault sites enumerated: "
	   << addrfaultSites.size() << "\n";
  } else if((fsalgo == Common::FS_ADDR) &&
	    Cl->getRangeFlag()){
    AddrSitesRange addrsr;
    set<Instruction*> templist = addrsr.getfaultSites(M,Cl,this->Fl);
    addrfaultSites.insert(templist.begin(),templist.end());
    errs() << "\nTotal no. of pure-data fault sites enumerated in the "
	   << "user provided range: "
	   << addrfaultSites.size() << "\n";
  } else if((fsalgo == Common::FS_MASK)){
    MaskSites masks;
    set<Instruction*> templist = masks.getfaultSites(M,Cl,this->Fl);
    maskfaultSites.insert(templist.begin(),templist.end());
    errs() << "\nTotal no. of mask fault sites enumerated: "
	   << maskfaultSites.size() << "\n";
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
  } else if(fsalgo == Common::FS_ADDR){
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


