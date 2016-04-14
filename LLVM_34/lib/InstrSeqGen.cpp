
#include <fstream>
#include "InstrSeqGen.h"
#include "DataSites.h"
#include "IntrinsicsDef.h"

using namespace llvm;
using namespace std;

InstrSeqGen::InstrSeqGen(){
}

void InstrSeqGen::printSequences(vector<vector<Instruction*> > seqmap, CLData* Cl){
  if(seqmap.size()==0){
    errs() << "\nError: No valid instruction sequences are found!\n";
    exit(-1);
  }

  map<string,int> fingerprintmap;
  for(unsigned i=0;i<seqmap.size();i++){
    string seqfp = getSeqFingerPrint(seqmap[i]);
    if(fingerprintmap.find(seqfp) == fingerprintmap.end()){
      fingerprintmap.insert(pair<string,int>(seqfp,1));
    } else {
      fingerprintmap[seqfp] += 1;
    }
    DEBUG(errs() << "\n==========================================");
    DEBUG(errs() << "\nPrinting instruction sequence # " << i+1);
    DEBUG(errs() << "\nSequence depth is: " << seqmap[i].size());
    DEBUG(errs() << "\n==========================================\n");
    for(unsigned j=0;j<seqmap[i].size();j++){
      DEBUG(errs() << *(seqmap[i][j]) << "\n");
    }
    DEBUG(errs() << "\n==========================================\n\n");
  }

  fstream fs;
  int depth = Cl->getDepth();
  stringstream ss;
  ss << depth;
  string filename = "fingerprintdb_depth_"+ss.str()+".csv";
  fs.open (filename.c_str(), ofstream::app);
  for(map<string,int>::iterator it = fingerprintmap.begin();it!=fingerprintmap.end();it++){
    fs << it->first << "," << it->second << "\n";
  }
  fs.close();
  return;
}

void InstrSeqGen::instrumentSeq(Module *M, CLData *Cl, FunctionList *Fl, vector<vector<Instruction*> > seqmap){
  int targetidx = Cl->getTargetInstrIdx();
  if(targetidx >= (int) seqmap.size()){
    errs() << "\nError: Target sequence out of bound!\n";
    exit(-1);
  }

  vector<Instruction*> targetseq = seqmap[targetidx];

  int targetdepth = Cl->getDepth();
  if(targetdepth>=(int)targetseq.size()){
    errs() << "\nError: Target sequence depth out of bound!\n";
    exit(-1);
  }

  return;
}

vector<vector<Instruction*> > InstrSeqGen::genInstrSequence(Module *M, CLData *Cl, FunctionList *Fl){
  DataSites ds;
  set<Instruction*> instrlist = ds.getfaultSites(M,Cl,Fl);
  vector<vector<Instruction*> > seqmap;
  for(set<Instruction*>::iterator it=instrlist.begin();it!=instrlist.end();it++){
    Instruction* currentInstr = *it;
    DEBUG(errs() << "\nCurrent instruction: \n" << *currentInstr);
    if(dyn_cast<PHINode>(currentInstr)){
      continue;
    }
    updateSeqMap(currentInstr,seqmap,Cl->getDepth(),currentInstr->getParent());
  }
  seqmap = validateSeqs(seqmap,Cl->getDepth());
  return seqmap;
}

string InstrSeqGen::getStringSeq(vector<Instruction*> instrseq){
  string strseq="";
  for(unsigned i=0;i<instrseq.size();i++){
    strseq=strseq+Common::instrToString(instrseq[i]);
  }
  return strseq;
}

vector<string> InstrSeqGen::getOperandNameList(map<Value*, string> instrregmap, Instruction* instr, int* regcounter){
  vector<string> namelist;
  unsigned numop = instr->getNumOperands();
  //	errs() << "\nCurrent instruction: " << instr << "\n";
  for(unsigned i=0;i<numop;i++){
    //		errs() << "\nCurrent operand: " << instr->getOperand(i) << "\n";
    Type* instrTy = instr->getOperand(i)->getType();
    Value* currOp = instr->getOperand(i);
    string currOpName="";
    string regname="";
    if(instrregmap.find(currOp) != instrregmap.end() &&
       !instr->isCommutative(instr->getOpcode()) ){
      regname = instrregmap[currOp];
    } else {
      regname =  "newreg_"+Common::toString<int>(*regcounter);
      (*regcounter)++;
    }

    if(instrTy->isIntegerTy()){
      currOpName = currOpName + "int";
      currOpName = currOpName + Common::toString<unsigned>(instrTy->getIntegerBitWidth());
    } else if(instrTy->isFloatTy()) currOpName = currOpName + "float";
    else if(instrTy->isDoubleTy()) currOpName = currOpName + "double";
    currOpName = currOpName + " " + regname;
    namelist.push_back(currOpName);
  }
  return namelist;
}

