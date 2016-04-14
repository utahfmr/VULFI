#include "FunctionList.h"

using namespace llvm;
using namespace std;

FunctionList::FunctionList(Module *M, CLData *Cl){
  this->fiFnDoubleTy=NULL;
  this->fiFnFloatTy=NULL;
  this->fiFnIntTy1=NULL;
  this->fiFnIntTy8=NULL;
  this->fiFnIntTy16=NULL;
  this->fiFnIntTy32=NULL;
  this->fiFnVectorIdx=NULL;
  this->fiFnIntTy64=NULL;
  this->fiFnPrintStats=NULL;
  this->fiFnPrintInjStats=NULL;
  this->fiFnMain = NULL;
  this->fillTargetFnList(M,Cl);
}

inline bool FunctionList::getFnMatch(string matchFnName,
				     string targetFnName, bool mangledFlag){
  if(mangledFlag){
    if(matchFnName.compare(targetFnName) == 0){
      return true;
    }
  } else {
    if(matchFnName.find(targetFnName) != string::npos){
      return true;
    }
  }
  return false;
}

void FunctionList::fillTargetFnList(Module *M, CLData *Cl){
  set<string> fnFindList = Cl->getfnList();
  Module::FunctionListType &functionList = M->getFunctionList();
  // Iterate over module and search for functions matching user provided list
  for (Module::iterator it = functionList.begin(); it != functionList.end(); ++it){
    string fnName = it->getName().str();
    DEBUG(errs() << "\nFunction name: " << fnName);
    errs() << fnName << " ";
    if(fnName.compare(fnMain)==0){ // main() function is never mangled
      this->fiFnMain = it;
    }

    bool mangledFlag = false;
    if(this->isMangledName(fnName)){
      mangledFlag = true;
      DEBUG(errs() << "\nMangled function name: " << fnName);
      fnName = this->getDemangledName(fnName);
      DEBUG(errs() << "\nDemangled function name: " << fnName);
    }

    for(set<string>::iterator si=fnFindList.begin();
	si!=fnFindList.end();++si){
      string fnTargetName = *si;
      if(this->getFnMatch(fnName,fnTargetName,mangledFlag)){
	this->targetFnList.push_back((Function*)it);
      }
    }

    // Fill reference to fault injection functions
    // which are called at runtime
    if(this->getFnMatch(fnName,fnISEIntTy1,mangledFlag)){
      DEBUG(errs() << "\nSuccessfully located injector for IntTy1\n");
      this->fiFnIntTy1 = it;
    } else if(this->getFnMatch(fnName,fnISEIntTy8,mangledFlag)){
      DEBUG(errs() << "\nSuccessfully located injector for IntTy8\n");
      this->fiFnIntTy8 = it;
    } else if(this->getFnMatch(fnName,fnISEIntTy16,mangledFlag)){
      DEBUG(errs() << "\nSuccessfully located injector for IntTy16\n");
      this->fiFnIntTy16 = it;
    } else if(this->getFnMatch(fnName,fnISEIntTy32,mangledFlag)){
      DEBUG(errs() << "\nSuccessfully located injector for IntTy32\n");
      this->fiFnIntTy32 = it;
    } else if(this->getFnMatch(fnName,fnISEIntTy64,mangledFlag)){
      DEBUG(errs() << "\nSuccessfully located injector for IntTy64\n");
      this->fiFnIntTy64 = it;
    } else if(this->getFnMatch(fnName,fnISEFloatTy,mangledFlag)){
      DEBUG(errs() << "\nSuccessfully located injector for FloatTy\n");
      this->fiFnFloatTy = it;
    } else if(this->getFnMatch(fnName,fnISEDoubleTy,mangledFlag)){
      DEBUG(errs() << "\nSuccessfully located injector for DoubleTy\n");
      this->fiFnDoubleTy = it;
    } else if(this->getFnMatch(fnName,fnISEVectorIdx,false)){
      DEBUG(errs() << "\nSuccessfully located injector for VectorIdx\n");
      this->fiFnVectorIdx = it;
    } else if(this->getFnMatch(fnName,fnPrintStats,mangledFlag)){
      DEBUG(errs() << "\nSuccessfully located function reference used \
					for printing fault site statistics\n");
      this->fiFnPrintStats = it;
    } else if(this->getFnMatch(fnName,fnPrintInjStats,mangledFlag)){
      DEBUG(errs() << "\nSuccessfully located function reference used \
					for printing fault injection statistics\n");
      this->fiFnPrintInjStats = it;
    }
  }
  return;
}

