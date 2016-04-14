
#include "Common.h"
#include "DataFaults.h"

using namespace llvm;
using namespace std;

bool DataFaults::injectFaults(Module *M, set<Instruction*> instrList,
			      CLData* Cl, FunctionList *Fl){
  errs() << "\nInitiating data fault injection instrumentation..\n";
  this->module = M;
  for(set<Instruction*>::iterator si=instrList.begin();
      si!=instrList.end();++si){
    Instruction* currentInstr = *si;
    Type::TypeID tyID = currentInstr->getType()->getTypeID();
    if(tyID==Type::IntegerTyID ||
       tyID==Type::FloatTyID   ||
       tyID==Type::DoubleTyID){
      FaultInjector::injectNonVectorFaults(M,currentInstr,Cl,Fl);
    } else if(tyID==Type::VectorTyID){
      FaultInjector::injectVectorFaults(M,currentInstr,Cl,Fl);
    }
  }
  if(Cl->getInstMainFlag()){
    FaultInjector::insertCalltoPrintinMain(Cl,Fl);
  }
  return true;
}

