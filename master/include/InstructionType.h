#pragma once

// user defined headers
#include "CLData.h"
#include "FunctionList.h"

using namespace llvm;
using namespace std;

/*! \brief Checks if an instruction confirms to a fault site selection
 *         criteria.
 *
 */
class InstructionType
{
 public:
  static bool isInstrDesiredType(Instruction* instr, Common::FSAlgo fsalgo,
				 Common::TargetType tgTy);
};

