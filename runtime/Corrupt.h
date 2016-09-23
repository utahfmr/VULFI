#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

// This is based on LLVM Types
enum DataType{
  IntTy1=0,
  IntTy8,
  IntTy16,
  IntTy32,
  IntTy64,
  FloatTy,
  DoubleTy,
  StringTy // custom type (non-llvm)
};

// fault injection algorithm
enum FIAlgo{
  FI_CBR=0, // cumulative byte range
  FI_ABR, // absolute bit range
  FI_MSK, // inject fault into a mask register
  FI_ERROR
};

// fault injection byte range
enum FIByteRange{
  FIBR_1=1,
  FIBR_2,
  FIBR_3,
  FIBR_4,
  FIBR_5,
  FIBR_6,
  FIBR_7,
  FIBR_8,
  FIBR_ERROR
};

// direction for counting bytes
enum FIDirection{
  FID_LSB=0,
  FID_MSB,
  FID_ERROR
};


void updateFIParams(void);
void printBitPattern(void* data, unsigned sz);
int testSingleBitFlip(void* dest, void* src, unsigned sz);
char* getStrFieldByIndex(char* currentStr, const char* delim, int idx);
void processParams(const char* name, const char* value);
void printParams(void);
void checkParams(void);
#ifdef __cplusplus
extern "C" int printFaultSitesData(void);
extern "C" int printFaultInjectionData(void);
#else
int printFaultSitesData(void);
int printFaultInjectionData(void);
#endif
int injectSoftErrorIntTy1(int data, const char* name, int __mask);
int injectSoftErrorIntTy8(char data, const char* name, char __mask);
short injectSoftErrorIntTy16(short data, const char* name, short __mask);
int injectSoftErrorIntTy32(int data, const char* name, int __mask);
long long injectSoftErrorIntTy64(long long data, const char* name, long long __mask);
float injectSoftErrorFloatTy(float data, const char* name, float __mask);
double injectSoftErrorDoubleTy(double data, const char* name, double __mask);
inline FILE* getFD(const char* fname, const char* mode);
int file_exists(const char *fname);
void writeFIData(const char* name, void* d, void* dc, void* bitPos, enum DataType dt, const char* mode);
int getBitPosCBR(unsigned bytesz);
int checkMask(void* a, int numbytes);
