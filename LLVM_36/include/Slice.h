#pragma once

// c++ system headers
#include <climits>

// user defined headers
#include "CLData.h"

/*! \brief This class provides APIs for computing backward and forward program slices
  for any given llvm-ir instruction. It is used by classes such as DataSites, CtrlSites, 
  and AddrSites for pruning the initial list of fault sites. It also exposes utility APIs
  for generating the dependency graph etc."*/
class Slice {
 public:
  /*!default constructor*/
  Slice();
  
  /*!Top-level function to obtain forward or backward program slice.
    \param instrList ; computes the forward/backward slice for each instruction in 
    the instrList based on choice provided in \param Cl .
    \param Cl the commandline option provided by the user.
    \param writeFlag switch for writing to file.
   \return a map with each key representing an instruction and its value representing 
    a list of instructions representing its backward or forward slice.*/
  void fillExtdLists(set<Instruction*> instrList, CLData *Cl, bool writeFlag);
  
  /*!Get the list of all definitions (direct/indirect) which has a given 
    llvm-ir instruction as a user*/
  map<Instruction*,set<Instruction*> >
    &getExtdUseDefList(void){return this->extdUseDefList;}
  
  /*!Get the list of all users (direct/indirect) of a given llvm-ir instruction 
    definition*/
  map<Instruction*,set<Instruction*> >
    &getExtdDefUseList(void){return this->extdDefUseList;}

  /*! writes forward slice to a file*/
  void writeFwdSlice(set<Instruction*>& instrlist, string filename, CLData* Cl);
  
 private:
  
  bool isSelectInstr(Instruction* node);
  bool isMaskedControlInstr(Instruction* node);
  bool isMaskedLoad(Instruction* node);
  bool isMaskedStore(Instruction* node);
  void calculateRank(Instruction* node, int rank);
  void writeDAG(Instruction* node);

  /*! experimental function, may be removed in future version*/
  void writeRank(Instruction* node);

  /*! checks if the current instructions has any users*/
  bool isTerminalNode(Instruction* node);
  
  double rank; //! may be removed in future versions

  /*!simple graph traversal routine for travesing the use-def/def-use chain of an 
   Instruction.*/
  void traverseGraph(Instruction* node);

  /*!simple graph traversal routine for travesing the use-def/def-use chain of an 
   Instruction with option for writing the graph to a file.*/    
  void traverseGraph(Instruction* node, ofstream& fout, map<string,int>& edge);

  set<Instruction*> visitedNodes;

  /*! extended user-definitions list*/
  map<Instruction*,set<Instruction*> > extdUseDefList;

  /*! extended definition-users list*/
  map<Instruction*,set<Instruction*> > extdDefUseList;
};



