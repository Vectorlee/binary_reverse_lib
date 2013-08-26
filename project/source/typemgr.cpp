

#include "typemgr.h"
#include <queue>
#include <memory.h>
extern "C"
{
    #include "xed-interface.h"
}


int MAX = 0x1000000;

TypeManager::TypeManager(TypeHunter* hunter, BYTE* start, BYTE* end)
{
	startAddr = start;
	endAddr = end;

	matrix = NULL;
	blockNum = 0;

    typeHunter = hunter;  // we pass the pointer to the class

	blockContainer.clear();
}

TypeManager::~TypeManager()
{

	ASSERT(blockNum != 0);
	for(int i = 0; i < blockNum; i++)
	{
		delete [] matrix[i];
	}

	for(int i = 0; i < blockNum; i++)
	{
		BasicBlock* p = blockContainer[i];
		delete p;
	}

}


bool 
TypeManager::buildCFG()
{

	static const xed_state_t dstate = { XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b };
    const unsigned int maxInstructionLength = 15;
    
    xed_decoded_inst_t xedd;
    int instLength, index;
	
	std::set<int> jmpTarget;
    std::set<int> instruction;


    BYTE *currentAddr = startAddr;
    nodes.insert(0);

    index = 0;
    while(currentAddr < endAddr)
    {	
    	instruction.insert(index);

        xed_decoded_inst_zero_set_mode(&xedd, &dstate);
	    xed_error_enum_t xedCode = xed_decode(&xedd, (uint8_t*)currentAddr, maxInstructionLength);
		
	    if(xedCode == XED_ERROR_NONE)
	    {
            instLength = xed_decoded_inst_get_length(&xedd);   //get the length of the instruction in byte

            xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);
            switch(iclass){

            	case XED_ICLASS_JMP:
            	{
            		const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);
            		int target = xed_operand_values_get_branch_displacement_int32(operandPtr);

            		target = index + target + instLength;
            		jmpTarget.insert(target);

            		edges.insert(std::make_pair(index, target));
                    nodes.insert(target); 
            		break;
            	}

            	case XED_ICLASS_JB:     
                case XED_ICLASS_JBE:
                case XED_ICLASS_JNB:    
                case XED_ICLASS_JNBE:  
                case XED_ICLASS_JL:    
                case XED_ICLASS_JLE:  
                case XED_ICLASS_JNL:    
                case XED_ICLASS_JNLE:
                case XED_ICLASS_JNS:    
                case XED_ICLASS_JS:
                case XED_ICLASS_JNO:    
                case XED_ICLASS_JNP:    
                case XED_ICLASS_JNZ:    
                case XED_ICLASS_JO:    
                case XED_ICLASS_JP:    
                case XED_ICLASS_JZ:

                case XED_ICLASS_LOOP:
                case XED_ICLASS_LOOPE:  
                case XED_ICLASS_LOOPNE: 
                {
            		const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);
            		int target = xed_operand_values_get_branch_displacement_int32(operandPtr);
            		
            		target = index + target + instLength;
            		jmpTarget.insert(target);
            		
            		edges.insert(std::make_pair(index, target));  // to the target
            		edges.insert(std::make_pair(index, index + instLength));  // to the next instruction

                    nodes.insert(target);
                    nodes.insert(index + instLength);
            		break;
            	}
            	default: break;
            }

            index += instLength;   // index here emulate the eip
	        currentAddr += instLength;
        }
        else
            return false;
    }


    std::set<int>::iterator ptr;
    for(ptr = instruction.begin(); ptr != instruction.end(); ptr++)
    {

    	index = (*ptr);
        if(jmpTarget.find(index) != jmpTarget.end())
        {
           	std::set<int>::iterator itr = ptr;
           	int previous = *(--itr);

           	xed_decoded_inst_zero_set_mode(&xedd, &dstate);
           	xed_error_enum_t decode = xed_decode(&xedd, (uint8_t*)(startAddr + previous), maxInstructionLength);
           	if(decode == XED_ERROR_NONE)
           	{
           		xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);
                // there is a flow that can reach this.
           		if(iclass != XED_ICLASS_RET_FAR && iclass != XED_ICLASS_RET_NEAR && iclass != XED_ICLASS_JMP)
           		{
           			edges.insert(std::make_pair(previous, index));
           		}
           	}
           	else{
           		return false;
           	}
        }
    }

    return true;
}


void 
TypeManager::buildMatrix()
{

	// blockNum is the number of nodes in the function..
    blockNum = nodes.size();    
	nodes.insert(MAX);         // insert an very big number

	// initinal the adjacency matrix
	// the order of nodes in the matrix is based on their location in sequence binary
	matrix = new int* [blockNum];
	for(int i = 0; i < blockNum; i++)
	{
		matrix[i] = new int [blockNum];
		memset(matrix[i], 0, sizeof(int) * blockNum); // clear it to zero.
	}


	std::multimap<int, int>::iterator ptr;
	std::set<int>::iterator itr1;
	std::set<int>::iterator itr2;	

	for(ptr = edges.begin(); ptr != edges.end(); ptr++)
	{
		int from = ptr -> first;    // for an edge.
		int to = ptr -> second;     // from, to 
		int i = 0, j = 0;

		itr1 = itr2 = nodes.begin();
		itr2++;
		while(itr2 != nodes.end())
		{	
			int top  = (*itr1);
			int down = (*itr2);  
			if(from >= top and from < down)
				break;

			i++;
			itr1++; itr2++;   // both iterator move down. 
		}

		itr1 = itr2 = nodes.begin();
		itr2++;
		while(itr2 != nodes.end())
		{	
			int top  = (*itr1);
			if(to == top)
				break;

			j++;
			itr1++; itr2++;   // both iterator move down. 
		}

		matrix[i][j] = 1;
	}

	// build the blockContainer
	int index = 0;
	itr1 = itr2 = nodes.begin();
	itr2++;

	while(itr2 != nodes.end())
	{	
		int top  = (*itr1);
		int down = (*itr2);  // the next node.

		BasicBlock *block = new BasicBlock(index, top, down);
    	blockContainer.push_back(block); // insert the block in the vector, for easy access.
		
        index++;
		itr1++; itr2++;   // both iterator move down. 
	}

	ASSERT((int)blockContainer.size() == blockNum);
	ASSERT(index == blockNum);
	// we need to change the end address of the last block
	blockContainer[blockNum - 1] -> endAddr = (this -> endAddr) - (this -> startAddr);

	return;
}


