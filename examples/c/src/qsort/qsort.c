
/*
 * Source: http://rosettacode.org/mw/index.php?title=Sorting_algorithms/Quicksort&oldid=223058
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

void quick_sort(int *a, int n) {
  int i, j, p, t;
  if (n < 2)
    return;
  p = a[n / 2];
  for (i = 0, j = n - 1;; i++, j--) {
    while (a[i] < p)
      i++;
    while (p < a[j])
      j--;
    if (i >= j)
      break;
    t = a[i];
    a[i] = a[j];
    a[j] = t;
  }
  quick_sort(a, i);
  quick_sort(a + i, n - i);
}
 
int main (int argc, char *argv[]) {
  if(argc!=2){    
    printf("\nERROR: Insufficient arguments!\n");
    exit(-1);      
  }
  FILE *fp;
  fp=fopen(argv[1],"w");
  int a[] = {4, 65, 2, -31, 0, 99, 2, 83, 782, 1};
  int n = sizeof a / sizeof a[0];
  int i;
  for (i = 0; i < n; i++){
#ifdef DEBUG
    printf("%d%s", a[i], i == n - 1 ? "\n" : " ");
#endif
  }
  quick_sort(a, n);
  for (i = 0; i < n; i++){
#ifdef DEBUG
    printf("%d%s", a[i], i == n - 1 ? "\n" : " ");
#endif    
    fprintf(fp,"%d\n",a[i]);    
  }
#ifdef INST
  printFaultSitesData();
  printFaultInjectionData();
#endif
  fclose(fp);
  return 0;
}
