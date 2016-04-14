# Requires following variables to be initialized through command line
# VULFI_SRC_DIR

# Example specific variable initializations
# EX_NAME, FCP

EX_DIR = ../../src/$(EX_NAME)
BUILD_DIR = $(EX_DIR)/bc

CMD1 = "result_noerror.dat"
CMD_DATA = "result_error_data.dat"
CMD_CTRL = "result_error_ctrl.dat"
CMD_ADDR = "result_error_addr.dat"

OUT1 = result_noerror.dat
OUT_DATA = "result_error_data.dat"
OUT_CTRL = "result_error_ctrl.dat"
OUT_ADDR = "result_error_addr.dat"


all: $(TARGETS)

ex1:
	python $(VULFI_SRC_DIR)/scripts/driver.py -e -c $(FCP) \
	--exec1 $(BUILD_DIR)/$(EX_NAME) \
	--cmd1 $(CMD1) \
	--out1 $(OUT1) \
	--exec2 $(BUILD_DIR)/$(EX_NAME)_data_inject \
	--cmd2 $(CMD_DATA) \
	--out2 $(OUT_DATA) \
	--iter 1 --pv --ff eql --fc 1 \
	--rslt result_$(EX_NAME)_data.csv --ficsv fi.csv \
	--th 0.0
	rm -f *.in *.out *.dat *.ppm *~
	
ex2:
	python $(VULFI_SRC_DIR)/scripts/driver.py -e -c $(FCP) \
	--exec1 $(BUILD_DIR)/$(EX_NAME) \
	--cmd1 $(CMD1) \
	--out1 $(OUT1) \
	--exec2 $(BUILD_DIR)/$(EX_NAME)_ctrl_inject \
	--cmd2 $(CMD_CTRL) \
	--out2 $(OUT_CTRL) \
	--iter 1 --pv --ff eql --fc 1 \
	--rslt result_$(EX_NAME)_ctrl.csv --ficsv fi.csv \
	--th 0.0
	rm -f *.in *.out *.dat *.ppm *~
	
ex3:
	python $(VULFI_SRC_DIR)/scripts/driver.py -e -c $(FCP) \
	--exec1 $(BUILD_DIR)/$(EX_NAME) \
	--cmd1 $(CMD1) \
	--out1 $(OUT1) \
	--exec2 $(BUILD_DIR)/$(EX_NAME)_addr_inject \
	--cmd2 $(CMD_ADDR) \
	--out2 $(OUT_ADDR) \
	--iter 1 --pv --ff eql --fc 1 \
	--rslt result_$(EX_NAME)_addr.csv --ficsv fi.csv \
	--th 0.0
	rm -f *.in *.out *.dat *.ppm *~

clean:
	rm -f *.csv *.in *.out *.dat *.ppm *~
