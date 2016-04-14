#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "OpenCLUtil.h"

//#define DEBUG

time_t execKey = 0;

/// DATA STRUCTURES
/// __global data structure "stat" passed as the 3rd last argument to a kernel
/// field definition below
/*
 * stat[0] - mode, 1:count dynamic instructions, 0: inject fault(s)
 * stat[1] - thread index to be target for a fault injection
 * stat[2] - instruction index to be target for a fault injection
 * stat[3] - random bit position to be corrupted for 8-bit wide data 
 * stat[4] - random bit position to be corrupted for 16-bit wide data 
 * stat[5] - random bit position to be corrupted for 32-bit wide data 
 * stat[6] - random bit position to be corrupted for 64-bit wide data 
 * stat[7] - fault injection status
 * 
 * */

/// __global data structure "fscount" passed as the 2nd last argument to a kernel
/// Used to store dynamic instruction count per thread


/// __global data structure "fscounter" passed as the 2nd last argument to a kernel
/// Used during the fault injection phase to maintain the count for dynamic instruction
/// per thread


/// USAGE
/// Phase 1 : Count Dynamic Instruction per thread
/*
 * Step 1: Create buffer for stat on the device
 * stat = clCreateBuffer(oclobjects.context, CL_MEM_READ_WRITE, sizeof(int) * 8, NULL, NULL);
 * 
 * Step 2: Get the maximum number of threads that can run on the device
 * size_t workgrpsz = getMaxWorkGrpSize(executable.kernel, oclobjects.device);
 * 
 * Step 3: Create a buffer "fscount" of size equal to max thread count on the device
 * fscount = clCreateBuffer(oclobjects.context, CL_MEM_READ_WRITE, sizeof(int) * workgrpsz, NULL, NULL);
 * 
 * Step 4: Create a buffer "fscounter" of size equal to max thread count on the device
 * fscounter = clCreateBuffer(oclobjects.context, CL_MEM_READ_WRITE, sizeof(int) * workgrpsz, NULL, NULL);    
 * 
 * Step 5: Create a buffer "fscount_data" of size equal to max thread count on the host
 * fscount_data = getArrayOfWorkGrpSize(executable.kernel, oclobjects.device);
 * 
 * Step 6: Create a buffer "fscounter_data" of size equal to max thread count on the host
 * fscounter_data = getArrayOfWorkGrpSize(executable.kernel, oclobjects.device);
 *  
 * Step 7: Init mode
 * initFaultSiteCountMode(stat_data);
 * 
 * Step 8: Write host data to the device
 * err = clEnqueueWriteBuffer(oclobjects.queue, stat, CL_TRUE, 0, sizeof(int) * FIELD_COUNT, stat_data, 0, NULL, NULL);
 * err |= clEnqueueWriteBuffer(oclobjects.queue, fscount, CL_TRUE, 0, sizeof(int) * workgrpsz, fscount_data, 0, NULL, NULL);
 * 
 * Step 9: Set the __global data sturctures as the additional kernel arguments
 * err = clSetKernelArg(executable.kernel, 3RD_LAST, sizeof(cl_mem), &stat);    
 * err |= clSetKernelArg(executable.kernel, 2ND_LAST, sizeof(cl_mem), &fscount);    
 * err |= clSetKernelArg(executable.kernel, LAST, sizeof(cl_mem), &fscounter);  
 * 
 * Step 10: Execute the kernel
 * 
 * Step 11: Read the dynamic instruction count
 * err = clEnqueueReadBuffer( oclobjects.queue, fscount, CL_TRUE, 0, sizeof(int) * workgrpsz, fscount_data, 0, NULL, NULL );  
 * 
 * 
 * */

/// Phase 2 : Inject fault
/*
 * Step 1: Init fault injection mode.
 * initFaultInjectionMode(stat_data,fscount_data,workgrpsz);
 * 
 * 
 * Step 2: Write "stat" and "fscounter" data to the device
 * err = clEnqueueWriteBuffer(oclobjects.queue, stat, CL_TRUE, 0, sizeof(int) * FIELD_COUNT, stat_data, 0, NULL, NULL);
 * err |= clEnqueueWriteBuffer(oclobjects.queue, fscounter, CL_TRUE, 0, sizeof(int) * workgrpsz, fscounter_data, 0, NULL, NULL);
 * err = clEnqueueReadBuffer( oclobjects.queue, output, CL_TRUE, 0, sizeof(float) * count, results, 0, NULL, NULL );  
 * err = clEnqueueReadBuffer( oclobjects.queue, stat, CL_TRUE, 0, sizeof(int) * FIELD_COUNT, stat_data, 0, NULL, NULL );  
 * 
 * Step 3: Execute the kernel
 * 
 * Step 4: Print the fault injection result on the host's stdout
 * printStat(stat_data);
 * 
 * */  
 