string InstrSeqGen::getSeqFingerPrint(vector<Instruction*> instrseq){
  // finger print format:
  // instruction name; first op type; second op type; ..;return type
  map<Value*,string> instrregmap;
  for(unsigned i=0;i<instrseq.size();i++){
    Instruction* cuurentInstr = instrseq[i];
    if(instrregmap.find(cuurentInstr)==instrregmap.end()){
      instrregmap.insert(pair<Value*,string>(cuurentInstr,"instr_"+Common::toString<unsigned>(i)));
    }
  }

  string seqfingerprint;
  int newRegCounter=0;
  for(unsigned i=1;i<instrseq.size();i++){
    // finger print format:
    // instruction name; first op type; second op type; ..;return type
    Instruction* instr = instrseq[i];
    string fingerprint;
    string opcodeName(instr->getOpcodeName());
    fingerprint = opcodeName + "<";
    vector<string> opNameList = getOperandNameList(instrregmap,instr,&newRegCounter);
    for(unsigned j=0;j<opNameList.size();j++)
      fingerprint = fingerprint + " " + opNameList[j];
    fingerprint=fingerprint + ">";
    seqfingerprint=seqfingerprint+ fingerprint;
  }
  DEBUG(errs() << "\nSequence fingerprint is: \n" << seqfingerprint << "\n\n");
  return seqfingerprint;
}


vector<vector<Instruction*> >
InstrSeqGen::validateSeqs(vector<vector<Instruction*> > seqmap, int depth){
  vector<vector<Instruction*> > cleanseq;
  bool insertFlag = true;
  for(unsigned i=0;i<seqmap.size();i++){
    if(seqmap[i].size()!=(unsigned)depth){
      insertFlag = false;
    } else {
      for(unsigned j=0;j<seqmap[i].size();j++){
	Instruction* currentInstr = seqmap[i][j];
	if(j>0){
	  if(dyn_cast<ExtractElementInst>(currentInstr) ||
	     dyn_cast<InsertElementInst>(currentInstr)  ||
	     dyn_cast<PHINode>(currentInstr)            ||
	     dyn_cast<LoadInst>(currentInstr)){
	    insertFlag = false;
	    break;
	  } else if(dyn_cast<CallInst>(currentInstr)){
	    IntrinsicsDef intdef = IntrinsicsDef(Common::X86);
	    string intname = intdef.getIntrinsic(dyn_cast<CallInst>(currentInstr));
	    if(intname.empty()) insertFlag = false;
	  }
	}
      }
    }
    if(insertFlag==true) cleanseq.push_back(seqmap[i]);
    else insertFlag=true;
  }
  errs() << "\nInfo: Number of unverified sequences found: " << seqmap.size();
  errs() << "\nInfo: Number of verified sequences found: " << cleanseq.size();
  errs() << "\nInfo: Number of sequences removed: " << seqmap.size()-cleanseq.size() << "\n\n";
  return cleanseq;
}

void InstrSeqGen::updateSeqMap(Instruction* node, vector<vector<Instruction*> >& seqmap, int depth, BasicBlock* parent){
  if (node == NULL){// ERROR
    this->visitedSeq.clear();
    this->instrseq.clear();
    return;
  }

  if(depth == 0){
    string seqstr = getStringSeq(this->instrseq);
    if (this->instrseq.size()>0 && this->visitedSeq.find(seqstr)==this->visitedSeq.end()){
      seqmap.push_back(this->instrseq);
      this->getSeqFingerPrint(this->instrseq);
      this->instrseq.clear();
      this->visitedSeq.insert(seqstr);
      return;
    }
  }

  if((node->getType()->isIntegerTy()  ||
      node->getType()->isFloatTy()    ||
      node->getType()->isDoubleTy())  &&
     !node->getType()->isIntegerTy(1) &&
     !dyn_cast<PHINode>(node)         &&
     node->getParent() == parent){
    this->instrseq.push_back(node);
  } else {
    this->instrseq.clear();
    return;
  }

  int currentdepth = depth-1;
  for(Value::use_iterator ui = node->use_begin();ui != node->use_end(); ++ui){
    Instruction* currentUser = dyn_cast<Instruction>(*ui);
    if(currentUser){
      updateSeqMap(currentUser,seqmap,currentdepth,parent);
    }
  }
  return;
}
