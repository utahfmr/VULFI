#include "InstructionType.h"

using namespace llvm;
using namespace std;

// @brief - checks if a given instruction is of a target type and
// also matches the fs algorithm
bool InstructionType::isInstrDesiredType(Instruction* instr, Common::FSAlgo fsalgo,
		                                 Common::TargetType tgTy){
	//Type refs
	Type* ty = instr->getType();
	Type* ety = NULL;

	// Type IDs
	// instruction type ID
	Type::TypeID tyID = instr->getType()->getTypeID();
	// element type ID - used in case the instruction is of type VectorTy
	Type::TypeID etyID = Type::VoidTyID;

	if(tyID == Type::VectorTyID){
		ety = instr->getType()->getVectorElementType();
		etyID = instr->getType()->getVectorElementType()->getTypeID();
	}

	bool addrFlag = false;
	bool cmpFlag = false;
	bool vcmpFlag = false;
	bool intFlag = false;

	if(dyn_cast<CmpInst>(instr)){
		if(tyID != Type::VectorTyID){
			cmpFlag=true;
		} else {
			vcmpFlag=true;
		}
	}

	if(tyID == Type::IntegerTyID){
		intFlag =   (tyID==Type::IntegerTyID &&
			        (ty->isIntegerTy(16)  ||
			        ty->isIntegerTy(32)  ||
			        ty->isIntegerTy(64)));
	} else if(tyID == Type::VectorTyID){
		intFlag =   (etyID==Type::IntegerTyID &&
			        (ety->isIntegerTy(16) ||
			        ety->isIntegerTy(32) ||
			        ety->isIntegerTy(64)));
	}

	bool floatFlag = (tyID==Type::FloatTyID  ||
			         tyID==Type::DoubleTyID) ||
			         (tyID==Type::VectorTyID &&
			         (etyID==Type::FloatTyID ||
			         etyID==Type::DoubleTyID));

	bool voidFlag = (tyID == Type::VoidTyID   ||
			        (tyID == Type::VectorTyID &&
			        etyID == Type::VoidTyID));

	if(tyID == Type::PointerTyID){
		addrFlag  =   true;
	} else if(tyID == Type::VectorTyID &&
			 (etyID == Type::PointerTyID)){
		addrFlag =   true;
	}

	if(fsalgo == Common::FS_DATA){
		if(tgTy == Common::DEFN && (intFlag || floatFlag) && !cmpFlag && !vcmpFlag){
			return true;
		} else if(tgTy == Common::USER && (intFlag || floatFlag || voidFlag) && !cmpFlag && !vcmpFlag){
			return true;
		}
	} else if(fsalgo == Common::FS_DINT){
		if(tgTy == Common::DEFN && intFlag && !cmpFlag && !vcmpFlag){
			return true;
		} else if(tgTy == Common::USER && (intFlag || voidFlag) && !cmpFlag && !vcmpFlag){
			return true;
		}
	} else if(fsalgo == Common::FS_DFLO){
		if(tgTy == Common::DEFN && floatFlag && !cmpFlag && !vcmpFlag){
			return true;
		} else if(tgTy == Common::USER && (floatFlag || voidFlag) && !cmpFlag && !vcmpFlag){
			return true;
		}
	} else if(fsalgo == Common::FS_CTRL){
		return (cmpFlag || vcmpFlag); // do not distinguish between scalar or vector control-flow for now
	} else if(fsalgo == Common::FS_VCTL){
		return (cmpFlag || vcmpFlag); // do not distinguish between scalar or vector control-flow for now
	} else if(fsalgo == Common::FS_ADDR){
		return addrFlag;
	}
	return false;
}


