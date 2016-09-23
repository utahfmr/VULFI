#pragma once

// c++ system headers
#include <set>
#include <string>

// user defined headers
#include "CLData.h"
#include "Common.h"
#include "FunctionList.h"
#include "X86IntrinsicsDef.h"

using namespace llvm;
using namespace std;

/*! \brief Provides implementations for handling LLVM intrinsics.
 *
 */

class IntrinsicsDef {
 public:
  IntrinsicsDef(Common::ArchType archty);
  static string getIntrinsic(CallInst* instr); // returns the name of the intrinsic
  //int usesMask(Instruction* instr); // if mask exists, returns arg index of the mask reg, returns -1 otherwise
  Value* getMask(CallInst* instr); // get the mask as a Value* if a valid mask exists
  //		static bool usesMask(Instruction* instr);

 private:
  Common::ArchType arch;
  set<string> instrinsicSetNoMask;
  set<string> instrinsicSetMaskArg2;
  set<string> instrinsicSetMaskArg3;
  set<string> instrinsicSetMaskArg4;
  set<string> instrinsicSetBitWiseMaskArg2;
  set<string> instrinsicSetBitWiseMaskArg3;
  set<string> instrinsicSetBitWiseMaskArg4;
  set<string> instrinsicSetBitWiseMaskArg5;
  set<string> instrinsicSetCtrlMaskArg2;
  set<string> instrinsicSetCtrlMaskArg3;
  set<string> instrinsicSetCtrlMaskArg4;
  const string *instrinsicListNoMask;
  const string *instrinsicListMaskArg2;
  const string *instrinsicListMaskArg3;
  const string *instrinsicListMaskArg4;
  const string *instrinsicListBitWiseMaskArg2;
  const string *instrinsicListBitWiseMaskArg3;
  const string *instrinsicListBitWiseMaskArg4;
  const string *instrinsicListBitWiseMaskArg5;
  const string *instrinsicListCtrlMaskArg2;
  const string *instrinsicListCtrlMaskArg3;
  const string *instrinsicListCtrlMaskArg4;
  void populateList(const string *instrinsicList,set<string> &instrinsicSet);
};

