#include "Slice.h"

using namespace llvm;
using namespace std;

Slice::Slice(){
  // init rank
  this->rank = 0.0;
}

void Slice::fillExtdLists(set<Instruction*> instrList, CLData *Cl, bool writeFlag){

  for(set<Instruction*>::iterator si=instrList.begin();si!=instrList.end();++si){

    // write DAG for each instruction in the instrlist
    if(writeFlag){
      this->visitedNodes.clear();
      writeDAG(*si);
      this->visitedNodes.clear();
      calculateRank(*si,0);
      writeRank(*si);
      this->rank=0;
      this->visitedNodes.clear();
    }

    // get forward and backward slice for each instruction in the instrlist
    // extdDefUseList -> forward slice
    // extdUseDefList -> backward slice
    this->visitedNodes.clear();
    traverseGraph(*si);
    if(!this->visitedNodes.empty()){
      set<Instruction*>  users;
      // From VisitedNodes save Instruction and its typeID into Users
      for(set<Instruction*>::iterator si1=this->visitedNodes.begin();
	  si1!=this->visitedNodes.end();++si1){
	if(users.find(*si1) == users.end()){
	  users.insert(*si1);
	}
      }

      if(this->extdDefUseList.find(*si) == this->extdDefUseList.end()){
	this->extdDefUseList.insert(pair<Instruction*,
				    set<Instruction*> >(*si,users));
      }

      // Now traverse through each user and update the
      // current instruction against these users
      for(set<Instruction*>::iterator si1=users.begin();
	  si1!=users.end();++si1){
	if(this->extdUseDefList.find(*si1) == this->extdUseDefList.end()){
	  set<Instruction*> tempDef;
	  tempDef.insert(*si);
	  this->extdUseDefList.insert(pair<Instruction*,
				      set<Instruction*> >(*si1,tempDef));
	} else{
	  extdUseDefList[*si1].insert(*si);
	}
      }
    }
  }

  this->visitedNodes.clear();
  return;
}

void Slice::writeRank(Instruction* node){
  DEBUG(errs() << "\nRank of instruction: \n" << Common::instrToString(node)
	<< " is: " << this->rank << "\n\n\n");
  string fname = "rank.csv";
  ofstream fout;
  if(!Common::fileExists(fname)){
    // write header in the CSV file
    fout.open(fname.c_str(),ios::out);
    fout << "instruction,rank\n";
  } else {
    fout.open(fname.c_str(),ios::app);
  }

  string instrname = Common::instrToString(node);
  Common::replace_all(instrname,",","#");
  fout << instrname << "," << this->rank << "\n";
  fout.close();
  return;
}

bool Slice::isSelectInstr(Instruction* node){
  if (dyn_cast<CallInst>(node)){
    CallInst* callinst = dyn_cast<CallInst>(node);
    if(callinst->getCalledFunction()->isIntrinsic()){
      string intrinsic = callinst->getCalledFunction()->getName().str();
      if(intrinsic.find("llvm.x86.avx2.pblend",0)!=string::npos    ||
	 intrinsic.find("llvm.x86.avx.blend",0)!=string::npos      ||
	 intrinsic.find("llvm.x86.sse41.blend",0)!=string::npos    ||
	 intrinsic.find("llvm.x86.sse41.pblend",0)!=string::npos    ||
	 intrinsic.find("llvm.x86.sse41.blendv",0)!=string::npos    ||
	 intrinsic.find("llvm.x86.avx2.pshuf",0)!=string::npos){
	return true;
      }
    }
  }
  return false;
}

bool Slice::isMaskedControlInstr(Instruction* node){
  if(dyn_cast<CmpInst>(node)){
    return true;
  }
  return false;
}

bool Slice::isMaskedLoad(Instruction* node){
  if (dyn_cast<CallInst>(node)){
    CallInst* callinst = dyn_cast<CallInst>(node);
    if(callinst->getCalledFunction()->isIntrinsic()){
      string intrinsic = callinst->getCalledFunction()->getName().str();
      if(intrinsic.find("llvm.x86.avx2.maskload",0)!=string::npos  ||
	 intrinsic.find("llvm.x86.avx.maskload",0)!=string::npos){
	return true;
      }
    }
  }
  return false;
}

bool Slice::isMaskedStore(Instruction* node){
  if (dyn_cast<CallInst>(node)){
    CallInst* callinst = dyn_cast<CallInst>(node);
    if(callinst->getCalledFunction()->isIntrinsic()){
      string intrinsic = callinst->getCalledFunction()->getName().str();
      if(intrinsic.find("llvm.x86.avx2.maskstore",0)!=string::npos ||
	 intrinsic.find("llvm.x86.avx.maskstore",0)!=string::npos){
	return true;
      }
    }
  }
  return false;
}



