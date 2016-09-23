

## Getting Started with VULFI -- version 1.0
### Software Dependencies
* LLVM (version = 3.9)
* CMake (version >= 3.0) 
* Python (version >= 2.7)
* Doxygen (version >= 1.8.9)
* Graphviz (default version)

### System Configuration
Below is the system configuration used for testing all versions of VULFI.
* CPU : Intel(R) Core(TM) i7-4770 CPU @ 3.40GHz
* OS : Ubuntu 15.10 (64-bit)
* Memory : 16GB

### License & Copyright Information
Please refer to the **LICENSE** document located in the top level directory of VULFI.

### Installation
##### Step 1: Download VULFI. 

```
git clone https://github.com/utahfmr/VULFI.git <vulfi dir>
```
where \<vulfi dir\> is the directory where you want vulfi source code to live.

If you would like to build VULFI using older versions of LLVM version then go
to http://tinyurl.com/vulfi-llvm-32-to-38 and refer to one of the folder starting with the prefix "LLVM_" representing your LLVM version.

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

After building VULFI, you could install VULFI by running below command. Please note that the default installation directory for VULFI is ${CMAKE_INSTALL_PREFIX}/lib. On Linux systems, CMAKE_INSTALL_PREFIX points to /usr/local. 

```
sudo make install
```

