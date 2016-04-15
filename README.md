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

