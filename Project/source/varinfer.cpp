// varinfer.cpp
// 
// infer the variable from memory. 
//
//



#include "varinfer.h"
#include "utility.h"
#include <math.h>
#include <stdio.h>
#include <iterator>


void
VariableHunter::initState()
{
	if(RegPtr != NULL)
		delete RegPtr;

	if(TrackState != NULL)
		delete TrackState;

	RegPtr = new Register();
	TrackState = new Tracker(RegPtr);


	stack_variable.clear();
	heap_variable.clear();
	abosulte_variable.clear();

}


void
VariableHunter::findVariable(xed_decoded_inst_t &xedd)
{

    // currently, we only looking for stack variables
    findStackVariable(xedd);

    findHeapVariable(xedd);

    findAbosulteVariable(xedd);
}



void
VariableHunter::findStackVariable(xed_decoded_inst_t &xedd)
{

    //unsigned int operandNum = xed_decoded_inst_noperands(&xedd);
    const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);

    bool trackanswer = TrackState -> trackESP(xedd);
    ASSERT(trackanswer);

    // we need to extract the variable from memory. 
    xed_bool_t mark = xed_operand_values_accesses_memory(operandPtr);
    if(!mark){
        return;
    }


    // we ignore the push or pop instruction
    xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);

    if( iclass == XED_ICLASS_PUSH    || iclass  == XED_ICLASS_POP     || 
        iclass == XED_ICLASS_ENTER   || iclass == XED_ICLASS_LEAVE    ||
        iclass == XED_ICLASS_RET_FAR || iclass == XED_ICLASS_RET_NEAR )
    {
        return;
    }


    xed_int32_t          disp = (xed_int32_t)xed_operand_values_get_memory_displacement_int64(operandPtr);
    xed_reg_enum_t   base_reg = xed_operand_values_get_base_reg(operandPtr, 0);
    //xed_reg_enum_t  index_reg = xed_operand_values_get_index_reg(operandPtr, 0);
    //unsigned int       length = xed_operand_values_get_memory_operand_length(operandPtr, 0);
    //unsigned int        scale = xed_operand_values_get_scale(operandPtr);
    //xed_reg_enum_t    seg_reg = xed_operand_values_get_seg_reg(operandPtr, 0); //segment reg
    

    //EBP based memory access 
    if(base_reg == XED_REG_EBP)//&& index_reg == XED_REG_INVALID)
    {
    	if(stack_variable.find(disp) == stack_variable.end())
    	{
    		AbstractVariable *var = new AbstractVariable;

    		var -> region = Stack;
    		var -> offset = disp;

    		stack_variable.insert(std::make_pair<int, AbstractVariable*>(disp, var));

    	}
    }


    //ESP based memory access 
    if(base_reg == XED_REG_ESP)// && index_reg == XED_REG_INVALID)
    {
    	int distance = TrackState -> getESP();
    	disp = disp - distance;
    	// compute the ebp based offset

    	if(stack_variable.find(disp) == stack_variable.end())
    	{
    		AbstractVariable *var = new AbstractVariable;

    		var -> region = Stack;
    		var -> offset = disp;

    		stack_variable.insert(std::make_pair<int, AbstractVariable*>(disp, var));
    	}
    }

    return;
}


void 
VariableHunter::findHeapVariable(xed_decoded_inst_t &xedd)
{
	//TODO
}

void 
VariableHunter::findAbosulteVariable(xed_decoded_inst_t &xedd)
{
	//TODO
}


void 
VariableHunter::getResult(std::map<int, AbstractVariable*> &container)
{
	// caculate the size of variables first.

	std::map<int, AbstractVariable*>::iterator ptr, tmp;
    for(ptr = stack_variable.begin(); ptr != stack_variable.end(); ptr++)
    {
        //printf("variable %d: ebp%d\n", i, ptr -> first);
        tmp = ptr, tmp++;
        if(tmp != stack_variable.end())
        {
        	int size = (tmp -> second) -> offset - (ptr -> second) -> offset;
        	(ptr -> second) -> size = size;
        }
    }


    // ensure that the if the map is not empty 
    if(!stack_variable.empty()){

        ptr = stack_variable.end(), ptr--;
        (ptr -> second) -> size = fabs((ptr -> second) -> offset);
    }
    // the size of this variable is unknown. 
    // we assume that this variable have a size equals to its offset.


    // put the variables in the container
    for(ptr = stack_variable.begin(); ptr != stack_variable.end(); ptr++)
    {
    	container.insert(*ptr);
    }


    // TODO
    // insert variables found in other places into the container.

}
