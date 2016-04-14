
#pragma once

// user defined headers
#include "Common.h"


using namespace llvm;
using namespace std;

/*! \brief This class contains the all inputs/choices provided by the user. */
class CLData
{
 public:
  //! default constructor
  CLData(){}
  
  //! get Architechture type
  Common::ArchType getArch(void){return this->arch;}
  
  //! get fault site selection algorithm chosen by the user
  Common::FSAlgo getFSAlgo(void){return this->fsalg;}

  //! get target function list
  set<string> getfnList(void){return this->fnList;}

  //! get the target (high-level) language (e.g., C,C++,OpenCL etc.).
  string getLang(void){return this->ln;}

  //! whether to target main() function (may be removed in future)
  bool getInstMainFlag(void){return this->instMainFlag;}

  //! do we need to inject faults in a given range?
  bool getRangeFlag(void){return this->rangeFlag;}

  //! file containing the range information
  string getRangeFileName(void){return this->rangeFlName;}

  // Default mode : fault injection mode (DO NOT choose any other mode for now)
  Common::Mode getMode(void){return this->mode;}

  // Protection mode Algorithm (experimental/not recommended for use)
  Common::PCTAlgo getPCTAlgo(void){return this->pctalg;}

  //! incase targeting a single llvm-ir instriction, give the index id
  int getTargetInstrIdx(void){return this->sidx;}

  //! experimental version -- ignore
  int getDepth(void){return this->depth;}

  //! set mode - always set fault injection mode.
  void setMode(string);

  //! ignore
  void setPCTAlgo(string);

  //! IMPORTANT: set the fault site selection algorithm
  void setFSAlgo(string);

  //! set the target architecture: x86 is the default one. 
  void setArch(string archty);

  //! ignore
  void setInstMainFlag(int);

  //! name of a user-level function to be targeted for fault injection
  void setFnList(string);

  //! list of user-level functions to be targeted for fault injection
  void setFnList(set<string>);

  //! set programming language
  void setLang(string);

  //! set whether fault injection to be done in a range
  void setRangeFlag(int);

  //! set filename containing range information
  void setRangeFileName(string);

  //! validates if command line parameters provided by user are correct/supported!
  void runCLValidator();

  //! set name of the debug file containing list of fault sites (llvm-ir instructions)  
  void setDbgFile(string flname){this->dbgFile=flname;}

  //! set target instruction index is a single instruction to be targeted
  void setTargetInstrIdx(int idx){this->sidx=idx;}

  //! ignore
  void setDepth(int dep){this->depth=dep;}

  //! set name of the debug file containing list of fault sites (llvm-ir instructions)
  string getDbgFile(void){return this->dbgFile;}

 private:
  Common::ArchType arch;
  int sidx;
  Common::Mode mode;
  string dbgFile;
  Common::FSAlgo fsalg; 
  Common::PCTAlgo pctalg; 
  bool instMainFlag; 
  bool rangeFlag; 
  set<string> fnList; 
  string ln; 
  string rangeFlName; 
  int depth;

};

