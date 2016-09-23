
all:run
	
cc:
	(cd c/src/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))		
	(cd c++/src/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))		
	
run:cc
	(cd c/run/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))	
	(cd c++/run/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))	

clean:	
	rm -f *~
	(cd c/src/; make clean)
	(cd c/run/; make clean)
	(cd c++/src/; make clean)	
	(cd c++/run/; make clean)





