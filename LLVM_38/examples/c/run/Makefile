
all:
	(cd lis/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))	
	(cd lu/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))
	(cd qsort/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))


clean: 
	rm -f *~
	(cd lis/; make clean)	
	(cd lu/; make clean)
	(cd qsort/; make clean)	






