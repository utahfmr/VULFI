# VULFI - An LLVM based Fault Injection Framework
## Introduction
VULFI is an open-source instruction-level fault injection framework developed using [LLVM compiler infrastructure](http://llvm.org/). VULFI performs fault injection at LLVM's intermediate representation (IR) level. It currently supports C, C++, [ISPC](https://ispc.github.io/), [OpenCL](https://www.khronos.org/opencl/), and MPI-C languages. Note that to target an OpenCL program for fault injection using VULFI, the program should be compiled using [Intel's OpenCL Compiler](https://software.intel.com/en-us/node/530783). Conceptually, VULFI can target any high level language which can be compiled to [LLVM's intermediate representation](http://llvm.org/docs/LangRef.html).

Vulfi supports all major LLVM versions starting from LLVM version 3.2. The top level vulfi directory has a master folder which contains the latest source code of VULFI supporting latest [LLVM version](http://llvm.org/svn/llvm-project/llvm/trunk). The top level directory has additional folders starting with the prefix "LLVM_" with each folder containing vulfi source code supporting the respective version of LLVM. For example, the folder LLVM_32 contains the vulfi source code supporting LLVM version 3.2. It is our intention to keep vulfi update supporting the latest LLVM [development branch] (http://llvm.org/svn/llvm-project/llvm/trunk) while also maintaining reasonable support for older versions of LLVM.

## People 
### Faculty & Collaborators
* [Ganesh Gopalakrishnan](http://www.cs.utah.edu/~ganesh/)
* [Sriram Krishnamoorthy](http://hpc.pnl.gov/people/sriram/)

### Developers
* [Vishal Sharma](https://sites.google.com/site/vishalutah/home)
* Ian Briggs
* Vinu Joseph
* Arnab Das

## Cite our work
When you use VULFI in your research work, please cite our work using below citation. More information about the paper could be found [here.](http://www.cs.utah.edu/~vcsharma/pubs/dpdns2016-shrg.pdf)

```
@inproceedings{dpdns16vulfi,
  author = {Vishal Chandra Sharma, Ganesh Gopalakrishnan, Sriram Krishnamoorthy},
  title = {Towards Reseiliency Evaluation of Vector Programs},
  booktitle = {21st IEEE Workshop on Dependable Parallel, Distributed and Network-Centric Systems (DPDNS)},
  year = {2016},
}
```
## Code documentation
The latest doxygen code documentation of VULFI can be found [here.](http://formalverification.cs.utah.edu/fmr/vulfi/html/)

## LLVM Resources
If you are new to LLVM, you may find below resources useful:
* [Getting started with LLVM ](http://llvm.org/docs/GettingStarted.html)
* [Writing an LLVM Pass](http://llvm.org/docs/WritingAnLLVMPass.html)
* [LLVM Programmer's Manual](http://llvm.org/docs/ProgrammersManual.html)
* [LLVM Language Reference](http://llvm.org/docs/LangRef.html)
* [Adrian Sampson's Blog on LLVM](http://adriansampson.net/blog/llvm.html)
* [Building LLVM with CMake](http://llvm.org/docs/CMake.html)
* [LLVM CMake Variables](http://llvm.org/docs/CMake.html#options-and-variables)
* [LLVM’s Analysis and Transform Passes](http://llvm.org/docs/Passes.html)

## Contact Us
If you have feedback or would like to report an issue, please [email us](
mailto:vulfi-dev@googlegroups.com) or join our [mailing list](https://groups.google.com/forum/#!forum/vulfi-dev).


## Getting Started with VULFI
### Software Dependencies
* LLVM (version >= 3.2)
* CMake (version > 3.0) 
* Python (version > 2.7)
* Doxygen (version > 1.8.9)
* Graphviz (default version)

### System Configuration
Below is the system configuration used for testing all versions of VULFI.
* CPU : Intel(R) Core(TM) i7-4770 CPU @ 3.40GHz
* OS : Ubuntu 15.10 (64-bit)
* Memory : 16GB

### License & Copyright Information
### Installation

##### Step 1: Download VULFI. 
```
git clone https://github.com/utahfmr/vulfi.github.io.git <vulfi dir>
```
where \<vulfi dir\> is the directory where you want vulfi source code to live.

##### Step 2: Choose the correct version of VULFI. 

If you would like to build VULFI using the latest LLVM version then go to "master" folder else refer to one of the folder starting with the prefix "LLVM_" represeting your LLVM version.

```
cd <vulfi dir>/master
```

##### Step 3: Create the build directory.

```
mkdir build
cd build
```

##### Step 4: Build & Install VULFI.

First, run cake to generate build scripts. If everything works fine then cmake should finish without errors.
```
cmake <vulfi dir>/master
```

Now, running make command should build VULFI library with the name LLVMVulfi.so.

```
make
```

After building VULFI, you could install vulfi by running below command. Please note that the default installation directory for VULFI is ${CMAKE_INSTALL_PREFIX}/lib. On linux systems, CMAKE_INSTALL_PREFIX points to /usr/local. 

```
sudo make install
```

To change the default installation directory of VULFI, you can use [DESTDIR](https://cmake.org/cmake/help/v3.0/variable/CMAKE_INSTALL_PREFIX.html). An example commandline given below.

```
sudo make install DESTDIR=<my dir>
```

##### Step 4: Run examples.

VULFI has some sample examples written in C and C++ which can be located at <vulfi dir>/master/examples. You can look at the "common.mk" files inside "src" and "run" subdirectories to understand how VULFI can be used for fault injection. You can compile and run all examples in one go by running below commands.

```
cd <vulfi dir>/master/examples
make VULFI_SRC_DIR=<vulfi dir>/master VULFI_INSTALL_DIR=/usr/local/lib
```

Note that the variables VULFI_SRC_DIR and VULFI_INSTALL_DIR must be assigned absolute paths representing where VULFI source code lives and where VULFI library (LLVMVulfi.so) is installed respectively.



### Steps for using VULFI for Fault Injection
Before reading the below documentation, please ensure that you have already executed the examples explained in the previous section, and also inspected the "common.mk" files to understand how the instrumentation and execution processes are carried out. You could pretty much use those "common.mk" files to bootstrap your fault injection process. 


##### Step 1: Adding function calls.

First, locate the code file which has the main() function. 

If your target program is a C program then add below code snippet at the begining of the code file.

```
#ifdef INST
extern int printFaultSitesData(void);
extern int printFaultInjectionData(void);
#endif
```

If your target program is a C++ program then add below code snippet at the begining of the code file.


```
#ifdef INST
extern "C" int printFaultSitesData(void);
extern "C" int printFaultInjectionData(void);
#endif
```

Now, in the main() function, add below code snippet before all the return calls.

```
#ifdef INST
  printFaultSitesData();
  printFaultInjectionData();
#endif  
```


##### Command line options supported by VULFI

```
-fn  : Name(s) of the function(s) to be targeted
       Example: -fn "func1 func2 func3"

-fsa : Fault Site Selection Algorithm
       Options: 
        data: target all fault sites which only affect data instructions 
        (and don't affect control or address)
        dint: target all "data" fault sites of integer type (bitwdith >=2)
        dflo: target all "data" fault sites of floating-point type (both 
        single and double precision)
        ctrl: target all fault sites which affect control instructions.
        addr: target all fault sites which affect address instructions.
       Example: -fsa "data"
       Default Value: data
       

-lang : Target language
        Options: C/C++/ISPC/OCL
        Description: This information is used by VULFI to implement language 
        specific features.
        Example: -lang "C++"
        Default Value: C++

-arch : Target architecture
        Options: x86/neon/nvvm/mips/spu
        Description: This information is used by VULFI to implement arch
        specific features. Currently, "x86" is the only supported one.
        Example: -arch "x86"
        Default Value: x86
        
-dbf  : Debug file name with csv extension
        Description: this CSV file provides the list of static LLVM-IR instructions 
        that will be targeted for fault injection.
```

##### Step 2: Instrumentation.
Suppose, we have a code file foo.cpp containing functions foo1(), foo2(), and main(), that we want to target for fault injection. 

* As a first step, we need to manually add the code snippets described in Step 1. 

* Next, compile Corrupt.C (inside the runtime folder of VULFI) to LLVM bitcode as shown below.

```
clang++ <vulfi dir>/master/runtime/Corrupt.C -emit-llvm -c -o Corrupt.bc
```

* Now, compile foo.cpp as shown below.

```
clang++ foo.cp -emit-llvm -c -o foo.bc 
```

* We should now link both the bitcode files as shown below.

```
llvm-link Corrupt.bc foo.bc -o foo_corrupt.bc
```

* Finally, instrument the bitcode foo_corrupt.bc using VULFI. For example, if we can to target all "data" fault sites in functions foo1() and foo2() in foo.cpp code file, we should run below command.
 ```
opt -load /usr/local/lib/LLVMVulfi.so -vulfi -fn "foo1 foo2" -fsa "data" -lang "C++" -arch "x86" \
-dbgf "fault_sites_data_foo.csv" < foo_corrupt.bc > foo_instrument.bc

```


##### Step 3: Execution.

```
python driver.py --help
usage: driver.py [-h] [-e] [-s] [-c] [--ov] [--pv] [--iter EXEC_COUNT]
                 [--fcp {bex,img,num}] [--th TH] [--exec1 EXEC1] [--cmd1 CMD1]
                 [--out1 OUT1] [--cho] [--cfs] [--exec2 EXEC2] [--cmd2 CMD2]
                 [--out2 OUT2] [--rslt RSLT] [--ficsv FICSV]
                 [--fia {cbr,abr,msk}] [--fib {1,2,3,4,5,6,7,8}]
                 [--fid {msb,lsb}] [--fbu FBU] [--fbl FBL] [--np NUM_PROB]
                 [--dp DEN_PROB] [--ff {eql,max,min,nlm}] [--fc FICOUNT]

Process commandline options..

optional arguments:
  -h, --help            show this help message and exit
  -e                    execution mode.
  -s                    standalone mode, always used in combination with -e option.
  -c                    comparison mode, always used in combination with -e option.
  --ov                  Calculate overhead
  --pv                  override user provided fault injection probability..
  --iter EXEC_COUNT     Number of iterations.
  --fcp {bex,img,num}   file comparison mode.
  --th TH               threshold value, only valid with --fcp=img or num.
  --exec1 EXEC1         location of the 1st executable to be executed with -s or -c mode.
  --cmd1 CMD1           command line args for the exe provided using --exec1 option.
  --out1 OUT1           name (including path) of the output generated by --exec1 option.
  --cho                 this flag indicates that use cached output for exec1 if available
  --cfs                 this flag indicates that use cached fault site count
  --exec2 EXEC2         location of the instrumented executable to be executed only with -c.
  --cmd2 CMD2           command line args for the exe provided using --exec2 option.
  --out2 OUT2           name (including path) of the output generated by --exec2 option.
  --rslt RSLT           name of the output file where result will be written
  --ficsv FICSV         name of the input csv file containing intermediate fi data.
  --fia {cbr,abr,msk}   fault injection algorithm, valid options: 
                         	cbr - cumulative byte ordering; target one or more contiguous 
                         	bytes for fault injection.
                        	abr - absolute bit range;target a bit-range for fault injection
  --fib {1,2,3,4,5,6,7,8}
                        number of contiguous bytes to be considered for fault injection
                        Note:this option must be provided when using -fia=cbr
                        
  --fid {msb,lsb}       direction from which no. of contiguous bytes to be considered for 
                        fault injection
                        Note:this option must be provided when using -fia=cbr
                        
  --fbu FBU             upper bound for the fault injection bit-range.
                        Note:this option must be provided when using -fia=abr
                        
  --fbl FBL             lower bound for the fault injection bit-range.
                        Note:this option must be provided when using -fia=abr
  --np NUM_PROB         numerator of fault injection probability expressed as a fraction.
  --dp DEN_PROB         denominator of fault injection probability expressed as a fraction.
  --ff {eql,max,min,nlm}
                        puts restriction on no. of faults that will be injected, valid options:
                        	eql - execution with fault injection count equal to the val 
                        	provided by --fc.
                        	max - execution with fault injection count at max equal to the val 
                        	provided by --fc.
                        	min - execution with fault injection count at least equal to the val 
                        	provided by --fc.
                        	nlm - execution with fault injection count having no limit.
  --fc FICOUNT          fault injection countm this value is used in combination with --ff.
```