bool Slice::isTerminalNode(Instruction* node){
  if (isSelectInstr(node) || isMaskedControlInstr(node) ||
      isMaskedLoad(node) || isMaskedStore(node)){
    return true;
  }
  return false;
}

void Slice::writeDAG(Instruction* node){
  // contruct DAG for each mask ins
  ofstream fout;

  // write dot file header
  fout.open((Common::instrToString(node)+".dot").c_str(),ios::out);
  fout << "digraph fwdslice { \n";
  fout << "node [shape=box] \n";

  // write DAG
  map<string,int> edges;
  traverseGraph(node,fout,edges);

  // write dot file footer
  fout << "\n}";
  fout.close();
}

void Slice::calculateRank(Instruction* node, int d){
  if (node == NULL || this->isTerminalNode(node)){
    //		this->rank = min(this->rank,d);
    if(isMaskedControlInstr(node) || isMaskedLoad(node)){
      this->rank += (1.0/(double)d);
    } else if(isMaskedStore(node)){
      this->rank += (2.0/(double)d);
    }
    //		else if(isSelectInstr(node)){
    //			this->rank += (1.0/(double)d);
    //		}
    return;
  }
  d++;
  for(Value::use_iterator ui = node->use_begin();ui != node->use_end(); ++ui){
    Instruction* currentUser = dyn_cast<Instruction>(*ui);
    if(currentUser){
      // Recursively visit nodes not visited earlier and add them to
      // visitedNodes list
      if(this->visitedNodes.find(currentUser) == this->visitedNodes.end()){
	this->visitedNodes.insert(currentUser);
	DEBUG(errs() << "\nVisited Node: " << *currentUser);
	calculateRank(currentUser,d);
      }
    }
  }
  return;
}

void Slice::traverseGraph(Instruction* node, ofstream& fout, map<string,int>& edge){
  if (node == NULL || this->isTerminalNode(node)){
    return;
  }

  for(Value::use_iterator ui = node->use_begin();ui != node->use_end(); ++ui){
    Instruction* currentUser = dyn_cast<Instruction>(*ui);
    if(currentUser){
      // Recursively visit nodes not visited earlier and add them to
      // visitedNodes list
      string wrtline = "\"" + Common::instrToString(node) + "\" -> \"" + Common::instrToString(currentUser) + "\";\n";
      if(edge.find(wrtline) == edge.end()){
	string opname1(node->getOpcodeName());
	fout << wrtline;
	edge.insert(pair<string,int>(wrtline,1));
	traverseGraph(currentUser,fout,edge);
      }
    }
  }
  return;
}

void Slice::traverseGraph(Instruction* node){
  if (node == NULL){
    return;
  }

  for(Value::use_iterator ui = node->use_begin();ui != node->use_end(); ++ui){
    Instruction* currentUser = dyn_cast<Instruction>(*ui);
    if(currentUser){
      // Recursively visit nodes not visited earlier and add them to
      // visitedNodes list
      if(this->visitedNodes.find(currentUser) == this->visitedNodes.end()){
	this->visitedNodes.insert(currentUser);
	DEBUG(errs() << "\nVisited Node: " << *currentUser);
	traverseGraph(currentUser);
      }
    }
  }
  return;
}

void Slice::writeFwdSlice(set<Instruction*>& instrlist, string filename, CLData* Cl){
  ofstream fout;
  fout.open(filename.c_str(),ios::out);
  this->fillExtdLists(instrlist,Cl,true);
  map<Instruction*,set<Instruction*> > fwdSlice
    = this->getExtdDefUseList();
  for(map<Instruction*,set<Instruction*> >::iterator mi
	= fwdSlice.begin();mi!=fwdSlice.end();mi++){
    Instruction* currInstr = mi->first;
    fout << "\nCurrent Instruction: " << Common::instrToString(currInstr) << "\n";
    fout << "\nForward slice instructions: \n";
    fout << "---------------------START----------------------\n";
    set<Instruction*> currSet = mi->second;
    for(set<Instruction*>::iterator si=currSet.begin();si!=currSet.end();si++){
      Instruction* currFwdSliceInstr = *si;
      fout << Common::instrToString(currFwdSliceInstr) << "\n";
    }
    fout << "-------------------END------------------------\n\n\n";
  }
  fout.close();
  return;
}
