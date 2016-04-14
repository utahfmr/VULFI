
all:
	(cd bsearch/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))
	(cd flyod/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))	
	(cd hstone/; make VULFI_SRC_DIR=$(VULFI_SRC_DIR) VULFI_INSTALL_DIR=$(VULFI_INSTALL_DIR))


clean: 
	rm -f *~
	(cd bsearch/; make clean)
	(cd flyod/; make clean)	
	(cd hstone/; make clean)





