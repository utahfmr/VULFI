#include "Corrupt.h"

//~ #ifndef DEBUG
//~ #define DEBUG
//~ #endif

#ifdef MPI_ENABLED
#include "mpi.h"
int targetRank=-1;
int currentRank=-2;
#endif

//dynamic fault site count
long long dynFSCount=0;

//detection count
long long detectCounter=0;

//dynamic fault site counter
long long dynFSCounter=0;

// interval size for computing fip
long long intvsz=0;

// interval count
long long intvCount=0;

// indices of the fault sites to be targeted for fault injection
// num elements in dynFSIdx[] = intvCount
long long* dynFSIdx=NULL;

// fault site counters
long long fsIntTy1=0;
long long fsIntTy8=0;
long long fsIntTy16=0;
long long fsIntTy32=0;
long long fsIntTy64=0;
long long fsFloatTy=0;
long long fsDoubleTy=0;

// fault injection counters
long long fiIntTy1=0;
long long fiIntTy8=0;
long long fiIntTy16=0;
long long fiIntTy32=0;
long long fiIntTy64=0;
long long fiFloatTy=0;
long long fiDoubleTy=0;

// To be read from fi.input file fi algo
enum FIAlgo fialg = FI_ERROR;

// byte direction
enum FIByteRange fibr = FIBR_ERROR;

// byte range for fault injection
enum FIDirection fid = FID_ERROR;

// Numerator of the fi probability
long long ef=-1;

// Denominator of the fi probability
long long tf=-1;

// read data from fi.input only once at runtime
int fidatardflag=0;

// Bit range lower position
int bl=-1;

// Bit range higher position
int bu=-1;

// print fault site flag
int pfs=-1;

// file containing user provided fi params
const char* fiParams = "fi.in";

// file to write fi result
const char* fiStat = "fi.csv";
const char* fiCount = "fi.out";

// file to write fs result
const char* fsStat = "fs.out";

// execution key to identify an execution;
//time_t execKey = 0;
long execKey=0;

inline void updateTimeStamp(void){
  struct timeval tv;
  gettimeofday(&tv,NULL);
  execKey = (1000000 * tv.tv_sec) + tv.tv_usec;
  return;
}

inline void writeInfo(const char* filename, const char* info){
  FILE *fp;
  fp = fopen(filename,"a");
  fprintf(fp,"%s\n",info);
  fclose(fp);
}

// @brief - checks if the mask is zero
// if mask is zero, and in fault injection mode, fault injection is not done
// if mask is zero, and in fault site counting mode, the site is not counted
inline int checkMask(void* a, int numbytes){
  // declare maximum size void* (64-bit)
  long long b = 0x0000000000000000;
  void *bptr = (void *) &b;
  return memcmp(a,bptr,numbytes)!=0;
}

inline long long gen64bitRand(void){
  long long longrand=0x0;
  int i;
  for(i=0;i<64;i++){
    if(rand()%2){
      longrand = longrand | 0x1;
    }
    longrand = longrand << 1;
  }
#ifdef DEBUG
  printf("\nGenerated random number: %lld",longrand);
#endif
  return longrand;
}

inline void updateFSIdx(void){
  int i,j;
  long long idx=0,base=0,randidx=0;
  if(intvCount>0 && ef>0){
    dynFSIdx = (long long*) calloc(intvCount*ef,sizeof(long long));
  }

  if(dynFSIdx==NULL){
    printf("\nError:memory allocation failed for dynFSIdx!!\n");
    exit(-1);
  }

  for(i=0;i<intvCount;i++){
    for(j=0;j<ef;j++){
      // generate a 64 bit random integer
      randidx = gen64bitRand();
      // choose dynamic instr for fault injection
      if(intvsz>0){
	base=intvsz*i;
	idx = randidx%intvsz;
	if(idx<0){idx=0-idx;}// this is a hack as abs only supports 32-bit int
	idx++;
	idx=idx+base; // index of the dynamic instruction chosen for fault injection
#ifdef DEBUG
	printf("\nbase=%lld\nidx=%lld",base,idx);
	printf("\n%lld mod %lld = %lld",randidx,intvsz,randidx%intvsz);
#endif
	dynFSIdx[(i*ef)+j]=idx;
      }
    }
  }
#ifdef DEBUG
  printf("\nBelow are the indices of the instr chosen for fault injection:");
  for(i=0;i<intvCount*ef;i++){
    printf("\ndynFSIdx[%d]=%lld",i,dynFSIdx[i]);
  }
#endif	
  return;
}

