
#include "Common.h"
#include "AddrFaults.h"

using namespace llvm;
using namespace std;

bool AddrFaults::injectFaults(Module *M, set<Instruction*> instrList,
			      CLData* Cl, FunctionList *Fl){
  errs() << "\nInitiating address fault injection instrumentation..\n";
  this->module = M;
  for(set<Instruction*>::iterator si=instrList.begin();
      si!=instrList.end();++si){
    Instruction* currentInstr = *si;
    Type::TypeID tyID = currentInstr->getType()->getTypeID();
    if(tyID == Type::VectorTyID){
      FaultInjector::injectVectorFaults(M,currentInstr,Cl,Fl);
    } else {
      FaultInjector::injectNonVectorFaults(M,currentInstr,Cl,Fl);
    }
  }
  if(Cl->getInstMainFlag()){
    FaultInjector::insertCalltoPrintinMain(Cl,Fl);
  }
  return true;
}
