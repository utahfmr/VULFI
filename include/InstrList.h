#pragma once

// user defined headers
#include "CLData.h"
#include "Slice.h"
#include "InstructionType.h"

using namespace llvm;
using namespace std;

/*! \brief Top-level class which provides an interface for pruning out
 *         instructions from a given list based on user provided
 *         fault site selection algorithm.
 *
 */
class InstrList
{
 protected:
  // pure virtual function (interface)
  virtual set<Instruction*> genInstrList(set<Instruction*> instrList,
					 CLData *Cl, Common::FSAlgo fsTy);

  // virtual destructor
  virtual ~InstrList(){}
};

