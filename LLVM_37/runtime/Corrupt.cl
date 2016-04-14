// stat[0] - mode, 1:count fault sites, 0: inject fault(s)
// stat[1] - thread idx 
// stat[2] - instruction idx
// stat[3] - random bit position to be corrupted for 8-bit wide data 
// stat[4] - random bit position to be corrupted for 16-bit wide data 
// stat[5] - random bit position to be corrupted for 32-bit wide data 
// stat[6] - random bit position to be corrupted for 64-bit wide data 
// stat[7] - fault injection status

//__attribute__((always_inline))
void memcpy_1byte(void *dest, void *src){
   char temp[1];       
   char *csrc = (char *)src;
   char *cdest = (char *)dest; 

   for (int i=0; i<1; i++)
       temp[i] = csrc[i]; 

   for (int i=0; i<1; i++)
       cdest[i] = temp[i];
   return;
}

//__attribute__((always_inline))
void memcpy_2byte(void *dest, void *src){
   char temp[2];       
   char *csrc = (char *)src;
   char *cdest = (char *)dest;
 
   for (int i=0; i<2; i++)
       temp[i] = csrc[i];
 
   for (int i=0; i<2; i++)
       cdest[i] = temp[i];
   return;
}

//__attribute__((always_inline))
void memcpy_4byte(void *dest, void *src){
   char temp[4];       
   char *csrc = (char *)src;
   char *cdest = (char *)dest;
 
   for (int i=0; i<4; i++)
       temp[i] = csrc[i];
 
   for (int i=0; i<4; i++)
       cdest[i] = temp[i];
   return;
}

//__attribute__((always_inline))
void memcpy_8byte(void *dest, void *src){
   char temp[8];       
   char *csrc = (char *)src;
   char *cdest = (char *)dest;
 
   for (int i=0; i<8; i++)
       temp[i] = csrc[i];
 
   for (int i=0; i<8; i++)
       cdest[i] = temp[i];
   return;
}