inline int getIntervalIdx(long long fsIdx){
  if(intvsz==0){
    return -1;
  }
  return floor(fsIdx/intvsz);
}

inline int matchDynFSIdx(void){
  int idx,base,j;
  dynFSCounter++;
  idx = getIntervalIdx(dynFSCounter);

  // check for invalid interval idx
  if(idx<0 || idx >= intvCount){
    return 0;
  }

  base = (idx*ef);
  for(j=0;j<ef;j++){
    if(dynFSIdx[base+j]==dynFSCounter){
      return 1;
    }
  }
  return 0;
}

// check if a file exists
inline int file_exists(const char *fname){
  FILE *file = fopen(fname, "r");
  if (file != NULL ){
    fclose(file);
    return 1;
  }
  return 0;
}

// write fault injection data to a fi.csv file
inline void writeFIData(const char* name, void* d, void* dc, void* bitPos, enum DataType dt, const char* mode){
  FILE *ft = getFD(fiStat,mode);
  char instrName[1024];
  unsigned len = strlen(name);

  //write execution key
  if(dt == StringTy){
    fprintf(ft,"Time;");
  } else {
    fprintf(ft,"%ld;",execKey);
  }

  // write instruction name
  strcpy(instrName,"\"");
  strncpy(instrName+1,name,len);
  strcpy(instrName+len,"\"\0");
  fprintf(ft,"%s;",instrName);

  // write original and corrupted value
  if(dt == IntTy1  ||
     dt == IntTy16 ||
     dt == IntTy32){
    fprintf(ft,"%d;",*((int*)d));
    fprintf(ft,"%d;",*((int*)dc));

  } else if( dt == IntTy8){
    fprintf(ft,"%c;",*((char*)d));
    fprintf(ft,"%c;",*((char*)dc));

  } else if(dt == IntTy64){
    fprintf(ft,"%lld;",*((long long*)d));
    fprintf(ft,"%lld;",*((long long*)dc));

  } else if(dt == FloatTy){
    fprintf(ft,"%f;",*((float*)d));
    fprintf(ft,"%f;",*((float*)dc));

  } else if(dt == DoubleTy){
    fprintf(ft,"%lf;",*((double*)d));
    fprintf(ft,"%lf;",*((double*)dc));

  } else if(dt == StringTy){
    fprintf(ft,"%s;",(char*)d);
    fprintf(ft,"%s;",(char*)dc);
  }

  // write bit position
  if(dt == StringTy){
    fprintf(ft,"%s\n",(char*)bitPos);
  } else {
    fprintf(ft,"%d\n",*((int*)bitPos));
  }
  if(ft){fclose(ft);}
}

// wrapper to get a new file handle
inline FILE* getFD(const char* fname, const char* mode){
  FILE *fp;
  fp = fopen(fname,mode);
  if (fp == NULL){
    printf("\nError:Coudn't find the file: %s\n",fname);
    exit(-1);
  }
  return fp;
}

// splits a string based on a delimiter and provides an element by index
inline char* getStrFieldByIndex(char* currentStr, const char* delim, int idx){
  char temp[1024];
  char* result;
  int i=-1;
  strcpy(temp,currentStr);
  result = strtok(temp,delim);
  while(result != NULL){
    i++;
    if(idx == i){
      return result;
    }
    result = strtok(NULL,delim);
  }
  return NULL;
}

// verify if fault injection params are valid
inline void checkParams(void){
  assert(fialg!=FI_ERROR && "Invalid fault injection algorithm!!");
  assert(fibr!=FIBR_ERROR && "Invalid byte range!!");
  assert(fid!=FID_ERROR && "Invalid byte direction!!");
  assert(((bu>=bl && bu<=63 && bl >=0)||fialg!=FI_ABR) && "Invalid bit range!!");
  assert(ef<=tf&&ef>0&&tf>0 && "Invalid fault injection probability!!");
  assert((pfs==0||pfs==1) && "Invalid value for pfs flag!!");
}

