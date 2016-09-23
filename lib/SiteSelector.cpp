#include "SiteSelector.h"

using namespace llvm;
using namespace std;

void SiteSelector::writeDbgData(Instruction* instr, CLData *Cl, const string fsstring){
  string dbgfile = Cl->getDbgFile();
  // if no dbgfile name provided, exit!
  if(dbgfile.empty()){
    return;
  }

  ofstream fout;
  string flname = fsstring + "_" + dbgfile;

  int vector_flag=0;
  if(instr->getType()->isVectorTy()){
    vector_flag=1;
  }

  bool writeheader_flag = false;
  if(Common::fileExists(flname)){
    writeheader_flag = true;
  }
  fout.open(flname.c_str(),ios::app);

  // if first write to the file then add header
  if(!writeheader_flag){
    fout <<"instruction_name,file_name,line_no,is_vector\n";
  }

  string instrString;
  llvm::raw_string_ostream rso(instrString);
  instr->print(rso);
  Common::replace_all(instrString,",","#");
  // if debug data exists, write it to the dbgfile
  if (DILocation *loc = instr->getDebugLoc()){
    StringRef file = loc->getFilename();
    unsigned line = loc->getLine();    
    if(fout.good()){
      fout << "\"" << instrString << "\",";
      fout << "\"" << file.str() << "\",";
      fout << "\"" << line << "\",";
      fout << vector_flag << "\n";
      fout.close();
    }
  } else {
    if(fout.good()){
      fout << "\"" << instrString << "\"";
      fout << ",,,";
      fout << vector_flag << "\n";
      fout.close();
    }
  }
  return;
}
