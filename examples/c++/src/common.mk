# Requires following variables to be initialized through command line
# VULFI_SRC_DIR and VULFI_INSTALL_DIR

# Example specific variable initializations
# FN_LIST, EX_NAME

EX_DIR = .
BUILD_DIR = $(EX_DIR)/bc

# compiler and flags
CC=clang
CXX = clang++
CXX_FLAGS = -o3
INCLUDE_DIRS=-I$(VULFI_SRC_DIR)/runtime/ -I$(BUILD_DIR)/ -I/usr/include/x86_64-linux-gnu/c++/5/
LIBS = -lm

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	LIBSUFFIX="so"
endif
ifeq ($(UNAME), Darwin)
		LIBSUFFIX="dylib"
endif

all: mkd cc lnk ld exe
cc: cc1 cc2 cc3
lnk: lnk1
ld: ld1 ld2 ld3
exe: exe1 exe2 exe3 exe4


mkd:
	mkdir -p $(BUILD_DIR)/


#########################################
# BITCODE GENERATION 
#########################################
cc1:
	$(CXX) $(CXX_FLAGS) $(INCLUDE_DIRS) $(VULFI_SRC_DIR)/runtime/Corrupt.C \
	-emit-llvm -g -c -o $(BUILD_DIR)/Corrupt.bc

cc2:
	$(CXX) $(CXX_FLAGS) $(INCLUDE_DIRS) $(EX_DIR)/$(EX_NAME).cpp -emit-llvm -g \
	-c -o $(BUILD_DIR)/$(EX_NAME).bc
	llc $(BUILD_DIR)/$(EX_NAME).bc -filetype=obj

cc3:
	$(CXX) $(CXX_FLAGS) $(INCLUDE_DIRS) $(EX_DIR)/$(EX_NAME).cpp -DINST \
	-emit-llvm -g -c -o $(BUILD_DIR)/$(EX_NAME)_inst.bc

################################
# LINK BITCODE FILES
################################
lnk1:
	llvm-link $(BUILD_DIR)/Corrupt.bc \
	$(BUILD_DIR)/$(EX_NAME)_inst.bc -o \
	$(BUILD_DIR)/$(EX_NAME)_corrupt.bc
	llc $(BUILD_DIR)/$(EX_NAME)_corrupt.bc -filetype=obj

################################
# LOAD FAULT INJECTION PASS
################################
ld1:    
	opt -load $(VULFI_INSTALL_DIR)/LLVMVulfi.$(LIBSUFFIX) -vulfi -fn $(FN_LIST) \
	-fsa "data" -lang "C" -dbgf "dbgData_"$(EX_NAME)"_data.csv" < \
	$(BUILD_DIR)/$(EX_NAME)_corrupt.bc > \
	$(BUILD_DIR)/$(EX_NAME)_"data"_inject.bc
	llc $(BUILD_DIR)/$(EX_NAME)_"data"_inject.bc -filetype=obj

ld2:    
	opt -load $(VULFI_INSTALL_DIR)/LLVMVulfi.$(LIBSUFFIX) -vulfi -fn $(FN_LIST) \
	-fsa "ctrl" -lang "C" -dbgf "dbgData_"$(EX_NAME)"_ctrl.csv" < \
	$(BUILD_DIR)/$(EX_NAME)_corrupt.bc > \
	$(BUILD_DIR)/$(EX_NAME)_"ctrl"_inject.bc
	llc $(BUILD_DIR)/$(EX_NAME)_"ctrl"_inject.bc -filetype=obj

ld3:    
	opt -load $(VULFI_INSTALL_DIR)/LLVMVulfi.$(LIBSUFFIX) -vulfi -fn $(FN_LIST) \
	-fsa "addr" -lang "C" -dbgf "dbgData_"$(EX_NAME)"_addr.csv" < \
	$(BUILD_DIR)/$(EX_NAME)_corrupt.bc > \
	$(BUILD_DIR)/$(EX_NAME)_"addr"_inject.bc
	llc $(BUILD_DIR)/$(EX_NAME)_"addr"_inject.bc -filetype=obj



################################
# GENERATE EXECUTABLE FILES
################################
exe1:
	$(CXX) $(CXX_FLAGS) $(BUILD_DIR)/$(EX_NAME).o -o \
	$(BUILD_DIR)/$(EX_NAME) $(LIBS)	


exe2:
	$(CXX) $(CXX_FLAGS) $(BUILD_DIR)/$(EX_NAME)_"data"_inject.o -o \
	$(BUILD_DIR)/$(EX_NAME)_"data"_inject $(LIBS)

exe3:
	$(CXX) $(BUILD_DIR)/$(EX_NAME)_"ctrl"_inject.o -o \
	$(BUILD_DIR)/$(EX_NAME)_"ctrl"_inject $(LIBS)

exe4:
	$(CXX) $(CXX_FLAGS) $(BUILD_DIR)/$(EX_NAME)_"addr"_inject.o -o \
	$(BUILD_DIR)/$(EX_NAME)_"addr"_inject $(LIBS)

clean:
	rm -rf bc/
	rm -f *.csv *.in *.out *.dat *.ppm *~ *.txt
