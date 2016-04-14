#pragma once

// c++ system headers
#include <algorithm>
#include <iterator>

// user defined headers
#include "CLData.h"
#include "DataFaults.h"
#include "DataSites.h"
#include "DataSitesRange.h"
#include "AddrFaults.h"
#include "AddrSites.h"
#include "AddrSitesRange.h"
#include "CtrlFaults.h"
#include "CtrlSites.h"
#include "CtrlSitesRange.h"
#include "MaskFaults.h"
#include "MaskSites.h"
#include "FunctionList.h"

using namespace llvm;
using namespace std;

/*! \brief Orchestrates the process of fault site selection,
 *         fault injection, and result generation.
 *
 */

class Dispatcher
{
 public:
  Dispatcher(Module *M,  CLData *Cl);
  ~Dispatcher(){delete this->Fl;}
 private:
  FunctionList* Fl;
  set<Instruction*> datafaultSites;
  set<Instruction*> ctrlfaultSites;
  set<Instruction*> vctrlfaultSites;
  set<Instruction*> addrfaultSites;
  set<Instruction*> maskfaultSites;
  set<Instruction*> brdcfaultSites;

  void populateFaultSites(Module *M, CLData *Cl);
  void runFaultInjector(Module *M, CLData *Cl);
  void protectFaultSites(Module *M, CLData *Cl);
  void remCommon(set<Instruction*>& c,set<Instruction*>& s);
};

