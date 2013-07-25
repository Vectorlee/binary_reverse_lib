

#include "solver.h"
#include <map>
#include <stdio.h>


void
TypeSolver::initState(std::map<int, AbstractVariable*> &container)
{
	//if(RegPtr != NULL)
	//	delete RegPtr;

	if(TrackState != NULL)
		delete TrackState;

	//RegPtr = new Register();
	TrackState = new Tracker();


	stack_variable.clear();
	heap_variable.clear();
	absolute_variable.clear();
    temp_variable.clear();


    std::map<int, AbstractVariable*>::iterator itr;
	for(itr = container.begin(); itr != container.end(); itr++)
	{
		if( (itr -> second) -> region == Stack ){
			stack_variable.insert(std::make_pair((itr -> second) -> offset, itr -> second));
		}
        else if( (itr -> second) -> region == Absolute ){
            absolute_variable.insert(std::make_pair((itr -> second) -> offset, itr -> second));
        }

	}

	// TODO
	// assign other variables map.
}


AbstractVariable*
TypeSolver::getVariableDescriptor(xed_decoded_inst_t &xedd) // std::map<int, AbstractVariable*> &container)
{

    xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);
    const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);

	if( iclass == XED_ICLASS_PUSH    || iclass  == XED_ICLASS_POP     || 
        iclass == XED_ICLASS_ENTER   || iclass == XED_ICLASS_LEAVE    ||
        iclass == XED_ICLASS_RET_FAR || iclass == XED_ICLASS_RET_NEAR )
	{
		ASSERT(false);
	}

    xed_bool_t mark = xed_operand_values_accesses_memory(operandPtr);
    if(!mark){
        ASSERT(false);
    }

    // find the memory operation displacement.
	xed_int32_t          disp = (xed_int32_t)xed_operand_values_get_memory_displacement_int64(operandPtr);
    xed_reg_enum_t   base_reg = xed_operand_values_get_base_reg(operandPtr, 0);
    xed_reg_enum_t  index_reg = xed_operand_values_get_index_reg(operandPtr, 0);


    // different variable category
    if(base_reg == XED_REG_ESP || base_reg == XED_REG_EBP)
    {
        // stack variable.
        if(base_reg == XED_REG_ESP)
            disp = disp - (TrackState -> getESP());

        if(stack_variable.find(disp) != stack_variable.end())
            return stack_variable[disp];
        else
            return NULL;
    }
    else if(base_reg == XED_REG_INVALID && index_reg == XED_REG_INVALID)
    {
        // global variable

        if(absolute_variable.find(disp) != absolute_variable.end())
            return absolute_variable[disp];
        else
            return NULL;
    }
    else
    {
        // heap variable
        return NULL;
    }

    return NULL;
}


void 
TypeSolver::getResult(std::map<int, AbstractVariable*> &container)
{
    // put the variables in the container
    int var_index = 0;
    std::map<int, AbstractVariable*>::iterator ptr;

    for(ptr = stack_variable.begin(); ptr != stack_variable.end(); ptr++)
    {
        var_index++; 
        container.insert(std::make_pair<int, AbstractVariable*>(var_index, ptr -> second));
    }

    for(ptr = absolute_variable.begin(); ptr != absolute_variable.end(); ptr++)
    {
        var_index++; 
        container.insert(std::make_pair<int, AbstractVariable*>(var_index, ptr -> second));
    }

    // TODO
    // insert variables found in other places into the container.
}


void 
TypeSolver::getConstraint(xed_decoded_inst_t &xedd)
{

	bool trackanswer;
    trackanswer = TrackState -> trackESP(xedd);
    ASSERT(trackanswer);

    trackanswer = TrackState -> trackVariable(xedd);
    if(!trackanswer)
    {
    	AbstractVariable* value = getVariableDescriptor(xedd);
        TrackState -> fixTracking(xedd, value);
    }

    generateConstraint(xedd);
}


