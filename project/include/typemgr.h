

// This new class is used to encapsulate the typeinfer.
// So we need a good name for it.

#include "utility.h"
#include "register.h"
#include "typeinfer.h"

#include <set>
#include <map>
#include <vector>

// use the adjacency matrix.
// it could be the easilest way.

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



class TypeManager
{

private:

	BYTE *startAddr;
	BYTE *endAddr;   // the start and end address of the whole binary.


	std::set<int> nodes;   // we must provide a start and end address of each nodes.
    std::multimap<int, int> edges;
    // the place we store basic block, the data structure need to be changed.

    std::vector<BasicBlock*> blockContainer;


    int blockNum;    // the number of the block
    int **matrix;    // adjacency matrix

    TypeHunter *typeHunter; 

    // private functions
    // private find function(), return an iterator
    std::vector<BasicBlock*>::iterator findNext(int deep, std::vector<BasicBlock*> &container);

    Register* mergeEnvironment(int index); // the index is the index of the basic block.

public:

	// we add the boundary check in the cfg generation.
	TypeManager(TypeHunter *hunter, BYTE* start, BYTE* end);

	~TypeManager();

	bool buildCFG();       // find the edges and nodes in the binary code. 

	void buildMatrix();    // build the adjacency matrix according to the edges and nodes.

	void calcDeep();       // calculate the depth of each basic block.

	// main function
	void analyzeBinary();

	bool inferType(BasicBlock* block);

};