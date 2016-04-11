
#pragma once

// c++ system headers
#include <stdlib.h>
#include <map>

// user defined headers
#include "DataSites.h"
#include "Common.h"
#include "ProcessCSV.h"

using namespace llvm;
using namespace std;

/*! \brief Provides an implementation for selecting pure-data fault sites
 *         with in range provided by the user.
 *
 */

class DataSitesRange : public DataSites {
 public:
  set<Instruction*> getfaultSites(Module *M, CLData *Cl, FunctionList *Fl);
  map<string,vector<vector<unsigned> > > getRangeDS(void){return this->rangeDS;}

 private:
  void populateRangeDS(Module *M, CLData *Cl);
  map<string,vector<vector<unsigned> > > rangeDS;
};

