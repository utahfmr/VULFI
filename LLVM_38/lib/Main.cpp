//===-- Main.cpp - Vulfi class definition -------*- C++ -*-===//
//
//                     The VULFI Framework
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Vulfi class declaration, command line options provide to
/// the users.
///
//===----------------------------------------------------------------------===//

// user defined headers
#include "Dispatcher.h"
#include "CLData.h"
#include "FunctionList.h"
#include "InstrSeqGen.h"

// llvm headers
#include <llvm/Support/CommandLine.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Module.h>

using namespace llvm;
using namespace std;

static cl::opt<string> fn("fn", cl::desc("Name(s) of the function(s) to be targeted"),
			  cl::value_desc("func1 func2 func3"), cl::init(""), cl::ValueRequired);

static cl::opt<string> mode("mode", cl::desc("Relief mode - fault injection / protection"),
			    cl::value_desc("inj/pct"),
			    cl::init("inj"), cl::ValueRequired);

static cl::opt<string> fsa("fsa", cl::desc("Fault Site Selection Algorithm"),
			   cl::value_desc("data/dint/dflo/ctrl/vctl/addr/mask/brdc"),
			   cl::init("data"), cl::ValueRequired);

static cl::opt<string> flr("flr", cl::desc("Name of the file containing source \
code range provided by the user"), cl::value_desc("file name"), cl::init(""),
			   cl::ValueRequired);

static cl::opt<int> rf("rf", cl::desc("range flag"), cl::init(0), cl::ValueRequired,
		       cl::value_desc("0:feature off,1:range selection for fault injection,2:range selection \
for fault analysis"));

static cl::opt<int> sidx("sidx", cl::desc("target instruction idx"), cl::init(-1), cl::ValueRequired,
			 cl::value_desc("-1: All instruction selected as per FSA will be targeted; \
		       >-1: Instruction with sidx will be targeted"));

static cl::opt<string> lang("lang", cl::desc("Target language, currently C \
, C++, and ISPC are the supported ones"), cl::value_desc("C/C++/ISPC/OCL"), cl::init("C++"),
			    cl::ValueRequired);

static cl::opt<string> arch("arch", cl::desc("Architecture type"),
			    cl::value_desc("x86/neon/nvvm/mips/spu"),
			    cl::init("x86"), cl::ValueRequired);

static cl::opt<string> dbgf("dbgf", cl::desc("Debug file name with csv extension"),
			    cl::value_desc("dbgfile.csv"), cl::init(""));

/*! \mainpage Code Documentation
 *
 * <A href="http://utahfmr.github.io/vulfi.github.io/">[Back to VULFI Home Page]</A>
 *
 * \section intro Introduction
 * <A href="http://formalverification.cs.utah.edu/fmr/vulfi/">VULFI</A>
 * is an instruction-level fault injection framework developed using
 * <A href="http://www.llvm.org/">LLVM compiler infrastructure</A>.
 * VULFI also provides necessary infrastructure to automate fault injection
 * and result generation processes. VULFI performs fault injection at
 * LLVM's intermediate representation (IR) level. It can target both
 * vector and scalar instructions including architectural specific
 * LLVM intrinsics.
 *
 * \section ex Extending VULFI
 * Below figure shows VULFI's block-level design. VULFI's modular
 * design makes it easier to modify or extend.
 * Specifically, VULFI provides key interfaces (through pure virtual functions)
 * for fault site selection, fault injection etc. It also provides a default set
 * of implementations for these interfaces. Refer to the code documentation
 * of CLData, SiteSelector, FaultInjector, and Dispatcher classes
 * to learn about how to add a new feature in VULFI.
 *
 *
 * <IMG SRC="../img/VULFI-Design.png" WIDTH=1000 HEIGHT=500>

 * \section lang Supported Languages
 * VULFI currently supports C, C++, <A href="https://ispc.github.io/">ISPC</A>, 
 <A href="https://www.khronos.org/opencl/">OpenCL</A>, and MPI-C languages. Note that to target OpenCL programs for fault injection using VULFI, the program should be compiled using <A href="https://software.intel.com/en-us/node/530783">Intel's OpenCL Compiler</A>. Conceptually, VULFI can target any high level language which can be compiled to LLVM's <A href="http://llvm.org/docs/LangRef.html"> intermediate representation</A>.
 *  
 */
namespace {
  class Vulfi : public ModulePass{
  public:
    static char ID; 
    Vulfi() : ModulePass(ID) {}
    virtual bool runOnModule(Module &M){
      CLData *Cl = new CLData();
      // Initialize CommandLine Object
      Cl->setMode(mode.getValue());
      Cl->setFSAlgo(fsa.getValue());
      Cl->setFnList(fn.getValue());
      Cl->setLang(lang.getValue());
      // Cl->setInstMainFlag(imn.getValue());
      Cl->setRangeFlag(rf.getValue());
      if(sidx.getValue() > -1)
	Cl->setTargetInstrIdx(sidx.getValue());
      Cl->setRangeFileName(cast<string>(flr));
      Cl->setDbgFile(dbgf);
      Cl->setArch(arch);
      Cl->runCLValidator();

      // Construct dispatcher object which orchestrate rest over activities:
      Dispatcher *Ds = new Dispatcher(&M,Cl);
      delete Cl;
      delete Ds;
      return true;
    }
    ~Vulfi(){}
  };
}

char Vulfi::ID = 0;
static RegisterPass<Vulfi> X0("vulfi","Vectorization aware Utah's LLVM based Fault Injection Framework",false,false);


