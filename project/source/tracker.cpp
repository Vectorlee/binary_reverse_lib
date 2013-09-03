// Tracker.cpp
//
//
//
//

#include "tracker.h"
#include "extract.h"
#include <stdio.h>


Tracker::Tracker(std::map<int, AbstractVariable*> &container, Register* reg)
{

	varIndex = 7;
	regPtr = reg; 

	stack_variable.clear();
	heap_variable.clear();
	absolute_variable.clear();

    std::map<int, AbstractVariable*>::iterator itr;
	for(itr = container.begin(); itr != container.end(); itr++)
	{
		// the id is assigned for these abstract variable
		(itr -> second) -> id = varIndex;
		varIndex++;

		if( (itr -> second) -> region == Stack ){
			stack_variable.insert(std::make_pair((itr -> second) -> offset, itr -> second));
		}
        else if( (itr -> second) -> region == Absolute ){
            absolute_variable.insert(std::make_pair((itr -> second) -> offset, itr -> second));
        }

	}
	varIndex += 20;   // here we plus 20, then the temporary variable will have much different id number.

	flagInfo.operandVar1 = NULL;
	flagInfo.operandVar2 = NULL;
	flagInfo.calcResult  = NULL;
	flagInfo.size = 0;

	temp_variable.clear();
}


Tracker::~Tracker()
{
	//delete regPtr;
	
	stack_variable.clear();
	heap_variable.clear();
	absolute_variable.clear();

	//emptyTrash();
	// here we don't delete the variable, because the variable will be moved out.
	// outer program will use variables.
}


AbstractVariable*
Tracker::getMemoryVariable(xed_decoded_inst_t &xedd, std::list<Constraint*> &typeContainer) // std::map<int, AbstractVariable*> &container)
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
        // IMPORTANT
        // here we have a register pointer that is shared with StackTrailer 
        // StackTrailer will figure out the distance between ESP and EBP. 
        // we get access to them directly

        if(base_reg == XED_REG_ESP)
            disp = disp - (regPtr -> getESP());

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
        // a variable that pointed by a common register, we need to generate constraints here.
        // [ base + index * scale + displacement ]
        int size = (int)xed_operand_values_get_memory_operand_length(operandPtr, 0);

        AbstractVariable* tmp = pointerInfer(base_reg, size, typeContainer);
        return tmp;
    }

    return NULL;
}


AbstractVariable*
Tracker::pointerInfer(xed_reg_enum_t base_reg, int size, std::list<Constraint*> &typeContainer)
{

    ASSERT(base_reg != XED_REG_ESP && base_reg != XED_REG_EBP);

    AbstractVariable* var = this -> getRegister(base_reg);   // the variable stored in base register.
    // this must be a pointer.

    if(var != NULL){

        AbstractVariable* tmp = new AbstractVariable();
        tmp -> region = Temporary;
        tmp -> size = size;

        // we need to set temporary variable here, but this time, there is no register to hold this temporary variable.
        // still, we need to get an index for this.
        // ---------------------------------------------------
        tmp -> id = varIndex;
        varIndex++;

        temp_variable.insert(temp_variable.begin(), tmp);
        // ---------------------------------------------------

        // generate new constraints, to indicate that the base register hold an pointer
        // var is a pointer contains the tmp.
        Constraint *Q = ConstraintLEA(tmp, var);           // generate a pointer type, with the contain as the result.
        typeContainer.insert(typeContainer.end(), Q);

        return tmp;  
    }

    return NULL;
}

void
Tracker::setTempVariable(xed_reg_enum_t dest_reg, AbstractVariable* value)
{

	ASSERT(value -> region == Temporary);
	ASSERT(value -> id == 0);

	value -> id = varIndex;    //assign an id for the temporary variable.
	varIndex++;

	temp_variable.insert(temp_variable.begin(), value);

	// we approach it in a oppositie way: just check whether the coming variable
	// is temporary, if it is, insert it to temp_variable.
	regPtr -> setRegister(dest_reg, value); 
}


