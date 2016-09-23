
#include "DataInstrList.h"
#include "Slice.h"
#include "InstructionType.h"

using namespace llvm;
using namespace std;

set<Instruction*> DataInstrList::genInstrList(set<Instruction*> instrList,
					      CLData *Cl, Common::FSAlgo fsTy){
  set<Instruction*> prunedInstrList = InstrList::genInstrList(instrList,Cl,fsTy);
  return prunedInstrList;
}
