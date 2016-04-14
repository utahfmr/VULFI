cmake_minimum_required(VERSION 2.8)
find_package(LLVM)
include(AddLLVM)
include(AddLLVMDefinitions)
include(CheckAtomic)
include(GetHostTriple)
include(HandleLLVMOptions)
include(LLVM-Config)
include(LLVMParseArguments)
include(LLVMProcessSources)
include(TableGen)
include(VersionFromVCS)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_subdirectory(lib)
add_subdirectory(doc)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake") 


