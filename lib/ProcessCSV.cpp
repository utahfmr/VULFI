
#include "ProcessCSV.h"

using namespace llvm;
using namespace std;

ProcessCSV::ProcessCSV(const string fileName){
  this->readFile(fileName);
}

vector<string> ProcessCSV::readColByIdx(unsigned idx){
  vector<string> col;
  for(unsigned i=0;i<this->filecontent.size();i++){
    vector<string> currentrow = this->filecontent[i];
    col.push_back(currentrow[idx]);
  }
  return col;
}

vector<string> ProcessCSV::readRowByIdx(unsigned idx){

  return this->filecontent[idx];
}

string ProcessCSV::getColElement(unsigned idx1,unsigned idx2){
  vector<string> col;
  for(unsigned i=0;i<this->filecontent.size();i++){
    vector<string> currentrow = this->filecontent[i];
    col.push_back(currentrow[idx1]);
  }
  return col[idx2];
}

string ProcessCSV::getRowElement(unsigned idx1,unsigned idx2){
  return this->filecontent[idx1][idx2];
}

void ProcessCSV::readFile(const string fileName){

  if(!Common::fileExists(fileName)){
    errs() << "\nError:Entered CSV file does not exist\n";
    exit(-1);
  }

  ifstream fl(fileName.c_str());
  string line;

  // skip the first line containing field names
  getline(fl, line);
  set<char> sequence;
  sequence.insert(',');
  sequence.insert(';');
  vector<string> headerrow = Common::splitAtSequence(line,sequence);
  unsigned numCol = headerrow.size();

  while (getline(fl, line)){
    vector<string> row = Common::splitAtSequence(line,sequence);

    // validate current row
    if(row.size() != numCol){
      errs() << "\nError:Some of the rows are missing data/fields.\n";
      exit(-1);
    }

    // save current row
    this->filecontent.push_back(row);
  }

  fl.close();
  return;
}
