
#pragma once

// user defined headers
#include "CtrlSites.h"
#include "Common.h"
#include "ProcessCSV.h"
#include <stdlib.h>
#include <map>

using namespace llvm;
using namespace std;

/*! \brief Provides an implementation for selecting control fault sites
 *         with in range provided by the user.
 *
 */

class CtrlSitesRange : public CtrlSites {
 public:
  set<Instruction*> getfaultSites(Module *M, CLData *Cl, FunctionList *Fl);
  map<string,vector<vector<unsigned> > > getRangeDS(void){return this->rangeDS;}

 private:
  void populateRangeDS(Module *M, CLData *Cl);
  map<string,vector<vector<unsigned> > > rangeDS;
};

