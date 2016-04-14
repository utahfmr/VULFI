
#pragma once

// user defined headers
#include "CLData.h"
#include "SiteSelector.h"
#include "FunctionList.h"
#include "IntrinsicsDef.h"

using namespace llvm;
using namespace std;

/*! */
class FaultInjector
{
 public:
  virtual bool injectFaults(Module *M, set<Instruction*> instrList, CLData* Cl,
			    FunctionList *Fl)=0;
  static Instruction* getNextInstr(Module *M, Instruction* instr);
  static Instruction* getPreviousInstr(Module *M, Instruction* instr);
  
  // virtual destructor
  virtual ~FaultInjector(){}

 protected:
  bool insertCalltoPrintinMain(CLData* Cl, FunctionList *Fl);
  map<BasicBlock*,Instruction*> getExitBBs(CLData* Cl, FunctionList *Fl);
  Instruction* prepareInstrForFaultInjection(Module *M,CLData* Cl,
					     vector<Value*> &arglist,
					     Instruction* instr);
  bool injectNonVectorFaults(Module *M, Instruction* instr, CLData* Cl,
			     FunctionList *Fl);
  bool injectVectorFaults(Module *M, Instruction* instr, CLData* Cl,
			  FunctionList *Fl);
  CallInst* createCallInstByInst(Module *M, Instruction* currentInstr,
				 Instruction* nextInstr, FunctionList* Fl,
				 vector<Value*> arglist, string callFnName);
  CastInst* createCastInstByInst(Module *M, Instruction* instrClone,
				 Instruction* nextInstr,FunctionList* Fl,
				 vector<Value*> arglist, string callFnName);
  IntToPtrInst* createCastPtrInstByInst(Module *M, Instruction* instrClone,
					Instruction* nextInstr,FunctionList* Fl,
					vector<Value*> arglist, string callFnName);
  CallInst* createCallInstByBB(Module *M, Instruction* instr, FunctionList* Fl,
			       vector<Value*> arglist, string callFnName);
  CastInst* createCastInstByBB(Module *M, Instruction* instrClone, FunctionList* Fl,
			       vector<Value*> arglist, string callFnName);
  IntToPtrInst* createCastPtrInstByBB(Module *M, Instruction* instrClone,
				      FunctionList* Fl, vector<Value*> arglist,
				      string callFnName);
  Value* getMaskRegValue(Instruction* instr, CLData* Cl);
  bool matchType(Instruction* instr, Value* __mask);
  Value* getAllMaskOn(Module* M, Instruction* instr);
  Value* getScalarMaskValByInst(Value* __mask, Value* idx, Instruction* instr);
  Value* getScalarMaskValByBB(Value* __mask, Value* idx, BasicBlock* bb);
};

