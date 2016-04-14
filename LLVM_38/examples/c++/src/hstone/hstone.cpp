/*
 * Source: http://rosettacode.org/mw/index.php?title=Hailstone_sequence&oldid=225288
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
#include <vector>
#include <utility>

#ifdef INST
extern "C" int printFaultSitesData(void);
extern "C" int printFaultInjectionData(void);
#endif

std::vector<int> hailstone(int i){
  std::vector<int> v;
  while(true){ 
    v.push_back(i);
    if (1 == i)
      break; 
    i = (i % 2) ? (3 * i + 1) : (i / 2);
  }
  return v;
}
 
std::pair<int,int> find_longest_hailstone_seq(int n){
  std::pair<int, int> maxseq(0, 0);
  int l; 
  for(int i = 1; i < n; ++i){
    l = hailstone(i).size(); 
    if (l > maxseq.second) maxseq = std::make_pair(i, l);
  }   
  return maxseq;
}
 
int main(int argc, char *argv[]){
  FILE *fp;
  if(argc!=2){    
    printf("\nERROR: Insufficient arguments!\n");
    exit(-1);      
  }
  fp=fopen(argv[1],"w");
  std::vector<int> h27;
  h27 = hailstone(27);
  for(int i=0;i<h27.size();i++)
    fprintf(fp,"%d\n",h27[i]);
  std::pair<int,int> m = find_longest_hailstone_seq(100000);
  fprintf(fp,"%d\n",m.first);
  fprintf(fp,"%d\n",m.second);
#ifdef INST
  printFaultSitesData();
  printFaultInjectionData();
#endif
  fclose(fp);
  return 0;
}
