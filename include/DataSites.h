
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

/*! \brief This class provides an implementation of the abstract class SiteSelector */
class DataSites : public SiteSelector {
 public:
  /*! 
    \param M module representing the byte code file
    \param Cl command line options provided by the user
    \param Fl list of functions to be analyzed 
    \return return the list of "pure-data" fault sites (llvm-ir instructions) for 
    the list of functions in \param FunctionList .*/
  set<Instruction*> getfaultSites(Module *M, CLData *Cl, FunctionList *Fl);

 private:
  map<Instruction*, Function*> instrToFuncTable;
  set<Instruction*> instrList;
};