inline FILE* getFD(const char* fname, const char* mode){
	FILE *fp;
	fp = fopen(fname,mode);
	if (fp == NULL){
		printf("\nError:Coudn't find the file: %s\n",fname);
		exit(-1);
	}
	return fp;
}

void writeFIData(const char* name, void* d, void* dc, void* bitPos, enum DataType dt, const char* mode){
	FILE *ft = getFD("fi.csv",mode);
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


void printStat(int *stat){
	if(stat[0]==1){
		printf("\nMode=%d\n",stat[0]);
		printf("\nChosen thread idx=%d\n",stat[1]);
	} else {
		printf("\nMode=%d",stat[0]);
		printf("\nChosen thread idx=%d",stat[1]);
		printf("\nChosen instruction idx=%d",stat[2]);
		printf("\nBit position for 8-bit wode data=%d",stat[3]);
		printf("\nBit position for 16-bit wode data=%d",stat[4]);
		printf("\nBit position for 32-bit wode data=%d",stat[5]);
		printf("\nBit position for 64-bit wode data=%d\n",stat[6]);
		printf("\nFault Injection status=%d\n",stat[7]);
	}
}

size_t getMaxWorkGrpSize(cl_kernel kernel,cl_device_id device){
	size_t param_value;
	cl_int err = clGetKernelWorkGroupInfo(kernel,device,CL_KERNEL_WORK_GROUP_SIZE,sizeof(param_value),&param_value,NULL);
    if (err != CL_SUCCESS){
        printf("Error: Could not find out work group size! %d\n", err);
        exit(1);
    }
    return param_value;
}

int* getArrayOfWorkGrpSize(cl_kernel kernel,cl_device_id device, size_t global_thsz){
	int *temp=NULL;
	int i;
	size_t param_value=0;
	if(global_thsz==0){
		param_value = getMaxWorkGrpSize(kernel,device);
	} else {
		param_value = global_thsz;
	}
    temp = (int*) malloc(sizeof(int)*param_value);
    for(i=0;i<param_value;i++) temp[i]=0;
	return temp;
}

void initFaultSiteCountMode(int *stat, int thread_max){
	srand(time(NULL));
	stat[0] = 1;
	//stat[1] = rand()%thread_max;
	//printf("\nThe index of the thread to be targeted: %d\n",stat[1]);
	stat[7] = 0;
	return;
}

void printArray(int *a, size_t n){
	int i;
	printf("[ %d ", a[0]);
	for(i=1;i<n-1;i++){
		printf(" %d ", a[i]);
	}
	printf(" %d]", a[i]);
	return;
}

void printArrayAtIdx(int *a, size_t n, size_t idx){
	printf("\nFault site count of work item # %d is: %d\n", idx, a[idx]);
}


void resetArray(int *a, size_t n){
	int i;
	for(i=0;i<n;i++){
		a[i]=0;
	}
	return;
}





void setTargetThreadIdx(int *stat, int* fscount, size_t len){
	srand(time(NULL));
	std::vector<int> idxlist;
	for(int i=0;i<len;i++){
		if(fscount[i]>0) {
			idxlist.push_back(i);
#ifdef DEBUG
			printf("\nSuccess: index of thread found is: %d\n",i);
#endif
		}
	}
	if(idxlist.size()>0){
		int tidx = rand()%idxlist.size();
		stat[1] = idxlist[tidx];
	} else {
		printf("\nFatal Error: Fault site count for all threads can't be zero!\n");
		exit(-1);
	}
	return;
}

void initFaultInjectionMode(int *stat, int* fscount, int* randi32, size_t len){
	srand(time(NULL));
	stat[0] = 0; // fi mode
	setTargetThreadIdx(stat,fscount,len);
	printf("\nThe index of the thread to be targeted: %d\n",stat[1]);
	if(fscount[stat[1]]>0){
		stat[2] = rand()%fscount[stat[1]]+1; // instruction idx
		printf("\nDynamic instruction to be targeted: %d\n",stat[2]);
	}
	stat[3] = rand()%8;
	stat[4] = rand()%16;
    stat[5] = rand()%32;
	stat[6] = rand()%64;
	stat[7] = 0;
	randi32[0] = rand();
	return;
}
