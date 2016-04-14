/*
 * Source: http://rosettacode.org/mw/index.php?title=Floyd%27s_triangle&oldid=224481
 * 
 * License: content is available under GNU Free Documentation License 1.2 unless otherwise noted.
 * 
 * Please refer to GNU Free Documentation License 1.2 @ http://www.gnu.org/licenses/fdl-1.2.html
 * 
 * Please also refer to following link for additional disclaimer: 
 * http://rosettacode.org/wiki/Rosetta_Code:General_disclaimer
 * 
 * */


#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#ifdef INST
extern "C" int printFaultSitesData(void);
extern "C" int printFaultInjectionData(void);
#endif

using namespace std;

typedef char BYTE;

int _rows;
BYTE* lastLineLen=NULL;

void killArray(){
  if( lastLineLen ) 
    delete [] lastLineLen;
}
 
void calculateLastLineLen(){
  killArray();
  lastLineLen = new BYTE[_rows]; 
  int s = 1 + ( _rows * ( _rows - 1 ) ) / 2; 
  for( int x = s, ix = 0; x < s + _rows; x++, ix++ ){
    ostringstream cvr;
    cvr << x;
    lastLineLen[ix] = static_cast<BYTE>( cvr.str().size() );
  }
}
 
void display(const char* fname){
  FILE *fp;
  fp=fopen(fname,"w");
  fprintf(fp,"Floyd\'s Triangle - %d rows\n",_rows);
  fprintf(fp,"===============================================\n");
  int number = 1;
  for( int r = 0; r < _rows; r++ ){
    for( int c = 0; c <= r; c++ ){
      ostringstream cvr;
      cvr << number++;
      string str = cvr.str();
      while( str.length() < lastLineLen[c] )
	str = " " + str;
      fprintf(fp,"%s ",str.c_str());
    }
    fprintf(fp,"\n");
  }
  fclose(fp);
}

void create( int rows , const char* fname){
  _rows = rows;
  calculateLastLineLen();
  display(fname);
}

int main( int argc, char* argv[] ){
  if(argc!=2){    
    printf("\nERROR: Insufficient arguments!\n");
    exit(-1);      
  }  

  int s=100;
  create(s,argv[1]); 	

#ifdef INST
  printFaultSitesData();
  printFaultInjectionData();
#endif  
  killArray();
  return 0;
}
