
#pragma once

// c++ system headers
#include <functional>
#include <string>

// user defined headers
#include "CLData.h"
#include "FunctionList.h"
#include "Slice.h"
#include "InstructionType.h"


using namespace llvm;
using namespace std;

/*! \brief Provides an implementation for selecting pure-data fault sites.
 *
 */

class InstrSeqGen {
 public:
  InstrSeqGen();
  void printSequences(vector<vector<Instruction*> > seqmap, CLData* Cl);
  vector<vector<Instruction*> > genInstrSequence(Module *M, CLData *Cl, FunctionList *Fl);
  void instrumentSeq(Module *M, CLData *Cl, FunctionList *Fl, vector<vector<Instruction*> > seqmap);

 private:
  vector<vector<Instruction*> >
    validateSeqs(vector<vector<Instruction*> > seqmap, int depth);
  void updateSeqMap(Instruction* instr, vector<vector<Instruction*> >& seqmap, int depth, BasicBlock* parent);
  set<Instruction*> instrList;
  set<string> visitedSeq;
  vector<Instruction*> instrseq;
  string getStringSeq(vector<Instruction*> instrseq);
  vector<string> getOperandNameList(map<Value*, string> instrregmap, Instruction* instr, int* regcounter);
  string getSeqFingerPrint(vector<Instruction*> instrseq);
};