// print fault injection params
inline void printParams(void){
  #ifdef DEBUG
  printf("\nFault injection algorithm: %d",fialg);
  printf("\nByte range: %d",fibr);
  printf("\nByte direction: %d",fid);
  printf("\nProbability: %lld/%lld",ef,tf);
  printf("\nBit range (only applicable for fi algo ABR): [%d-%d]",bl,bu);
  printf("\nPrint Fault Site Status Flag: %d\n",pfs);
  #endif
}

// key function which reads fi params from fi.in and init global vars
inline void processParams(const char* name, const char* value){
  if(!strcmp(name,"fia")){
    if(!strcmp(value,"CBR")){
      fialg = FI_CBR;
    } else if(!strcmp(value,"ABR")){
      fialg = FI_ABR;
    } else if(!strcmp(value,"MSK")){
      fialg = FI_MSK;
    } else {
      fialg = FI_ERROR;
    }
  } else if(!strcmp(name,"fib")){
    if(!strcmp(value,"1")){
      fibr = FIBR_1;
    } else if(!strcmp(value,"2")){
      fibr = FIBR_2;
    } else if(!strcmp(value,"3")){
      fibr = FIBR_3;
    } else if(!strcmp(value,"4")){
      fibr = FIBR_4;
    } else if(!strcmp(value,"5")){
      fibr = FIBR_5;
    } else if(!strcmp(value,"6")){
      fibr = FIBR_6;
    } else if(!strcmp(value,"7")){
      fibr = FIBR_7;
    } else if(!strcmp(value,"8")){
      fibr = FIBR_8;
    } else {
      fibr = FIBR_ERROR;
    }
  } else if(!strcmp(name,"fid")){
    if(!strcmp(value,"LSB")){
      fid = FID_LSB;
    } else if(!strcmp(value,"MSB")){
      fid = FID_MSB;
    } else {
      fid = FID_ERROR;
    }
  } else if(!strcmp(name,"fiu")){
    bu = atoi(value);
  } else if(!strcmp(name,"fil")){
    bl = atoi(value);
  } else if(!strcmp(name,"pn")){
    ef = atoll(value);
  } else if(!strcmp(name,"pd")){
    tf = atoll(value);
  } else if(!strcmp(name,"pfs")){
    pfs = atoi(value);
  } else if(!strcmp(name,"fc")){
    dynFSCount = atoll(value);
  } 
#ifdef MPI_ENABLED
  else if(!strcmp(name,"procid")){
    targetRank = atoi(value);
  }
#endif
  return;
}

// update fi params with data from fi.input file
inline void updateFIParams(){
#ifdef MPI_ENABLED
  MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);
#endif	
  char * line = NULL;
  char *name,*value;
  size_t len = 0;
  ssize_t read;
  FILE* fp;

  // initialize rand seed once
  srand(time(NULL));

  // init execution key
  //execKey = time(NULL);
  updateTimeStamp();
  fp = fopen(fiParams,"r");
  if (fp == NULL){
    printf("\nError:Coudn't find fi param file \"fi.in\"\n");
    exit(-1);
  }

  // read fault injection params
  while ((read = getline(&line, &len, fp)) != -1) {
    name = getStrFieldByIndex(line,":\n",0);
    value = getStrFieldByIndex(line,":\n",1);
    processParams(name,value);
  }
  // select dyn instr only in fi mode
  if(pfs==0){
    // update interval size
    if(tf>0){
      intvsz = tf;
    }

    // update interval count
    if(dynFSCount>0 && intvsz>0){
      if(dynFSCount%intvsz==0){
	intvCount = dynFSCount/intvsz;
      } else {
	intvCount = floor(dynFSCount/intvsz)+1;
      }
    }

    // update target dynamic FS indices only once at runtime
    if(!fidatardflag){
      updateFSIdx();
    }
    if(dynFSIdx==NULL){
      printf("\nError:memory allocation failed for dynFSIdx!!\n");
      exit(-1);
    }
  }

  // write FI headers
  const char* instrfield = "Instr_Name";
  const char* originalVal = "Original_Value";
  const char* corruptedVal = "Corrupted_Value";
  const char* bitPosVal = "Bit_Position";
  if(!file_exists(fiStat) && pfs==0){
    writeFIData(instrfield,(void*)originalVal,(void*)corruptedVal,(void*)bitPosVal,StringTy,"w");
  }