void 
TypeSolver::insertConstraint(AbstractVariable* var, int size, int level)
{

    TypeHierarchy byte, word, dword;

    if(level == 0){
        byte = Num8;  word = Num16;  dword = Num32;
    }
    else if(level == 1){
        byte = Uint8; word = Uint16; dword = Uint32;
    }
    else if(level == 2){
        byte = Int8;  word = Int16;  dword = Int32;
    }
    else{
        ASSERT(false);
    }


    if(size == 8){
        (var -> typeinfo).insert((var -> typeinfo).begin(), new Constraint(byte, All) );
    }
    else if(size == 16){
        (var -> typeinfo).insert((var -> typeinfo).begin(), new Constraint(word, All) );
    }
    else if(size == 32){
        (var -> typeinfo).insert((var -> typeinfo).begin(), new Constraint(dword, All) );
    }
    else{
        ASSERT(false);
    }
}


void
TypeSolver::generateConstraint(xed_decoded_inst_t &xedd)
{
	xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);

    xed_reg_enum_t reg1 = DestRegister(xedd);
    xed_reg_enum_t reg2 = DestRegister(xedd);

    if(reg1 == XED_REG_ESP || reg1 == XED_REG_EBP || 
       reg2 == XED_REG_ESP || reg2 == XED_REG_EBP )
        return;


	switch(iclass)
	{
        //case XED_ICLASS_SBB:
        //case XED_ICLASS_ADC:
		
        case XED_ICLASS_ADD:
		case XED_ICLASS_SUB:

        case XED_ICLASS_AND:
        case XED_ICLASS_OR:
        case XED_ICLASS_XOR:

        case XED_ICLASS_CMP:
		{
			// source operand:      reg8, reg16, reg32, imm8, imm16, imm32, mem8, mem16, mem32
            // destination operand: reg8, reg16, reg32,                     mem8, mem16, mem32.

            // reg <- reg
            if(IsRegisterOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {

                xed_reg_enum_t dest_reg = DestRegister(xedd);
            	xed_reg_enum_t src_reg  = SrcRegister(xedd);

            	// if two register operand calculate, the size must be same.
            	int size = RegisterOperandSize(xedd, 0);

            	AbstractVariable* var1 = TrackState -> getRegister(dest_reg);
                AbstractVariable* var2 = TrackState -> getRegister(src_reg);

                if(var1 != NULL)
                {
                    // num  oper  num
                    insertConstraint(var1, size, 0);

                    if(size == 32)
                        (var1 -> typeinfo).insert((var1 -> typeinfo).begin(), new Constraint(Ptr_s, All) );
                }

                if(var2 != NULL)
                {
                    // num  oper  num
                    insertConstraint(var2, size, 0);
                }


            }
            // reg <- mem
            else if(IsRegisterOperand(xedd, 0) && IsMemoryOperand(xedd, 1))   //IsMemoryOperand
            {
                // how to extract the proper variable. make it a
                xed_reg_enum_t dest_reg = DestRegister(xedd);
                int size = RegisterOperandSize(xedd, 0);

                AbstractVariable* var1 = TrackState -> getRegister(dest_reg);
                AbstractVariable* var2 = getVariableDescriptor(xedd);


                if(var1 != NULL)
                {
                    // num  oper  num
                    insertConstraint(var1, size, 0);

                    if(size == 32)
                        (var1 -> typeinfo).insert((var1 -> typeinfo).begin(), new Constraint(Ptr_s, All) );
                }

                if(var2 != NULL)
                {
                    // num  oper  num
                    insertConstraint(var2, size, 0);
                }

            }
            // reg <- imm
            else if(IsRegisterOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {                
                xed_reg_enum_t dest_reg = DestRegister(xedd);
                int size = RegisterOperandSize(xedd, 0);

                AbstractVariable* var1 = TrackState -> getRegister(dest_reg);

                int level = 0;
                if(xed_decoded_inst_get_immediate_is_signed(&xedd))
                    level = 2;        // 0 num, 1 unsigned, 2 signed 
                else
                    level = 1;


                if(var1 != NULL)
                {
                    // num  oper  num
                    insertConstraint(var1, size, level);

                    if(size == 32)
                        (var1 -> typeinfo).insert((var1 -> typeinfo).begin(), new Constraint(Ptr_s, All) );
                }


            }
            // mem <- reg
            else if(IsMemoryOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
                //add mem reg
                xed_reg_enum_t src_reg = SrcRegister(xedd);
                int size = RegisterOperandSize(xedd, 1);

                AbstractVariable* var1 = getVariableDescriptor(xedd);
                AbstractVariable* var2 = TrackState -> getRegister(src_reg);

                if(var1 != NULL)
                {
                    insertConstraint(var1, size, 0);

                    if(size == 32)
                        (var1 -> typeinfo).insert((var1 -> typeinfo).begin(), new Constraint(Ptr_s, All) );
                }
                if(var2 != NULL)
                {
                    insertConstraint(var2, size, 0);
                }
 
            }
            // mem <- imm
            else if(IsMemoryOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {
                AbstractVariable* var1 = getVariableDescriptor(xedd);
                int size = MemoryOperandSize(xedd, 0);


                int level = 0;
                if(xed_decoded_inst_get_immediate_is_signed(&xedd))
                    level = 2;
                else
                    level = 1;


                if(var1 != NULL)
                {
                    // num  oper  num
                    insertConstraint(var1, size, level);

                    if(size == 32)
                        (var1 -> typeinfo).insert((var1 -> typeinfo).begin(), new Constraint(Ptr_s, All) );
                }
            }
            else
            {
            	ASSERT(false);
            }
			break;
		}
        case XED_ICLASS_MUL:
        {
            // idiv mem.
            // EDX:EAX / mem = EAX ... EDX
            xed_reg_enum_t dest_reg = DestRegister(xedd);
            AbstractVariable* var1 = TrackState -> getRegister(dest_reg);
            AbstractVariable* var2 = getVariableDescriptor(xedd);
            
            int size = RegisterOperandSize(xedd, 0);

            if(var1 != NULL){
                insertConstraint(var1, size, 1);
            }

            if(var2 != NULL){
                insertConstraint(var2, size, 1);
            }

            break;
        }
        case XED_ICLASS_DIV:
        {
            // idiv mem.
            // EDX:EAX / mem = EAX ... EDX
            // we don't know which variable should this EDX be ?
            xed_reg_enum_t src_reg = SrcRegister(xedd);

            AbstractVariable* var1 = getVariableDescriptor(xedd);
            AbstractVariable* var2 = TrackState -> getRegister(src_reg);

            int size = RegisterOperandSize(xedd, 1);

            if(var1 != NULL){
                insertConstraint(var1, size, 1);
            }

            if(var2 != NULL){
                insertConstraint(var2, size, 1);
            }

            break;
        }

        case XED_ICLASS_IMUL:
        {
            // idiv mem.
            // EDX:EAX / mem = EAX ... EDX
            xed_reg_enum_t dest_reg = DestRegister(xedd);
            AbstractVariable* var1 = TrackState -> getRegister(dest_reg);
            AbstractVariable* var2 = getVariableDescriptor(xedd); 
            
            int size = RegisterOperandSize(xedd, 0);

            if(var1 != NULL){
                insertConstraint(var1, size, 2);
            }

            if(var2 != NULL){
                insertConstraint(var2, size, 2);
            }

            break;
        }
        case XED_ICLASS_IDIV:
        {
            // idiv mem.
            // EDX:EAX / mem = EAX ... EDX
            // we don't know which variable should this EDX be ?
            xed_reg_enum_t src_reg = SrcRegister(xedd);

            AbstractVariable* var1 = getVariableDescriptor(xedd);
            AbstractVariable* var2 = TrackState -> getRegister(src_reg);

            int size = RegisterOperandSize(xedd, 1);
            
            if(var1 != NULL){
                insertConstraint(var1, size, 2);
            }

            if(var2 != NULL){
                insertConstraint(var2, size, 2);
            }
            break;
        }
        default: break;
	}
    return;

}