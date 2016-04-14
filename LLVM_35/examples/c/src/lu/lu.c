

/*
 * Source: http://rosettacode.org/mw/index.php?title=LU_decomposition&oldid=224919
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
#include <math.h>

#ifdef INST
extern int printFaultSitesData(void);
extern int printFaultInjectionData(void);
#endif

#define foreach(a, b, c) for (int a = b; a < c; a++)
#define for_i foreach(i, 0, n)
#define for_j foreach(j, 0, n)
#define for_k foreach(k, 0, n)
#define for_ij for_i for_j
#define for_ijk for_ij for_k
#define _dim int n
#define _swap(x, y) { typeof(x) tmp = x; x = y; y = tmp; }
#define _sum_k(a, b, c, s) { s = 0; foreach(k, a, b) s+= c; }
#define _QUOT(x) #x
#define QUOTE(x) _QUOT(x)
#define _mul(a, b) mat_mul(a, b, n)
#define _zero(a) mat_zero(a, n)
#define _new(a) a = mat_new(n)
#define _copy(a) mat_copy(a, n)
#define _del(x) mat_del(x)
#define _LU(a, l, u, p) mat_LU(a, l, u, p, n)
#define _pivot(a, b) mat_pivot(a, b, n)

typedef double **mat;

double A3[][3] = {{ 1, 3, 5 }, { 2, 4, 7 }, { 1, 1, 0 }};
double A4[][4] = {{11, 9, 24, 2}, {1, 5, 2, 6}, {3, 17, 18, 1}, {2, 5, 7, 1}};
 
void mat_zero(mat x, int n) {
  for_ij x[i][j] = 0;
}
 
mat mat_new(_dim){
  mat x = malloc(sizeof(double*) * n);
  x[0]  = malloc(sizeof(double) * n * n); 
  for_i x[i] = x[0] + n * i;
  _zero(x); 
  return x;
}
 
mat mat_copy(void *s, _dim){
  mat x = mat_new(n);
  for_ij x[i][j] = ((double (*)[n])s)[i][j];
  return x;
}
 

void mat_del(mat x) {
  free(x[0]);
  free(x);
}
 

mat mat_mul(mat a, mat b, _dim){
  mat c = _new(c);
  for_ijk c[i][j] += a[i][k] * b[k][j];
  return c;
}
 
void mat_pivot(mat a, mat p, _dim){
  for_ij { p[i][j] = (i == j); }
  for_i  {
    int max_j = i;
    foreach(j, i, n)
      if (fabs(a[j][i]) > fabs(a[max_j][i])) max_j = j;
 
    if (max_j != i)
      for_k { _swap(p[i][k], p[max_j][k]); }
  }
}

void mat_write(mat a, int n, const char* fname, int open_mode){
  int i,j;
  FILE *fp;
  if(open_mode==0)
    fp=fopen(fname,"w");
  else
    fp=fopen(fname,"a");
  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      fprintf(fp,"%lf\n",a[i][j]);
    }
  }
  fclose(fp);
}

void mat_LU(mat A, mat L, mat U, mat P, _dim){
  _zero(L); _zero(U);
  _pivot(A, P); 
  mat Aprime = _mul(P, A); 
  for_i  { L[i][i] = 1; }
  for_ij {
    double s;
    if (j <= i) {
      _sum_k(0, j, L[j][k] * U[k][i], s)
	U[j][i] = Aprime[j][i] - s;
    }
    if (j >= i) {
      _sum_k(0, i, L[j][k] * U[k][i], s);
      L[j][i] = (Aprime[j][i] - s) / U[i][i];
    }
  } 
  _del(Aprime);
}
 
int main(int argc, char *argv[]){
  if(argc!=2){    
    printf("\nERROR: Insufficient arguments!\n");
    exit(-1);      
  }
  
  int n = 3;
  mat A, L, P, U; 
  _new(L);
  _new(P);
  _new(U);
  A = _copy(A3);
  _LU(A, L, U, P);
  mat_write(A,n,argv[1],0);
  mat_write(L,n,argv[1],1);
  mat_write(U,n,argv[1],1);
  mat_write(P,n,argv[1],1);
  _del(A);
  _del(L);
  _del(U);
  _del(P); 
  n = 4; 
  _new(L);
  _new(P);
  _new(U);
  A = _copy(A4);
  _LU(A, L, U, P);
  mat_write(A,n,argv[1],1);
  mat_write(L,n,argv[1],1);
  mat_write(U,n,argv[1],1);
  mat_write(P,n,argv[1],1);
  _del(A);
  _del(L);
  _del(U);
  _del(P);

#ifdef INST
  printFaultSitesData();
  printFaultInjectionData();
#endif	
  return 0;
}