#ifndef MPI_ENABLED
  printParams();
#endif
  checkParams();
  fidatardflag=1;
  fclose(fp);
  return;
}

// used for testing; prints bit-pattern for validation
inline void printBitPattern(void* data, unsigned sz){
  long long d=0;
  memcpy((void*)&d,data,sz);
  char bitPattern[1024];
  printf("\nObserved bit pattern is: ");
  for(unsigned i=0;i<(8*sz);i++){
    if((d>>i)&0x1){
      bitPattern[(8*sz)-1-i] = '1';
    } else {
      bitPattern[(8*sz)-1-i] = '0';
    }
  }
  bitPattern[(8*sz)]='\0';
  printf("%s\n",bitPattern);
}

// checks if exactly one bit is flipped in dest w.r.t src
inline int testSingleBitFlip(void* dest, void* src, unsigned sz){
  long long d=0,s=0;
  memcpy((void*)&d,dest,sz);
  memcpy((void*)&s,src,sz);
  int flippedPos=-1;

  long long result = (d^s);
  unsigned count =0;
  for(unsigned i=0;i<(8*sz);i++){
    if((result>>i)&0x1){
      count++;
      flippedPos=i+1;
    }
  }
  if(count == 1){ // single bit flip detected, return the position
    return  flippedPos;
  }
  return 0; //no or more than one bit flips detected
}

// returns a random bit position for fi algo - CBR
inline int getBitPosCBR(unsigned bytesz){
  unsigned totalBits = bytesz*8;
  int startBitPos=-1,endBitPos=-1;
  int corruptBitPos=-1;

  // if target direction is from MSB side
  if(fid == FID_MSB){
    // start bit always be the MSb
    startBitPos = (bytesz*8)-1;

    // end bit should always be #fibr bytes from MSB side
    if(fibr <= bytesz){
      endBitPos = (bytesz-fibr)*8;
    } else{  //if fibr exceeds bytesz then end bit should always be 0
      endBitPos = 0;
    }
  } else{ // if target direction is from LSB side
    // start bit should always be LSb
    startBitPos = 0;
    // end bit should always be #fibr bytes from LSB side
    if(fibr <= bytesz){
      endBitPos = (fibr*8)-1;
    } else{ //if fibr exceeds bytesz then end bit should always be MSb
      endBitPos = (bytesz*8)-1;
    }
  }

  if(fid){
    corruptBitPos = startBitPos - rand()%(abs(startBitPos-endBitPos)+1);
  } else {
    corruptBitPos = startBitPos + rand()%(abs(startBitPos-endBitPos)+1);
  }
  return corruptBitPos;
}

// returns a random bit position for fi algo - ABR
inline int getBitPosABR(enum DataType dt){
  int lsBitPos=-1,msBitPos=-1;
  int maxBitPos=-1,minBitPos=0;
  int corruptBitPos=-1;

  if(dt==IntTy8){
    maxBitPos=7;
  } else if(dt==IntTy16){
    maxBitPos=15;
  } else if(dt==IntTy32){
    maxBitPos=31;
  } else if(dt==IntTy64){
    maxBitPos=63;
  } else if(dt==FloatTy){
    maxBitPos=31;
  } else if(dt==DoubleTy){
    maxBitPos=63;
  }
	
  if(bu<=maxBitPos && bl>=minBitPos){
    lsBitPos=bl;
    msBitPos=bu;
  } else {
    lsBitPos=minBitPos;
    msBitPos=maxBitPos;
  }
  corruptBitPos = lsBitPos + rand()%(abs(msBitPos-lsBitPos)+1);
  return corruptBitPos;
}

// wrapper function, gets bit position for fi based on bitwidth and fi algo
inline int getBitPos(unsigned bytesz,
		     enum DataType dt,
		     enum FIAlgo fia){

  int corruptBitPos=-1;
  switch(fia){
  case FI_CBR :
    corruptBitPos = getBitPosCBR(bytesz);
    break;
  case FI_ABR :
    corruptBitPos = getBitPosABR(dt);			
    break;
  case FI_MSK :
    corruptBitPos = (bytesz*8)-1;
    break;
  case FI_ERROR :
    printf("\nError: Invalid value provided for FI Algo!\n");
    exit(-1);
    break;
  }
  return corruptBitPos;
}