void 
Tracker::setFlagInfo(int size, AbstractVariable* var1, AbstractVariable* var2, AbstractVariable* result)
{
    regPtr -> setFlagInfo(size, var1, var2, result);
    return;
}


FlagFactor 
Tracker::getFlagInfo()
{
	return regPtr -> getFlagInfo();
}

//--------------------------------------------
// Tracker::trackVariable
//
// Track the propagation of variables in different
// registers.
//
// This function need to be more tricky, it will need 
// the data flow analysis.
//--------------------------------------------

bool 
Tracker::trackVariable(xed_decoded_inst_t &xedd, std::list<Constraint*> &typeContainer)
{
	xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);

    xed_reg_enum_t src_reg  = SrcRegister(xedd);
    xed_reg_enum_t dest_reg = DestRegister(xedd);

    if( src_reg == XED_REG_ESP || src_reg == XED_REG_EBP || 
	    dest_reg == XED_REG_ESP || dest_reg == XED_REG_EBP )
		return true;

	bool answer = false;

	switch(iclass)
	{
		// for mov instruction, no temporary variable will be created, we just need to transfer the variable.
		case XED_ICLASS_MOV:
		{
			// reg <- reg
			if(IsRegisterOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
            	AbstractVariable* var = regPtr -> getRegister(src_reg);
				regPtr -> setRegister(dest_reg, var); // use set register, not set temp variable.
				answer = true;
            }
            // reg <- mem
            else if(IsRegisterOperand(xedd, 0) && IsMemoryOperand(xedd, 1))   //IsMemoryOperand
            {
                // how to extract the proper variable. make it a
                AbstractVariable* var = getMemoryVariable(xedd, typeContainer);
                regPtr -> setRegister(dest_reg, var);
                answer = true;
            }
            // reg <- imm
            else if(IsRegisterOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {
            	// create a temporary variable.
            	AbstractVariable* temp = new AbstractVariable();
            	temp -> region = Temporary;
            	temp -> size = RegisterOperandSize(xedd, 0) / 8; 

            	setTempVariable(dest_reg, temp);
            	answer = true;
            }
            // mem <- reg
            else if(IsMemoryOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
            	// we dont need to do anything here, the register information is unchanged.
            	// we need to generate constraint, the equal constraint.
            	answer = false;
            }
            // mem <- imm
            else if(IsMemoryOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {
            	answer = true;
            }
            else
            {
            	ASSERT(false);
            }
			break;
		}
		case XED_ICLASS_XOR:
		{
			if(IsRegisterOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
			{
				if(src_reg == dest_reg && src_reg != XED_REG_INVALID){         // XOR EAX, EAX
					regPtr -> setRegister(src_reg, NULL);
					answer = true;
				}
			}
			break;
		}
		default: answer = false; break;
	}

	return answer;
}


void 
Tracker::getResult(std::map<int, AbstractVariable*> &container)
{
	//std::map<int, AbstractVariable*> stack_variable;
	//std::map<int, AbstractVariable*> heap_variable;
	//std::map<int, AbstractVariable*> absolute_variable;

	//std::list<AbstractVariable*> temp_variable;
	ASSERT(container.empty());

	std::map<int, AbstractVariable*>::iterator ptr;
	for(ptr = stack_variable.begin(); ptr != stack_variable.end(); ptr++){
		AbstractVariable* tmp = ptr -> second;
		container.insert(std::make_pair(tmp -> id, tmp));  // this time, the id is the index
	}

	
	for(ptr = absolute_variable.begin(); ptr != absolute_variable.end(); ptr++){
		AbstractVariable* tmp = ptr -> second;
		container.insert(std::make_pair(tmp -> id, tmp));  // this time, the id is the index
	}
	

	std::list<AbstractVariable*>::iterator itr;
	for(itr = temp_variable.begin(); itr != temp_variable.end(); itr++){
		AbstractVariable* tmp = (*itr);
		container.insert(std::make_pair(tmp -> id, tmp));
	}

}


void 
Tracker::resetEnvironment(Register *reg_file)
{
    // set the register class to a new one
    regPtr = reg_file;

    // so we only need to assign the value of register, 
    // and we can then change the environment.
    return;
}