//__attribute__((always_inline))
int testSingleBitFlip(void* dest, void* src, unsigned bytesz){
	long long d=0,s=0;
   if(bytesz==1){
       memcpy_1byte((void*)&d,dest);
       memcpy_1byte((void*)&s,src);
   } else if(bytesz==2){
       memcpy_2byte((void*)&d,dest);
       memcpy_2byte((void*)&s,src);
   } else if(bytesz==4){
       memcpy_4byte((void*)&d,dest);
       memcpy_4byte((void*)&s,src);
   } else if(bytesz==8){
       memcpy_8byte((void*)&d,dest);
       memcpy_8byte((void*)&s,src);
   }
	int flippedPos=-1;

	long long result = (d^s);
	unsigned count =0;
	for(unsigned i=0;i<(8*bytesz);i++){
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

//__attribute__((always_inline))
void flipBit(void* data,long long bitPos, unsigned bytesz){
	long long dest = 0;
	// Copy source data to a 64-bit integer
   if(bytesz==1){
       memcpy_1byte((void*)&dest,data);
   } else if(bytesz==2){
       memcpy_2byte((void*)&dest,data);       
   } else if(bytesz==4){
       memcpy_4byte((void*)&dest,data);       
   } else if(bytesz==8){
       memcpy_8byte((void*)&dest,data);              
   }

    if ((dest>>bitPos)&0x1){
        dest = dest & (~((long long)0x1 << (bitPos)));
    } else{
        dest = dest |  ((long long) 0x1 << (bitPos));
    }

    // Copy back the data with a random bit flipped into the source
   if(bytesz==1){
       memcpy_1byte(data,(void*)&dest);
   } else if(bytesz==2){
       memcpy_2byte(data,(void*)&dest);       
   } else if(bytesz==4){
       memcpy_4byte(data,(void*)&dest);       
   } else if(bytesz==8){
       memcpy_8byte(data,(void*)&dest);              
   }    
    return; // A single-bit error successfully injected!!
}


//__attribute__((always_inline))
int injectSoftErrorVectorIdx(int idx, const char* instrName, int idx_limit,__global int* stat,__global int* fscount,__global int* fscounter,__global int* randi32){
    if(stat[7]>0) return idx;    
    int i = get_global_id(0);
    int cidx=idx;
    if(stat[0]==1){
        fscount[i] = fscount[i] + 1;
    } else if(stat[1]==i) {
        fscounter[i] = fscounter[i] + 1;
        if(stat[2]==fscounter[i]){ // found the target instruction
            int bitPos = abs(randi32[0]%idx_limit);
            flipBit((void*)&cidx,bitPos,4); // always flip bit 0 for IntTy1
            atom_add(&stat[7],1);
            return cidx;
        }
    }   
    return cidx;
}


//__attribute__((always_inline))
int _Z21injectSoftErrorIntTy1iPKci(int data, const char* instrName, int __mask,__global int* stat,__global int* fscount,__global int* fscounter,__global int* randi32) {
    if(stat[7]>0) return data;    
    int i = get_global_id(0);
    int cdata=data;       
    if(stat[0]==1){
        fscount[i] = fscount[i] + 1;
    } else if(stat[1]==i) {
        fscounter[i] = fscounter[i] + 1;
        if(stat[2]==fscounter[i]){ // found the target instruction
          // flip bit
          flipBit((void*)&cdata,0,4); // always flip bit 0 for IntTy1
          atom_add(&stat[7],1);    
          return cdata;
        }
    }   
    return cdata;
}

// key function called from instrumented code for fault injection w/ datatype IntTy8(char)
//__attribute__((always_inline))
int _Z21injectSoftErrorIntTy8cPKcc(char data, const char* instrName, char __mask,__global int* stat,__global int* fscount,__global int* fscounter,__global int* randi32){
    if(stat[7]>0) return data;    
    int i = get_global_id(0);
    char cdata=data;   
    if(stat[0]==1){
        fscount[i] = fscount[i] + 1;
    } else if(stat[1]==i) {
        fscounter[i] = fscounter[i] + 1;
        if(stat[2]==fscounter[i]){ // found the target instruction
          // flip bit
          flipBit((void*)&cdata,stat[3],1);
          atom_add(&stat[7],1);    
          return cdata;
        }
    }   
    return cdata;
}

// key function called from instrumented code for fault injection w/ datatype IntTy16
//__attribute__((always_inline))
short _Z22injectSoftErrorIntTy16sPKcs(short data, const char* instrName, short __mask,__global int* stat,__global int* fscount,__global int* fscounter,__global int* randi32){
    if(stat[7]>0) return data;    
    int i = get_global_id(0);
    short cdata=data;   
    if(stat[0]==1){
        fscount[i] = fscount[i] + 1;
    } else if(stat[1]==i) {
        fscounter[i] = fscounter[i] + 1;
        if(stat[2]==fscounter[i]){ // found the target instruction
          // flip bit
          flipBit((void*)&cdata,stat[4],2);
          atom_add(&stat[7],1);    
          return cdata;
        }
    }   
    return cdata;
}

// key function called from instrumented code for fault injection w/ datatype IntTy32
//__attribute__((always_inline))
int _Z22injectSoftErrorIntTy32iPKci(int data, const char* instrName, int __mask,__global int* stat,__global int* fscount,__global int* fscounter,__global int* randi32){
    if(stat[7]>0) return data;    
    int i = get_global_id(0);
    int cdata=data;   
    if(stat[0]==1){
        fscount[i] = fscount[i] + 1;
    } else if(stat[1]==i) {
        fscounter[i] = fscounter[i] + 1;
        if(stat[2]==fscounter[i]){ // found the target instruction
          // flip bit
          flipBit((void*)&cdata,stat[5],4);
          atom_add(&stat[7],1);    
          return cdata;
        }
    }   
    return cdata;
}

// key function called from instrumented code for fault injection w/ datatype IntTy64
//__attribute__((always_inline))
long long _Z22injectSoftErrorIntTy64xPKcx(long long data,const char* instrName, long long __mask,__global int* stat,__global int* fscount,__global int* fscounter,__global int* randi32){
    if(stat[7]>0) return data;    
    int i = get_global_id(0);
    long long cdata=data;   
    if(stat[0]==1){
        fscount[i] = fscount[i] + 1;
    } else if(stat[1]==i) {
        fscounter[i] = fscounter[i] + 1;
        if(stat[2]==fscounter[i]){ // found the target instruction
          // flip bit
          flipBit((void*)&cdata,stat[6],8);
          atom_add(&stat[7],1);    
          return cdata;
        }
    }   
    return cdata;
}

// key function called from instrumented code for fault injection w/ datatype FloatTy
//__attribute__((always_inline))
float _Z22injectSoftErrorFloatTyfPKcf(float data, const char* instrName, float __mask,__global int* stat,__global int* fscount,__global int* fscounter,__global int* randi32){         
    if(stat[7]>0) return data;    
    int i = get_global_id(0);
    float cdata=data;   
    if(stat[0]==1){
        fscount[i] = fscount[i] + 1;
    } else if(stat[1]==i) {
        fscounter[i] = fscounter[i] + 1;
        if(stat[2]==fscounter[i]){ // found the target instruction
          // flip bit
          flipBit((void*)&cdata,stat[5],4);
          atom_add(&stat[7],1);    
          return cdata;
        }
    }   
    return cdata;
}

// key function called from instrumented code for fault injection w/ datatype DoubleTy
//__attribute__((always_inline))
double _Z23injectSoftErrorDoubleTydPKcd(double data,const char* instrName, double __mask,__global int* stat,__global int* fscount,__global int* fscounter,__global int* randi32){
    if(stat[7]>0) return data;    
    int i = get_global_id(0);
    double cdata=data;   
    if(stat[0]==1){
        fscount[i] = fscount[i] + 1;
    } else if(stat[1]==i) {
        fscounter[i] = fscounter[i] + 1;
        if(stat[2]==fscounter[i]){ // found the target instruction
          // flip bit
          flipBit((void*)&cdata,stat[6],8);
          atom_add(&stat[7],1);    
          return cdata;
        }
    }   
    return cdata;
}

//__attribute__((always_inline))
int printFaultInjectionData(void){
    return 0;
}

//__attribute__((always_inline))
int printFaultSitesData(void){
    return 0;
}