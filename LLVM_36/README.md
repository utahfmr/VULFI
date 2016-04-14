VULFI - An LLVM based Fault Injector
=====================================

#### Version 1.0

## Introduction
VULFI is an instruction-level fault injection framework developed using LLVM compiler infrastructure. 
VULFI performs fault injection at LLVM's intermediate representation (IR) level. Any front-end compiler 
which generates LLVM IR can be targeted for fault injection using VULFI. VULFI has been extensively 
tested with C++ and ISPC languages. It is capable of injecting faults into both vector and scalar 
instructions. 
