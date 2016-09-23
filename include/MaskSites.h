
#pragma once

// user defined headers
#include "CLData.h"
#include "SiteSelector.h"
#include "DataInstrList.h"
#include "FunctionList.h"
#include "Slice.h"
#include "InstructionType.h"

using namespace llvm;
using namespace std;

/*! This class provides an implementation of the abstract class SiteSelector */
class MaskSites : public DataSites {
 public:
  MaskSites();
  /*! 
    \param M module representing the byte code file
    \param Cl command line options provided by the user
    \param Fl list of functions to be analyzed 
    \return return the list of "vector-mask" fault sites (llvm-ir instructions) for 
    the list of functions in \param FunctionList .*/    
  set<Instruction*> getfaultSites(Module *M, CLData *Cl, FunctionList *Fl);

 private:
  Instruction* getInstrFromMaskReg(Instruction* instr, Value* __mask);
  map<Instruction*, Function*> instrToFuncTable;
  set<Instruction*> instrList;
  int maskInstrCount;
  int shufInstrCount;
  int extInstrCount;
  int insInstrCount;
  int vcmpInstrCount;
};

