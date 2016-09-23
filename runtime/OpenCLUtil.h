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

#pragma once

#include <CL/cl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <vector>

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

enum VectorFSType{
	FST_MSK, // Data sites (both integer and floating-point)
	FST_INSIDX, // Integer data sites
	FST_EXIDX, // Floating-point data sites
	FST_SHUFIDX, // Address sites
	FST_VCMP, // Vector control-flow sites
	FST_ERROR
};

 
void printStat(int *stat);
size_t getMaxWorkGrpSize(cl_kernel kernel,cl_device_id device);
int* getArrayOfWorkGrpSize(cl_kernel kernel,cl_device_id device, size_t global_thsz);
void initFaultSiteCountMode(int *stat,int);
void printArray(int *a, size_t n);
void printArrayAtIdx(int *a, size_t n, size_t idx);
void initFaultInjectionMode(int *stat, int* fscount, int* randi32, size_t len);
void resetArray(int *a, size_t n);
void writeFIData(const char* name, void* d, void* dc, void* bitPos, enum DataType dt, const char* mode);
inline FILE* getFD(const char* fname, const char* mode);
void setTargetThreadIdx(int *stat, int* fscount, size_t len);


template <class T>
int checkNwriteSDC(const char* fname, T *a, T *ac, size_t n, T th, int* stat){
	FILE *ft = getFD(fname,"a");
	if(stat[7] != 1){
		printf("\nNo fault or more than one faults injected!\n");
		fprintf(ft,"-1,0.0,0.0\n");
		if(ft) fclose(ft);
		return -1;
	}
	int i;
	T l2norm = 0.0;

	for (i=0;i<n;i++){
		l2norm += (a[i]-ac[i])*(a[i]-ac[i]);
	}
	l2norm = sqrt(l2norm);

	if(l2norm > th){
		printf("\nSDC found, L2Norm is: %f\n", l2norm);
		fprintf(ft,"1,%f,%f\n",l2norm,th);
		if(ft) fclose(ft);
		return 1;
	} else {
		printf("\nNo SDC found, L2Norm is: %f\n", l2norm);
		fprintf(ft,"0,%f,%f\n",l2norm,th);
		if(ft) fclose(ft);
		return 0;
	}
}

template <class T>
int checkNwriteSDC(const char* fname, T a, T ac, int* stat){
	FILE *ft = getFD(fname,"a");
	int i;
	if(stat[7] != 1){
		printf("\nNo fault or more than one faults injected!\n");
		fprintf(ft,"-1,0\n");
		if(ft) fclose(ft);
		return -1;
	} else if(a != ac){
		printf("\nSDC found\n");
		fprintf(ft,"1,0\n");
		if(ft) fclose(ft);
		return 1;
	} else {
		printf("\nNo SDC found\n");
		fprintf(ft,"0,0\n");
		if(ft) fclose(ft);
		return 0;
	}
}
