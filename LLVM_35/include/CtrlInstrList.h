#pragma once

// user defined headers
#include "CLData.h"
#include "InstrList.h"


using namespace llvm;
using namespace std;

/*! \brief This is a wrapper class derived from InstrList class. It provides an
  API for extracting list of instructions which are either control instruction or
  instructions which must affect at least one contol instruction.*/
class CtrlInstrList : public InstrList
{
 public:
  /*! \param instrList the list of instructions which are to be analyzed. 
    \return list of llvm-ir instructions which are of type control or must affect
    at least one control instruction.*/
  set<Instruction*> genInstrList(set<Instruction*> instrList, CLData *Cl,
				 Common::FSAlgo fsTy);
};

