
#pragma once

// user defined headers
#include "CLData.h"
#include "InstrList.h"

using namespace llvm;
using namespace std;

/*! \brief This is a wrapper class derived from InstrList class. It provides an
  API for extracting list of instructions which are either of type PointerTy or
  instructions which affect at least one instruction of type PointerTy. Usually
  instructions such as getelementptr or inttoptr produce values of type PointerTy */
class AddrInstrList : public InstrList
{
 public:
 /*! \param instrList the list of instructions which are to be analyzed. 
    \return list of llvm-ir instructions which are of type PointerTy or must affect
    at least one instruction of type PointerTy.*/  
  set<Instruction*> genInstrList(set<Instruction*> instrList, CLData *Cl,
				 Common::FSAlgo fsTy);
  inline bool isDataType(Value* instr, CLData* Cl);
};

