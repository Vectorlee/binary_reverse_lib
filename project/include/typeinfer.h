

#ifndef TYPEINFER_H
#define TYPEINFER_H

#include <map>
#include <list>
#include <vector>

#include "register.h"
#include "tracker.h"
#include "trailer.h"

#include "constraint.h"
extern "C"
{
    #include "xed-interface.h"
}



class BasicBlock
{

public:
	int id;

	int startAddr;        // the start and end address of the basic block
	int endAddr;          // the relative address inside the function.
	// startAddr <= ; < endAddr.

	int depth;              // the depth of the basicblock
	Register* regPtr;       // the register environment after execute this basic block. 


    BasicBlock(int index, int begin, int end){
    	id = index;
    	startAddr = begin;
    	endAddr = end;

    	depth = -1;
    	regPtr = NULL;
    }
};




class TypeHunter
{
private:

	//Register *regPtr;
	Tracker  *TrackState;    //the tracker will assign the id to each variable.
	StackTrailer *trailer;

	std::list<Constraint*> typeContainer;


    // for control flow analysis
	BYTE *startAddr;
	BYTE *endAddr;   // the start and end address of the whole binary.


	std::set<int> nodes;   // we must provide a start and end address of each nodes.
    std::multimap<int, int> edges;


    int blockNum;    // the number of the block
    int **matrix;    // adjacency matrix
    std::vector<BasicBlock*> blockContainer;     // the place we store basic block

public:

	TypeHunter(BYTE* start, BYTE* end, std::map<int, AbstractVariable*> &container);

	~TypeHunter();

	void analyzeBinary();

	void getResult(std::list<Constraint*> &container, std::map<int, AbstractVariable*> &variable);


private:

	// functions to generate cfg

	bool buildCFG();       // find the edges and nodes in the binary code. 

	void buildMatrix();    // build the adjacency matrix according to the edges and nodes.

	void calcDeep();       // calculate the depth of each basic block.

	bool inferType(BasicBlock* block);   

	std::vector<BasicBlock*>::iterator findNext(int deep, std::vector<BasicBlock*> &container);

    Register* mergeBranch(int index); // the index is the index of the basic block

    void mergeRegister(Register *dest, std::list<BasicBlock*> &container);


    // functions to infer type constraints

	void assignConstraint(xed_decoded_inst_t &xedd);

	void generateConstraint(xed_decoded_inst_t &xedd);

	void resetState(Register* reg_file);


};


#endif