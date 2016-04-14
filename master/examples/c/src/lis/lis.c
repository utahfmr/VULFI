/*
 * Source: http://rosettacode.org/mw/index.php?title=Longest_increasing_subsequence&oldid=223688
 * 
 * License: content is available under GNU Free Documentation License 1.2 unless otherwise noted.
 * 
 * Please refer to GNU Free Documentation License 1.2 @ http://www.gnu.org/licenses/fdl-1.2.html
 * 
 * Please also refer to following link for additional disclaimer: 
 * http://rosettacode.org/wiki/Rosetta_Code:General_disclaimer
 * 
 * */

#include <stdio.h>
#include <stdlib.h>

#ifdef INST
extern int printFaultSitesData(void);
extern int printFaultInjectionData(void);
#endif
 
struct node {
  int val, len;
  struct node *next;
};
 
void lis(int *v, int len, const char* fname){
  int i;
  FILE *fp;  
  struct node *p, *n = calloc(len, sizeof *n);
  for (i = 0; i < len; i++)
    n[i].val = v[i];
 
  for (i = len; i--; ) {
    // find longest chain that can follow n[i]
    for (p = n + i; p++ < n + len; ) {
      if (p->val > n[i].val && p->len >= n[i].len) {
	n[i].next = p;
	n[i].len = p->len + 1;
      }
    }
  }
 
  // find longest chain
  for (i = 0, p = n; i < len; i++)
    if (n[i].len > p->len) p = n + i;
  
  fp=fopen(fname,"w");
  do fprintf(fp,"%d\n", p->val); while ((p = p->next));
  fclose(fp);
  free(n);
}
 
int main(int argc, char *argv[]){
  if(argc!=2){    
    printf("\nERROR: Insufficient arguments!\n");
    exit(-1);      
  }
  
  int x[] = { 3, 2, 6, 4, 5, 1 };
  int y[] = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15 }; 
  lis(x,sizeof(x) / sizeof(int),argv[1]);
  lis(y,sizeof(y) / sizeof(int),argv[1]);
#ifdef INST
  printFaultSitesData();
  printFaultInjectionData();
#endif  	
  return 0;
}
