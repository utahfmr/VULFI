
#include "CLData.h"

using namespace llvm;
using namespace std;

void CLData::setFSAlgo(string algo){
  if(algo.compare("data") == 0){
    this->fsalg=Common::FS_DATA;
  } else if(algo.compare("dint") == 0){
    this->fsalg=Common::FS_DINT;
  } else if(algo.compare("dflo") == 0){
    this->fsalg=Common::FS_DFLO;
  } else if(algo.compare("addr") == 0){
    this->fsalg=Common::FS_ADDR;
  }else if(algo.compare("addg") == 0){
	this->fsalg=Common::FS_ADDG;
  }else if(algo.compare("addi") == 0){
	this->fsalg=Common::FS_ADDI;
  } else if(algo.compare("ctrl") == 0){
    this->fsalg=Common::FS_CTRL;
  } else if(algo.compare("vctl") == 0){
    this->fsalg=Common::FS_VCTL;
  } else if(algo.compare("mask") == 0){
    this->fsalg=Common::FS_MASK;
  } else if(algo.compare("brdc") == 0){
    this->fsalg=Common::FS_BRDC;
  } else {
    this->fsalg=Common::FS_ERROR;
  }
}

void CLData::setMode(string md){
  if(md.compare("inj") == 0){
    this->mode = Common::INJECT;
  } else if(md.compare("pct") == 0){
    this->mode = Common::PROTECT;
  }
  return;
}

void CLData::setArch(string archty){
  if(archty.compare("x86") == 0){
    this->arch = Common::X86;
  } else if(archty.compare("neon") == 0){
    this->arch = Common::NEON;
  } else if(archty.compare("nvvm") == 0){
    this->arch = Common::NVVM;
  } else if(archty.compare("mips") == 0){
    this->arch = Common::MIPS;
  } else if(archty.compare("spu") == 0){
    this->arch = Common::SPU;
  }
}

void CLData::setFnList(string list){
  this->fnList = Common::splitAtSpace(list);
}

void CLData::setFnList(set<string> list){
  this->fnList = list;
}

void CLData::setLang(string l){
  this->ln = l;
}

void CLData::setRangeFlag(int rf){
  #ifdef DEBUG
  errs() << "\nValue of rf: " << rf;
  #endif
  if(rf == 1){
    this->rangeFlag=true;
  } else{
    this->rangeFlag=false;
  }
}

void CLData::setRangeFileName(string flName){
  this->rangeFlName = flName;
}

void CLData::setInstMainFlag(int imnFlag){
  if(imnFlag){
    this->instMainFlag=true;
  } else{
    this->instMainFlag=false;
  }
}

void CLData::runCLValidator(){

  //Verify fault site selection algo input
  Common::FSAlgo fsalgo = this->getFSAlgo();
  if(fsalgo == Common::FS_ERROR){
    errs() << "\nError:Invalid value for fault site selection algo!\n";
    exit(-1);
  }

  //Verify target language
  if(this->getLang().compare("C")!=0   &&
     this->getLang().compare("C++")!=0 &&
     this->getLang().compare("ISPC")!=0 &&
     this->getLang().compare("OCL")!=0){
    errs() << "\nError:Invalid target language!\n";
    exit(-1);
  }

  // verify range flag and associated file input
  if(this->getfnList().empty()){
    errs() << "\nError:Target function list couldn't be empty, "
      "please enter at least one function name!\n";
    exit(-1);
  }
}