// top level function which performs a bitflip
inline int flipBit(void* data,
		   unsigned bytesz,
		   enum DataType dt,
		   enum FIAlgo fia){
  long long dest = 0;
  long long bitPos = -1;

  //get bit position for error injection
  if(dt == IntTy1){
    bitPos=0;
  } else {
    bitPos = getBitPos(bytesz,dt,fia);
  }	

  // Copy source data to a 64-bit integer
  memcpy((void*)&dest,data,bytesz);

  if ((dest>>bitPos)&0x1){
    dest = dest & (~((long long)0x1 << (bitPos)));
  } else{
    dest = dest |  ((long long) 0x1 << (bitPos));
  }

  // Copy back the data with a random bit flipped into the source
  memcpy(data,(void*)&dest,bytesz);

  return bitPos; // A single-bit error successfully injected!!
}


// prints fault sites to stdout and fs.out
int printFaultSitesData(void){
#ifdef MPI_ENABLED
  if(targetRank!=currentRank) return -1;
#endif
  FILE *fp;
  long long faultSites;
  if(pfs==0 || pfs==-1){
    return -1;
  }
  faultSites = fsIntTy1+fsIntTy16+fsIntTy32+fsIntTy64+fsFloatTy+fsDoubleTy;
  fp = getFD(fsStat,"w");
  fprintf(fp,"fault site count:%lld",faultSites);
#ifdef MPI_ENABLED
  fprintf(fp,"\ntargetrank:%d",targetRank);
#endif
  if(fp){fclose(fp);}
  if(dynFSIdx){
    free(dynFSIdx);
    dynFSIdx=NULL;
  }
#ifdef DEBUG
  printf("\n----------------Fault Site Statistics------------------");
  printf("\nTotal # of fault sites enumerated : %lld",faultSites);
  printf("\nFurther sub-categorization of fault sites below:");
  printf("\nTotal # of Data fault sites IntTy1 : %lld",fsIntTy1);
  printf("\nTotal # of Data fault sites IntTy16 : %lld",fsIntTy16);
  printf("\nTotal # of Data fault sites IntTy32 : %lld",fsIntTy32);
  printf("\nTotal # of Data fault sites IntTy64 : %lld",fsIntTy64);
  printf("\nTotal # of Data fault sites FloatTy : %lld",fsFloatTy);
  printf("\nTotal # of Data fault sites DoubleTy : %lld",fsDoubleTy);
#ifdef MPI_ENABLED
  printf("\nThe chosen MPI process is : %d",targetRank);
#endif
  printf("\n------------------------------------------------------------\n");
#endif
  return 0;
}

// print fault injection count to stdout and fi.out
int printFaultInjectionData(void){
#ifdef MPI_ENABLED
  if(targetRank!=currentRank) {
#ifdef DEBUG
    printf("\nINFO: Target rank is %d but my rank is %d, therefore exiting!",targetRank,currentRank);
#endif
    return -1;
  }
  if(currentRank!=0){
    printf("\nINFO: Process # %d is waiting to receive error detection status from process# 0",targetRank);
    MPI_Status status;
    MPI_Recv(&detectCounter,1,MPI_LONG_LONG,0,targetRank,MPI_COMM_WORLD,&status);
    printf("\nINFO: Process# %d successfully received error detection status from process# 0",targetRank);
  }
#endif

  FILE *fp;
  long long totalFiCount;
  if(pfs==1 || pfs==-1){
    return -1;
  }
  totalFiCount = fiIntTy1+fiIntTy16+fiIntTy32+fiIntTy64+fiFloatTy+fiDoubleTy;
  fp = getFD(fiCount,"w");
  fprintf(fp,"fault injection count:%lld",totalFiCount);
  fprintf(fp,"\ntimestamp:%lu",execKey);
  fprintf(fp,"\nerror detection count:%lld",detectCounter);
  if(fp){fclose(fp);}
  if(dynFSIdx){
    free(dynFSIdx);
    dynFSIdx=NULL;
  }

#ifdef DEBUG
  printf("\n----------------Fault Injection Statistics------------------");
  printf("\nTotal # of fault injection done : %lld",totalFiCount);
  printf("\nFurther sub-categorization of fault sites below:");
  printf("\nTotal # of Data faults injected of type IntTy1 : %lld",fiIntTy1);
  printf("\nTotal # of Data faults injected of type IntTy16 : %lld",fiIntTy16);
  printf("\nTotal # of Data faults injected of type IntTy32 : %lld",fiIntTy32);
  printf("\nTotal # of Data faults injected of type IntTy64 : %lld",fiIntTy64);
  printf("\nTotal # of Data faults injected of type FloatTy : %lld",fiFloatTy);
  printf("\nTotal # of Data faults injected of type DoubleTy : %lld",fiDoubleTy);
  printf("\nTotal # of errors detected : %lld",detectCounter);
#ifdef MPI_ENABLED
  printf("\nThe chosen MPI process is : %d",targetRank);
#endif
  printf("\n------------------------------------------------------------\n");
#endif
  return 0;
}