To change the default installation directory of VULFI, you can use [DESTDIR](https://cmake.org/cmake/help/v3.0/variable/CMAKE_INSTALL_PREFIX.html). An example command-line given below.

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

If your target program is a C program then add below code snippet at the beginning of the code file.

```
#ifdef INST
extern int printFaultSitesData(void);
extern int printFaultInjectionData(void);
#endif
```

If your target program is a C++ program then add below code snippet at the beginning of the code file.


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
        Description: This information is used by VULFI to implement 
        language specific features.
        Example: -lang "C++"
        Default Value: C++

-arch : Target architecture
        Options: x86/neon/nvvm/mips/spu
        Description: This information is used by VULFI to implement arch
        specific features. Currently, "x86" is the only supported one.
        Example: -arch "x86"
        Default Value: x86
        
-dbf  : Debug file name with csv extension
        Description: this CSV file provides the list of static 
        LLVM-IR instructions that will be targeted for fault injection.
```

##### Step 2: Instrumentation.
Suppose, we have a code file foo.cpp containing functions foo1(), foo2(), and main(), that we want to target for fault injection. 

* As a first step, we need to manually add the code snippets described in Step 1 in foo.cpp and name the modified file foo_inst.cpp. Essentially, we now have two code files: foo.cpp (without code snippets) and foo_inst.cpp (with code snippets inserted).

* Next, compile Corrupt.C (inside the runtime folder of VULFI) to LLVM bitcode as shown below.

```
clang++ <vulfi dir>/master/runtime/Corrupt.C -emit-llvm -c -o Corrupt.bc
```

* Now, compile foo.cpp and foo_inst.cpp as shown below.

```
clang++ foo.cp -emit-llvm -c -o foo.bc
clang++ foo_inst.cp -DINST -emit-llvm -c -o foo_inst.bc 
```

* We should now link both the bitcode files as shown below.

```
llvm-link Corrupt.bc foo_inst.bc -o foo_corrupt.bc
```

* Now, instrument the bitcode file foo_corrupt.bc using VULFI. For example, if we want to target all the "data" fault sites in functions foo1() and foo2() of foo.cpp code file, we should run below command.

 ```
opt -load /usr/local/lib/LLVMVulfi.so -vulfi -fn "foo1 foo2" -fsa "data" -lang "C++" -arch "x86" \
-dbgf "fault_sites_data_foo.csv" < foo_corrupt.bc > foo_instrument.bc
```

* Finally, generate the binary files using below commands.

```
clang++ foo.bc -o foo
clang++ foo_instrument.bc -o foo_instrument
```

The binary foo would execute the unmodified version of foo.cpp whereas the binary foo_instrument would execute the instrumented version of foo.cpp which would perform runtime fault injection.

**IMPORTANT NOTE:- Please note that the instrumented version of the binary (in this case: foo_instrument) MUST NOT be executed from the terminal else you would get "segmentation fault" error. The instrumented binary expects certain configuration files which are automatically generated by a python script provided with the VULFI release. Therefore, the instrumented binary must always be executed using the python script as explained in the next step.**

##### Step 3: Execution.
VULFI automates the fault injection process mainly consisting of **execution** and **result generation** phases, using a python script that can be located at <vulfi dir>/master/scripts/driver.py. The python script provides two modes of operation: **standalone** and **comparison** modes. In standalone mode, only instrumented binary is needed (foo\_instrument) for execution. In comparison mode, we require both uninstrumented and instrumented binaries (foo and foo\_instrument) for execution. At the end of the execution, result produced by both the binaries are compared to report a potential occurrence of silent data corruption. A detailed information (e.g., position of the flipped bit, targeted instruction, runtime value which is corrupted, etc.) about the fault injection is written to a CSV file. In case of multiple runs, the results from each run is appended to the same CSV file. The command-line information of the python script could be printed using below command.

```
python <vulfi dir>/master/scripts/driver.py --help
```

Command output:

```
usage: driver.py [-h] [-e] [-s] [-c] [--ov] [--pv] [--iter EXEC_COUNT]
                 [--fcp {bex,img,num}] [--th TH] [--exec1 EXEC1] 
                 [--cmd1 CMD1] [--out1 OUT1] [--cho] [--cfs] 
                 [--exec2 EXEC2] [--cmd2 CMD2]
                 [--out2 OUT2] [--rslt RSLT] [--ficsv FICSV]
                 [--fia {cbr,abr,msk}] [--fib {1,2,3,4,5,6,7,8}]
                 [--fid {msb,lsb}] [--fbu FBU] [--fbl FBL] [--np NUM_PROB]
                 [--dp DEN_PROB] [--ff {eql,max,min,nlm}] [--fc FICOUNT]

Process commandline options..

optional arguments:
  -h, --help            show this help message and exit
  -e                    execution mode.
  -s                    standalone mode, always used in combination with 
                        -e option.
  -c                    comparison mode, always used in combination with 
                        -e option.
  --ov                  Calculate overhead
  --pv                  override user provided fault injection probability..
  --iter EXEC_COUNT     Number of iterations.
  --fcp {bex,img,num}   file comparison mode.
  --th TH               threshold value, only valid with --fcp=img or num.
  --exec1 EXEC1         location of the 1st executable to be executed with -s 
                        or -c mode.
  --cmd1 CMD1           command line args for the exe provided using --exec1 
                        option.
  --out1 OUT1           name (including path) of the output generated by 
                        --exec1 option.
  --cho                 this flag indicates that use cached output for exec1 if 
                        available
  --cfs                 this flag indicates that use cached fault site count
  --exec2 EXEC2         location of the instrumented executable to be executed 
                        only with -c.
  --cmd2 CMD2           command line args for the exe provided using --exec2 option.
  --out2 OUT2           name (including path) of the output generated by --exec2 
                        option.
  --rslt RSLT           name of the output file where result will be written
  --ficsv FICSV         name of the input csv file containing intermediate fi data.
  --fia {cbr,abr,msk}   fault injection algorithm, valid options: 
                         	cbr - cumulative byte ordering; target one or more 
                         	contiguous bytes for fault injection.
                        	abr - absolute bit range;target a bit-range for fault 
                        	injection
  --fib {1,2,3,4,5,6,7,8}
                        number of contiguous bytes to be considered for fault 
                        injection
                        Note:this option must be provided when using -fia=cbr
                        
  --fid {msb,lsb}       direction from which no. of contiguous bytes to be considered 
                        for 
                        fault injection
                        Note:this option must be provided when using -fia=cbr
                        
  --fbu FBU             upper bound for the fault injection bit-range.
                        Note:this option must be provided when using -fia=abr
                        
  --fbl FBL             lower bound for the fault injection bit-range.
                        Note:this option must be provided when using -fia=abr
  --np NUM_PROB         numerator of fault injection probability expressed as a 
                        fraction.
  --dp DEN_PROB         denominator of fault injection probability expressed as a 
                        fraction.
  --ff {eql,max,min,nlm}
                        puts restriction on no. of faults that will be injected, 
                        valid options:
                        	eql - execution with fault injection count equal to the 
                        	val provided by --fc.
                        	max - execution with fault injection count at max equal 
                        	to the val provided by --fc.
                        	min - execution with fault injection count at least equal 
                        	to the val provided by --fc.
                        	nlm - execution with fault injection count having no limit.
  --fc FICOUNT          fault injection countm this value is used in combination with 
                        --ff.
  ```

