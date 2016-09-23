#pragma once

// c++ system headers
#include <cxxabi.h>

// user defined headers
#include "CLData.h"

using namespace llvm;
using namespace std;

const string fnMain = "main";
const string fnPrintStats = "printFaultSitesData";
const string fnPrintInjStats = "printFaultInjectionData";
const string fnISEIntTy1 = "injectSoftErrorIntTy1";
const string fnISEIntTy8 = "injectSoftErrorIntTy8";
const string fnISEIntTy16 = "injectSoftErrorIntTy16";
const string fnISEIntTy32 = "injectSoftErrorIntTy32";
const string fnISEVectorIdx = "injectSoftErrorVectorIdx";
const string fnISEIntTy64 = "injectSoftErrorIntTy64";
const string fnISEFloatTy = "injectSoftErrorFloatTy";
const string fnISEDoubleTy = "injectSoftErrorDoubleTy";


/*! \brief Provides the references to runtime fault injection functions.
 *
 */
class FunctionList
{
 public:
  FunctionList(Module *M, CLData *Cl);
  vector<Function*> getTargetFnList(void){return this->targetFnList;}
  Function* getFiFnFloatTy(void){return this->fiFnFloatTy;}
  Function* getFiFnDoubleTy(void){return this->fiFnDoubleTy;}
  Function* getFiFnVectorIdx(void){return this->fiFnVectorIdx;}
  Function* getFiFnIntTy1(void){return this->fiFnIntTy1;}
  Function* getFiFnIntTy8(void){return this->fiFnIntTy8;}
  Function* getFiFnIntTy16(void){return this->fiFnIntTy16;}
  Function* getFiFnIntTy32(void){return this->fiFnIntTy32;}
  Function* getFiFnIntTy64(void){return this->fiFnIntTy64;}
  Function* getFiFnPrintStats(void){return this->fiFnPrintStats;}
  Function* getFiFnPrintInjStats(void){return this->fiFnPrintInjStats;}
  Function* getFiFnMain(void){return this->fiFnMain;}
  Function* getPctFnFEAllocaPre(void){return this->pctFnFEAllocaPre;}
  Function* getPctFnFEAllocaPost(void){return this->pctFnFEAllocaPost;}
  Function* getPctFnFEFullBodyPre(void){return this->pctFnFEFullBodyPre;}
  Function* getPctFnFEFullBodyPost(void){return this->pctFnFEFullBodyPost;}
  Function* getPctFnFEPartialPre(void){return this->pctFnFEPartialPre;}
  void updateTargetFnList(Module *M, CLData *Cl){this->fillTargetFnList(M,Cl);}
  void runFnPtrValidator(CLData *Cl);
  static bool isMangledName(string fnName);
  static string getDemangledName(string mangledName);
 private:
  void fillTargetFnList(Module *M, CLData *Cl);
  inline bool getFnMatch(string matchFnName, string targetFnName, bool mangledFlag);
  vector<Function*> targetFnList;
  Function* fiFnFloatTy;
  Function* fiFnDoubleTy;
  Function* fiFnIntTy1;
  Function* fiFnIntTy8;
  Function* fiFnIntTy16;
  Function* fiFnIntTy32;
  Function* fiFnVectorIdx;
  Function* fiFnIntTy64;
  Function* fiFnPrintStats;
  Function* fiFnPrintInjStats;
  Function* fiFnMain;
  Function* pctFnFEAllocaPre;
  Function* pctFnFEAllocaPost;
  Function* pctFnFEFullBodyPre;
  Function* pctFnFEFullBodyPost;
  Function* pctFnFEPartialPre;
};