// key function called from instrumented code for fault injection w/ datatype IntTy1
int injectSoftErrorIntTy1(int data, const char* instrName, int __mask){
  // check if fi params are read
  if(!fidatardflag){
    updateFIParams();
  }	
#ifdef MPI_ENABLED
  if(targetRank!=currentRank) return data;
#endif	
  int datacp = data;
  int bitPos=-1;
  if(!checkMask(&__mask,4)){
    return data;
  }
  // update fs count
  fsIntTy1++;
  // if pfs enabled then nothing more to do!
  if(pfs==1){
#ifdef PROFILE_DIC
    writeInfo("instrlist.csv",instrName);
#endif
    return data;
  }

  // if current dynamic instruction is not the
  // target instruction then skip fault injection
  if(!matchDynFSIdx()){
    return data;
  }

  enum DataType dt = IntTy1;
  // flip bit
  bitPos = flipBit((void*)&data,4,dt,fialg);
  // update fi count if bit is flipped
  if(bitPos >= 0){
    fiIntTy1++;
    writeFIData(instrName,(void*)&datacp,(void*)&data,&bitPos,IntTy1,"a");
  }
  return data;
}

// key function called from instrumented code for fault injection w/ datatype IntTy8(char)
int injectSoftErrorIntTy8(char data, const char* instrName, char __mask){
  if(!fidatardflag){
    updateFIParams();
  }	
#ifdef MPI_ENABLED
  if(targetRank!=currentRank) return data;
#endif	
  char datacp = data;
  int bitPos=-1;
  if(!checkMask(&__mask,1)){
    return data;
  }
  fsIntTy8++;
  if(pfs==1){
#ifdef PROFILE_DIC
    writeInfo("instrlist.csv",instrName);
#endif
    return data;
  }
  // if current dynamic instruction is not the
  // target instruction then skip fault injection
  if(!matchDynFSIdx()){
    return data;
  }

  enum DataType dt = IntTy8;
  bitPos = flipBit((void*)&data,1,dt,fialg);
  if(bitPos >= 0){
    fiIntTy8++;
    writeFIData(instrName,(void*)&datacp,(void*)&data,&bitPos,IntTy8,"a");
  }
  return data;
}

// key function called from instrumented code for fault injection w/ datatype IntTy16
short injectSoftErrorIntTy16(short data, const char* instrName, short __mask){
  if(!fidatardflag){
    updateFIParams();
  }	
#ifdef MPI_ENABLED
  if(targetRank!=currentRank) return data;
#endif	
  short datacp = data;
  int bitPos=-1;
  if(!checkMask(&__mask,2)){
    return data;
  }
  fsIntTy16++;
  if(pfs==1){
#ifdef PROFILE_DIC
    writeInfo("instrlist.csv",instrName);
#endif
    return data;
  }
  // if current dynamic instruction is not the
  // target instruction then skip fault injection
  if(!matchDynFSIdx()){
    return data;
  }
		
  enum DataType dt = IntTy16;
  bitPos = flipBit((void*)&data,2,dt,fialg);
  if(bitPos >= 0){
    fiIntTy16++;
    writeFIData(instrName,(void*)&datacp,(void*)&data,&bitPos,IntTy16,"a");
  }
  return data;
}

