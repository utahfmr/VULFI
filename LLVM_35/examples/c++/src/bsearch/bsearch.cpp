
/*
 * Source: http://rosettacode.org/mw/index.php?title=Binary_search&oldid=225237
 * 
 * License: content is available under GNU Free Documentation License 1.2 unless otherwise noted.
 * 
 * Please refer to GNU Free Documentation License 1.2 @ http://www.gnu.org/licenses/fdl-1.2.html
 * 
 * Please also refer to following link for additional disclaimer: 
 * http://rosettacode.org/wiki/Rosetta_Code:General_disclaimer
 * 
 * */


#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#ifdef INST
extern "C" int printFaultSitesData(void);
extern "C" int printFaultInjectionData(void);
#endif

int binsearch(int array[], int len, int what){
  if (len == 0) return -1;
  int mid = len / 2;
  if (array[mid] == what) return mid;
  if (array[mid] < what) {
    int result = binsearch(array+mid+1, len-(mid+1), what);
    if (result == -1) return -1;
    else return result+mid+1;
  }
  if (array[mid] > what)
    return binsearch(array, mid, what);
  return -1;
}
 

int main(int argc, char *argv[]){
  FILE *fp;
  if(argc!=2){    
    printf("\nERROR: Insufficient arguments!\n");
    exit(-1);      
  }
  fp=fopen(argv[1],"w");  
  int array[] = {2, 3, 5, 6, 8};
  int result1 = binsearch(array, sizeof(array)/sizeof(int), 4);
  int result2 = binsearch(array, sizeof(array)/sizeof(int), 8);
  
  if (result1 == -1)
    fprintf(fp,"1\n");  
  else
    fprintf(fp,"0\n");

  if (result2 == -1)
    fprintf(fp,"1\n");  
  else
    fprintf(fp,"0\n");
  
#ifdef INST
  printFaultSitesData();
  printFaultInjectionData();
#endif
  
  fclose(fp); 
  return 0;
}
