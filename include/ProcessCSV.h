
#pragma once

// c++ system headers
#include <fstream>

// user defined headers
#include "CLData.h"
#include "Common.h"

using namespace llvm;
using namespace std;


/*! \brief Provides implementations for processing a CSV file,and generating result 
  files in CSV format.*/
class ProcessCSV {
 public:
  //! default constructor
  ProcessCSV(const string fileName);
  vector<string> readColByIdx(unsigned idx);
  vector<string> readRowByIdx(unsigned idx);
  string getColElement(unsigned idx1,unsigned idx2);
  string getRowElement(unsigned idx1,unsigned idx2);
  unsigned getRowCount(void){return this->filecontent.size();}
  unsigned getColCount(void){return this->filecontent[0].size();}
  vector<vector<string> >
    getContent(void){return this->filecontent;}

 private:
  void readFile(const string fileName);
  vector<vector<string> > filecontent;
};