// key function called from instrumented code for fault injection w/ datatype IntTy32
int injectSoftErrorIntTy32(int data, const char* instrName, int __mask){
  if(!fidatardflag){
    updateFIParams();
  }	
#ifdef MPI_ENABLED
  if(targetRank!=currentRank) return data;
#endif	
  int datacp = data;
  int bitPos=-1;
  if(!checkMask(&__mask,4)){
    return data;
  }
  fsIntTy32++;
  if(pfs==1){
#ifdef PROFILE_DIC
    writeInfo("instrlist.csv",instrName);
#endif
    return data;
  }
  // if current dynamic instruction is not the
  // target instruction then skip fault injection
  if(!matchDynFSIdx()){
    return data;
  }
		
  enum DataType dt = IntTy32;
  bitPos = flipBit((void*)&data,4,dt,fialg);
  if(bitPos >= 0){
    fiIntTy32++;
    writeFIData(instrName,(void*)&datacp,(void*)&data,&bitPos,IntTy32,"a");
  }
  return data;
}

// key function called from instrumented code for fault injection w/ datatype IntTy64
long long injectSoftErrorIntTy64(long long data,const char* instrName, long long __mask){
  if(!fidatardflag){
    updateFIParams();
  }	
#ifdef MPI_ENABLED
  if(targetRank!=currentRank) return data;
#endif
  long long datacp = data;
  int bitPos=-1;
  if(!checkMask(&__mask,8)){
    return data;
  }
  fsIntTy64++;
  if(pfs==1){
#ifdef PROFILE_DIC
    writeInfo("instrlist.csv",instrName);
#endif
    return data;
  }
  // if current dynamic instruction is not the
  // target instruction then skip fault injection
  if(!matchDynFSIdx()){
    return data;
  }
		
  enum DataType dt = IntTy64;
  bitPos = flipBit((void*)&data,8,dt,fialg);
  if(bitPos >= 0){
    fiIntTy64++;
    writeFIData(instrName,(void*)&datacp,(void*)&data,&bitPos,IntTy64,"a");
  }
  return data;
}

// key function called from instrumented code for fault injection w/ datatype FloatTy
float injectSoftErrorFloatTy(float data,const char* instrName, float __mask){
  if(!fidatardflag){
    updateFIParams();
  }
#ifdef MPI_ENABLED
  if(targetRank!=currentRank) return data;
#endif	
  float datacp = data;
  int bitPos=-1;
  if(!checkMask(&__mask,4)){
    return data;
  }
  fsFloatTy++;
  if(pfs==1){
#ifdef PROFILE_DIC
    writeInfo("instrlist.csv",instrName);
#endif
    return data;
  }
	
  // if current dynamic instruction is not the
  // target instruction then skip fault injection
  if(!matchDynFSIdx()){
    return data;
  }

  enum DataType dt = FloatTy;
  bitPos = flipBit((void*)&data,4,dt,fialg);
  if(bitPos >= 0){
    fiFloatTy++;
    writeFIData(instrName,(void*)&datacp,(void*)&data,&bitPos,FloatTy,"a");
  }
  return data;
}

// key function called from instrumented code for fault injection w/ datatype DoubleTy
double injectSoftErrorDoubleTy(double data,const char* instrName, double __mask){
  if(!fidatardflag){
    updateFIParams();
  }	
#ifdef MPI_ENABLED
  if(targetRank!=currentRank) return data;
#endif		
  double datacp = data;
  int bitPos=-1;
  if(!checkMask(&__mask,8)){
    return data;
  }
  fsDoubleTy++;
  if(pfs==1){
#ifdef PROFILE_DIC
    writeInfo("instrlist.csv",instrName);
#endif
    return data;
  }
  // if current dynamic instruction is not the
  // target instruction then skip fault injection
  if(!matchDynFSIdx()){
    return data;
  }	
	
  enum DataType dt = DoubleTy;
  bitPos = flipBit((void*)&data,8,dt,fialg);
  if(bitPos >= 0){
    fiDoubleTy++;
    writeFIData(instrName,(void*)&datacp,(void*)&data,&bitPos,DoubleTy,"a");
  }
  return data;
}
