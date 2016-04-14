
#pragma once

// user defined headers
#include "CLData.h"
#include "SiteSelector.h"
#include "CtrlInstrList.h"
#include "FunctionList.h"
#include "Slice.h"
#include "InstructionType.h"

using namespace llvm;
using namespace std;

/*! This class provides an implementation of the abstract class SiteSelector */
class CtrlSites : public SiteSelector {
 public:
  CtrlSites();
  /*! 
    \param M module representing the byte code file
    \param Cl command line options provided by the user
    \param Fl list of functions to be analyzed 
    \return return the list of "control" fault sites (llvm-ir instructions) for 
    the list of functions in \param FunctionList .*/    
  set<Instruction*> getfaultSites(Module *M, CLData *Cl, FunctionList *Fl);

 private:
  map<Instruction*, Function*> instrToFuncTable;
  set<Instruction*> instrList;
  int ctrlInstrCount;
  int vctrlInstrCount;
};

