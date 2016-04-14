
#include "IntrinsicsDef.h"

using namespace llvm;
using namespace std;

IntrinsicsDef::IntrinsicsDef(Common::ArchType archty){
	this->arch = archty;
	if(archty == Common::X86){
		DEBUG(errs() << "\nArchitecure type is x86\n");
		this->instrinsicListNoMask = x86InstrinsicListNoMask;
		this->instrinsicListMaskArg2 = x86InstrinsicListMaskArg2;
		this->instrinsicListMaskArg3 = x86InstrinsicListMaskArg3;
		this->instrinsicListMaskArg4 = x86InstrinsicListMaskArg4;
		this->instrinsicListBitWiseMaskArg2 = x86InstrinsicListBitWiseMaskArg2;
		this->instrinsicListBitWiseMaskArg3 = x86InstrinsicListBitWiseMaskArg3;
		this->instrinsicListBitWiseMaskArg4 = x86InstrinsicListBitWiseMaskArg4;
		this->instrinsicListBitWiseMaskArg5 = x86InstrinsicListBitWiseMaskArg5;
		this->instrinsicListCtrlMaskArg2 = x86InstrinsicListCtrlMaskArg2;
		this->instrinsicListCtrlMaskArg3 = x86InstrinsicListCtrlMaskArg3;
		this->instrinsicListCtrlMaskArg4 = x86InstrinsicListCtrlMaskArg4;
	}

	populateList(instrinsicListNoMask,instrinsicSetNoMask);
	populateList(instrinsicListMaskArg2,instrinsicSetMaskArg2);
	populateList(instrinsicListMaskArg3,instrinsicSetMaskArg3);
	populateList(instrinsicListMaskArg4,instrinsicSetMaskArg4);
	populateList(instrinsicListBitWiseMaskArg2,instrinsicSetBitWiseMaskArg2);
	populateList(instrinsicListBitWiseMaskArg3,instrinsicSetBitWiseMaskArg3);
	populateList(instrinsicListBitWiseMaskArg4,instrinsicSetBitWiseMaskArg4);
	populateList(instrinsicListBitWiseMaskArg5,instrinsicSetBitWiseMaskArg5);
	populateList(instrinsicListCtrlMaskArg2,instrinsicSetCtrlMaskArg2);
	populateList(instrinsicListCtrlMaskArg3,instrinsicSetCtrlMaskArg3);
	populateList(instrinsicListCtrlMaskArg4,instrinsicSetCtrlMaskArg4);

}

void IntrinsicsDef::populateList(const string *instrinsicList,
								 set<string> &instrinsicSet){
	for(int i=0;!instrinsicList[i].empty();i++){
		DEBUG(errs() << "\nInserting " << instrinsicList[i] << "\n");
		instrinsicSet.insert(instrinsicList[i]);
	}
	return;
}

string IntrinsicsDef::getIntrinsic(CallInst* instr){
	Function* fn = instr->getCalledFunction();
	string intName;
	if(fn->isIntrinsic()){
		intName = fn->getName().str();
		DEBUG(errs() << "\nName of the intrinsic function: " << fn->getName() << "\n");
	}
	return intName;
}

//bool IntrinsicsDef::usesMask(Instruction* instr){
//	CallInst* callinstr = dyn_cast<CallInst>(instr);
//	if(callinstr == NULL){
//		return false;
//	}
//
//	if(this->getMask(callinstr)==NULL){
//		return false;
//	}
//
//	return true;
//}

Value* IntrinsicsDef::getMask(CallInst* instr){
	string name = this->getIntrinsic(instr);
	Value* __mask = NULL;
	if(this->instrinsicSetMaskArg2.find(name) != this->instrinsicSetMaskArg2.end()){
		  DEBUG(errs() << "\nFound argument 2 as mask\n");
		__mask = instr->getArgOperand(1);
	} else if(this->instrinsicSetMaskArg3.find(name) != this->instrinsicSetMaskArg3.end()){
		DEBUG(errs() << "\nFound argument 3 as mask\n");
		__mask = instr->getArgOperand(2);
	} else if(this->instrinsicSetMaskArg4.find(name) != this->instrinsicSetMaskArg4.end()){
		DEBUG(errs() << "\nFound argument 4 as mask\n");
		__mask = instr->getArgOperand(3);
	}

	if(this->instrinsicSetCtrlMaskArg2.find(name) != this->instrinsicSetCtrlMaskArg2.end()){
		  DEBUG(errs() << "\nFound argument 2 as mask\n");
		__mask = instr->getArgOperand(1);
	} else if(this->instrinsicSetCtrlMaskArg3.find(name) != this->instrinsicSetCtrlMaskArg3.end()){
		DEBUG(errs() << "\nFound argument 3 as mask\n");
		__mask = instr->getArgOperand(2);
	} else if(this->instrinsicSetCtrlMaskArg4.find(name) != this->instrinsicSetCtrlMaskArg4.end()){
		DEBUG(errs() << "\nFound argument 4 as mask\n");
		__mask = instr->getArgOperand(3);
	}


	if(this->instrinsicSetBitWiseMaskArg2.find(name) != this->instrinsicSetBitWiseMaskArg2.end() ||
	   this->instrinsicSetBitWiseMaskArg3.find(name) != this->instrinsicSetBitWiseMaskArg3.end() ||
	   this->instrinsicSetBitWiseMaskArg4.find(name) != this->instrinsicSetBitWiseMaskArg4.end() ||
	   this->instrinsicSetBitWiseMaskArg5.find(name) != this->instrinsicSetBitWiseMaskArg5.end()){
		errs() << "\nWarning: found an intrinsic: "<< name
			   << ", w/ bitwise mask for which supported is yet to be added ";
		errs() << "\nPlease notify the developer of VULFI! \n";
		return __mask;
	}

	DEBUG(errs() << "\nMask is: " << *__mask << "\n");
	return __mask;
}
