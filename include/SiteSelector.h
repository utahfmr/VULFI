#pragma once

// user defined headers
#include "CLData.h"
#include "FunctionList.h"

using namespace std;
using namespace llvm;


/*! This is an asbtract class which defines the interface for implementing
  a fault site selection algorithm defined by a user. The current implementation 
  of vulfi has four key implementations for selecting fault sites: pure-data, control, 
  address, and mask."*/
class SiteSelector
{
 public:
  /*! pure virtual function defining the interface for a user-defined implementation 
    for fault site selection. 
    \param M the module representing the llvm-ir of a given byte-code file.
    \param Cl the commandline options provided by the user.
    \param Fl the list of functions in the module \param M whose instructions
    would be considered as part of site selection process.
  */
  virtual set<Instruction*> getfaultSites(Module *M,
					  CLData *Cl,
					  FunctionList *Fl)=0;
  // virtual destructor
  virtual ~SiteSelector(){}

 protected:
  /*! writes to a file the final list of sites selected for fault injection.*/
  void writeDbgData(Instruction* instr, CLData *Cl, const string fsstring);
};