// starts with _Z
bool FunctionList::isMangledName(string fnName){
  size_t pos1=0,pos2=0;
  pos1 = fnName.find_first_of('_',0);
  pos2 = fnName.find_first_of('Z',0);
  if(pos1==0 and pos2==1){
    return true;
  }
  return false;
}

string FunctionList::getDemangledName(string mangledName){

  //	return mangledName;
  if(mangledName.empty()){
    errs() << "\nError:mangled name couldn't be an empty string\n";
    exit(-1);
  }

  string demangledName;
  char buffer[1024];
  size_t length;
  int status;
  size_t pos = 0;
  char* result = abi::__cxa_demangle(mangledName.c_str(),buffer,&length,&status);
  if(result){
    demangledName = string(result);
    pos = demangledName.find_first_of('(',0);
    if(pos < demangledName.length()){
      demangledName.erase(pos,demangledName.length()-pos);
      return demangledName;
    }
  }
  return demangledName;
}


void FunctionList::runFnPtrValidator(CLData *Cl){
  if(this->getFiFnIntTy1() == NULL){
    errs() << "\nError:Couldn't find the reference to injectSoftErrorIntTy1() function.";
    errs() << "\nPlease ensure to link Corrupt library bitcode to your target bitcode.\n";
    exit(-1);
  }

  if(this->getFiFnIntTy8() == NULL){
    errs() << "\nError:Couldn't find the reference to injectSoftErrorIntTy8() function.";
    errs() << "\nPlease ensure to link Corrupt library bitcode to your target bitcode.\n";
    exit(-1);
  }

  if(this->getFiFnIntTy16() == NULL){
    errs() << "\nError:Couldn't find the reference to injectSoftErrorIntTy16() function.";
    errs() << "\nPlease ensure to link Corrupt library bitcode to your target bitcode.\n";
    exit(-1);
  }

  if(this->getFiFnIntTy32() == NULL){
    errs() << "\nError:Couldn't find the reference to injectSoftErrorIntTy32() function.";
    errs() << "\nPlease ensure to link Corrupt library bitcode to your target bitcode.\n";
    exit(-1);
  }

  if(this->getFiFnIntTy64() == NULL){
    errs() << "\nError:Couldn't find the reference to injectSoftErrorIntTy64() function.";
    errs() << "\nPlease ensure to link Corrupt library bitcode to your target bitcode.\n";
    exit(-1);
  }

  if(this->getFiFnFloatTy() == NULL){
    errs() << "\nError:Couldn't find the reference to injectSoftErrorFloatTy() function.";
    errs() << "\nPlease ensure to link Corrupt library bitcode to your target bitcode.\n";
    exit(-1);
  }

  if(this->getFiFnDoubleTy() == NULL){
    errs() << "\nError:Couldn't find the reference to injectSoftErrorDoubleTy() function.";
    errs() << "\nPlease ensure to link Corrupt library bitcode to your target bitcode.\n";
    exit(-1);
  }

  if(Cl->getLang().compare("OCL") == 0){
    if(this->getFiFnVectorIdx() == NULL){
      errs() << "\nError:Couldn't find the reference to injectSoftErrorVectorIdx() function.";
      errs() << "\nPlease ensure to link Corrupt library bitcode to your target bitcode.\n";
      exit(-1);
    }
  }

  if(this->getFiFnPrintStats() == NULL){
    errs() << "\nError:Couldn't find the reference to printFaultSitesData() function.";
    errs() << "\nPlease ensure to link Corrupt library bitcode to your target code.\n";
    exit(-1);
  }

  if(this->getFiFnPrintInjStats() == NULL){
    errs() << "\nError:Couldn't find the reference to printFaultInjectionData() function.";
    errs() << "\nPlease ensure to link Corrupt library bitcode to your target code.\n";
    exit(-1);
  }

  if(Cl->getInstMainFlag()){ // if target function is not main()
    if(this->getFiFnMain() == NULL){
      errs() << "\nError:Couldn't find the reference to main() function.";
      errs() << "\nPlease ensure that your target bitcode contain main() definition.\n";
      exit(-1);
    }
  }
}
