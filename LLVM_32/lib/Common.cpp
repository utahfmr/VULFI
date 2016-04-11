
#include <string>
#include "Common.h"

using namespace llvm;
using namespace std;

Common::Common(){
  int blsz = sizeof(blacklist)/sizeof(blacklist[0]);
  for(int i=0;i<blsz;i++){
    blacklistSet.insert(blacklist[i]);
  }

  set<string>::iterator si;
  for(si=blacklistSet.begin();si!=blacklistSet.end();++si){
    errs() << "\nA new element of the black list is:" << *si << "\n";
  }
}

set<string> Common::splitAtSpace(string spltStr){
  vector<string> strLst;
  istringstream isstr(spltStr);
  copy(istream_iterator<string>(isstr),
       istream_iterator<string>(),
       back_inserter<vector<string> >(strLst));

  set<string> splitList;
  for(unsigned i=0;i<strLst.size();i++){
    splitList.insert(strLst[i]);
  }
  return splitList;
}

vector<string> Common::splitAtSequence(string targetstr,
				       set<char> sequence){

  vector<string> result;
  size_t startpos=0, len=0;
  unsigned i=0;
  for(;i<targetstr.size();i++){
    char c = targetstr[i];
    if(sequence.find(c) != sequence.end()){
      len = i-startpos;
      if(len > 0){
	result.push_back(targetstr.substr(startpos,len));
      }
      startpos=i+1;
    }
  }

  // remove newline character
  size_t newlinepos = targetstr.find('\n',0);
  while(newlinepos != string::npos){
    targetstr.erase(newlinepos,1);
    i--; // adjust the value of i
    newlinepos = targetstr.find('\n',0);
  }

  // process last part of the string
  len = i-startpos;
  if(len > 0){
    result.push_back(targetstr.substr(startpos,len));
  }

  return result;
}

bool Common::fileExists(const string fileName){
  ifstream fl(fileName.c_str());
  if(!fl.good()){
    fl.close();
    return false;
  }
  return true;
}


void Common::replace_all(std::string &strname, const std::string findstr, const std::string repl){
  size_t idx = 0;
  int len = 0;
  while (true) {
    len = findstr.length();
    idx = strname.find(findstr, idx);
    if (idx == std::string::npos){
      break;
    }
    strname.replace(idx, len, repl);    
    idx += len;
  }
  return;
}

Instruction* Common::getInstrByValue(Function* func, Value* val){
  for(inst_iterator it=inst_begin(func),itEnd=inst_end(func);
      it!=itEnd;it++){
    Instruction* currentInstr = &*it;
    string instrName = currentInstr->getName().str();
    string valName = val->getName().str();
    if(instrName.compare(valName)==0){
      if(dyn_cast<PHINode>(currentInstr)==NULL){
	DEBUG(errs() << "\nFound matching instruction: " << *currentInstr);
	return currentInstr;
      }
    }
  }
  return NULL;
}

string Common::instrToString(Instruction* instr){
  string instrString;
  llvm::raw_string_ostream rso(instrString);
  instr->print(rso);
  return instrString;
}
