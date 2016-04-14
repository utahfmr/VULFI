
include_directories(../include)
add_compile_options(-std=c++11)

add_llvm_loadable_module(LLVMVulfi MODULE Main.cpp
AddrFaults.cpp
CtrlSites.cpp
FaultInjector.cpp
MaskFaults.cpp
CtrlSitesRange.cpp
FunctionList.cpp
MaskSites.cpp
AddrInstrList.cpp
DataFaults.cpp
InstrList.cpp
ProcessCSV.cpp
Common.cpp
DataInstrList.cpp
InstrSeqGen.cpp
SiteSelector.cpp
AddrSites.cpp
CtrlFaults.cpp
DataSites.cpp
InstructionType.cpp
Slice.cpp
AddrSitesRange.cpp
DataSitesRange.cpp
IntrinsicsDef.cpp
CLData.cpp
CtrlInstrList.cpp
Dispatcher.cpp)

install(TARGETS LLVMVulfi DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
