// varinfer.cpp
// 
// infer the variable from memory. 
//
//

#include "varinfer.h"
#include <math.h>
#include <stdio.h>
#include <iterator>


VariableHunter::VariableHunter()
{
    regPtr = new Register();
    TrackState = new StackTrailer(regPtr);

    stack_variable.clear();
    heap_variable.clear();
    absolute_variable.clear();
    temp_variable.clear();
}

VariableHunter::~VariableHunter()
{

    ASSERT(regPtr != NULL);
    delete regPtr;

    ASSERT(TrackState != NULL);
    delete TrackState;
}


void
VariableHunter::findVariable(xed_decoded_inst_t &xedd)
{

    // currently, we only looking for stack variables
    ASSERT(TrackState != NULL);

    findStackVariable(xedd);

    findHeapVariable(xedd);

    findAbsoluteVariable(xedd);
}



void
VariableHunter::findStackVariable(xed_decoded_inst_t &xedd)
{

    //unsigned int operandNum = xed_decoded_inst_noperands(&xedd);
    const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);

    bool trackanswer;
    trackanswer = TrackState -> trackESP(xedd);
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
    		AbstractVariable *var = new AbstractVariable();

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
    		AbstractVariable *var = new AbstractVariable();

    		var -> region = Stack;
    		var -> offset = disp;

            // we can assign a size here, if the later calculation change this size, everything is OK
            // if this variable happen to be a boundary variable, the size of this instruction will be the size of the variable
            var -> size = xed_operand_values_get_memory_operand_length(operandPtr, 0); 
            // the unit is byte. 

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
VariableHunter::findAbsoluteVariable(xed_decoded_inst_t &xedd)
{
    const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);

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
    xed_reg_enum_t  index_reg = xed_operand_values_get_index_reg(operandPtr, 0);

    
    if(base_reg == XED_REG_INVALID && index_reg == XED_REG_INVALID)
    {
        if(absolute_variable.find(disp) == absolute_variable.end())
        {
            AbstractVariable *var = new AbstractVariable();

            var -> region = Absolute;
            var -> offset = disp;

            // we can assign a size here, if the later calculation change this size, everything is OK
            // if this variable happen to be a boundary variable, the size of instruction will be the size of variable.
            var -> size = xed_operand_values_get_memory_operand_length(operandPtr, 0);   

            absolute_variable.insert(std::make_pair<int, AbstractVariable*>(disp, var));

        }
    }

    return;
}


void 
VariableHunter::getResult(std::map<int, AbstractVariable*> &container)
{

	std::map<int, AbstractVariable*>::iterator ptr, tmp;
    int var_index = 0;


    // insert stack variable. 
    // caculate the size of variables first.
    for(ptr = stack_variable.begin(); ptr != stack_variable.end(); ptr++)
    {
        tmp = stack_variable.upper_bound(ptr -> first);
        if(tmp != stack_variable.end())
        {
        	int size = (tmp -> second) -> offset - (ptr -> second) -> offset;
        	(ptr -> second) -> size = size;
        }
    }
    ptr--; // the last variable in the stack has size of the offset.
    ptr -> second -> size = fabs(ptr -> second -> offset);


    // put the variables in the container
    for(ptr = stack_variable.begin(); ptr != stack_variable.end(); ptr++)
    {
        var_index++; 
    	container.insert(std::make_pair<int, AbstractVariable*>(var_index, ptr -> second));
    }


    // insert abosulte variable. 
    for(ptr = absolute_variable.begin(); ptr != absolute_variable.end(); ptr++)
    {
        tmp = absolute_variable.upper_bound(ptr -> first);
        if(tmp != absolute_variable.end())
        {
            int size = (tmp -> second) -> offset - (ptr -> second) -> offset;
            (ptr -> second) -> size = size;
        }
    }

    for(ptr = absolute_variable.begin(); ptr != absolute_variable.end(); ptr++)
    {
        var_index++;
        container.insert(std::make_pair<int, AbstractVariable*>(var_index, ptr -> second));
    }

}
