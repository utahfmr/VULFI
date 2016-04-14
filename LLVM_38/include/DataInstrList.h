
#pragma once

// user defined headers
#include "CLData.h"
#include "InstrList.h"

/*! \brief This is a wrapper class derived from InstrList class. It provides an
  API for extracting list of instructions which are  must not affect any control
  instructions or instructions of type PointerTy. We call these instructions as
  "pure-data instructions."*/
class DataInstrList : public InstrList
{
 public:
 /*! \param instrList the list of instructions which are to be analyzed. 
    \return list of llvm-ir instructions which are "pure-data" instructions.*/    
  set<Instruction*> genInstrList(set<Instruction*> instrList, CLData *Cl,
				 Common::FSAlgo fsTy);
};