// we have matrix, we have blockContainer.
// want we need next.
// calculate depth of each block, get a order of different blocks.

void 
TypeManager::calcDeep()
{

	ASSERT(matrix != NULL);
	ASSERT(blockNum != 0);

	// the order of node in blockContainer will change after this function.
	// but their attribute index will record their place in the adjacency matrix.
    std::queue<int> pQueue;
    blockContainer[0] -> depth = 0; 
    // the deep of first block is doom to be 0.

    pQueue.push(0);    // the index.
    while(!pQueue.empty())
    {        
        int index = pQueue.front();   // pop the first element
        pQueue.pop();

        int deep = blockContainer[index] -> depth;
        for(int j = 0; j < blockNum; j++)
        {
            if(matrix[index][j] == 1 && blockContainer[j] -> depth == -1)
            {           // the children of this node  // if the depth of the node is unknow.
                blockContainer[j] -> depth =  deep + 1;
                pQueue.push(j);
            }
        }
    }
    // I can't sort the vector blockContainer. // because I put in the pointer of it.
    // so we just keep the order of the basic block simple with the matrix

    //sort(blockContainer.begin(), blockContainer.end());
    return;
}


std::vector<BasicBlock*>::iterator 
TypeManager::findNext(int deep, std::vector<BasicBlock*> &container)
{
	// find one, and delete one outside the class.
	std::vector<BasicBlock*>::iterator ptr;
	for(ptr = container.begin(); ptr != container.end(); ptr++)
    {
        BasicBlock* p = (*ptr);
        if(p -> depth == deep)
            return ptr;
    }

    return container.end();
}


Register* 
TypeManager::mergeEnvironment(int index) // the index is the index of the basic block.
{

    BasicBlock* node = blockContainer[index];
    std::list<BasicBlock*> container;
    // find the father of this basic block.

    for(int i = 0; i < blockNum; i++)
    {
        if(matrix[i][index] == 1)        // it is his father
        {
            BasicBlock* father = blockContainer[i];
            ASSERT(father -> depth != -1);

            if(node -> depth > father -> depth){  // must be bigger than, can't be the same.
                ASSERT(father -> regPtr != NULL);
                container.insert(container.end(), father);
            }
        }
    }

    // generate new temporary variable, generate new constraint.
    int size = container.size();
    if(size == 0)
    {
        ASSERT(index == 0);  // no path came in, assert it is the first block. 
        Register* ptr = new Register();  // generate a new register class.
        return ptr;

    }
    else if(size == 1)
    {
        Register* ptr  = new Register();
        Register* last = (container.front()) -> regPtr;

        ptr -> copyEnvironment(last);   // we must generate a new register and copy the environment
        return ptr; 

    }
    else{  // more than two branch merge here, we need to merge the envrionment.  

        ASSERT(false);  // for our current case, there doesn't exist such case.
        return NULL;    // TODO, to merge the branch
    }

    return NULL;
}


// now we have the order of different block, we need to move in to get the type constraint.
void 
TypeManager::analyzeBinary()
{
    
	ASSERT(blockNum != 0);
	ASSERT(blockContainer[0] -> depth != -1);

    // copy one of the block container.
    std::vector<BasicBlock*> tmp_container = std::vector<BasicBlock*>(blockContainer);

    int deep = 0;
    while(!tmp_container.empty())
    {
        std::vector<BasicBlock*>::iterator ptr = findNext(deep, tmp_container);
        if(ptr == tmp_container.end())
        {
            deep++;
            continue;
        }

        BasicBlock *block = (*ptr);
        tmp_container.erase(ptr);
        ASSERT(block -> regPtr == NULL);


        int index = block -> id;
        Register* reg = mergeEnvironment(index);

        block -> regPtr = reg;
        inferType(block);

    }

}


bool
TypeManager::inferType(BasicBlock* block)
{

    const unsigned int maxInstructionLength = 15;
    static const xed_state_t dstate = { XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b };

    xed_decoded_inst_t xedd;
    int instLength;
    char buffer[1024];

    // set the start and end address of analysis
    BYTE *current = (this -> startAddr) + (block -> startAddr);
    BYTE *end = (this -> startAddr) + (block -> endAddr);

    // set the environment of the type hunter
    typeHunter -> resetState(block -> regPtr);

    while(current < end)
    {
        memset(buffer, 0, sizeof(buffer));
        xed_decoded_inst_zero_set_mode(&xedd, &dstate);
        xed_error_enum_t xedCode = xed_decode(&xedd, (uint8_t*)current, maxInstructionLength);
        
        if(xedCode == XED_ERROR_NONE)
        {
            instLength = xed_decoded_inst_get_length(&xedd);   //get the length of the instruction in byte
            typeHunter -> assignConstraint(xedd);

            xed_uint64_t runtime_address = (xed_uint64_t)current;
            xed_decoded_inst_dump_intel_format(&xedd, buffer, 1024, runtime_address);
            printf("%s\n", buffer);


            current += instLength;
        }
        else
            return false;
    }
    printf("\n\n");

    return true;
}