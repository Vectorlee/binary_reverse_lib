//Tracker.cpp
//
//
//
//

#include "tracker.h"
#include <stdio.h>

bool 
Tracker::trackESP(xed_decoded_inst_t &xedd)
{
	//xed_category_enum_t category = xed_decoded_inst_get_category(&xedd);
	xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);  //get the class of the instructions

	xed_reg_enum_t src_reg  = SrcRegister(xedd);
	xed_reg_enum_t dest_reg = DestRegister(xedd);
	
	switch(iclass)
	{
		case XED_ICLASS_PUSH: regPtr -> addESP(1);  break;    // we only deal with push, pop, add, sub, enter, leave, mov. ret
		case XED_ICLASS_POP:  regPtr -> subESP(1);  break;    // the operation that can affect the value of esp is just them.
		case XED_ICLASS_MOV:                                 //  
		{
			if(dest_reg == XED_REG_EBP)
			{
				if(src_reg == XED_REG_ESP){
					regPtr -> setESP();
				}
				else
					return false;
			}
			if(dest_reg == XED_REG_ESP)
				return false;

			break;
		}
		case XED_ICLASS_ADD:
		{
			int mark = 0;
			if(dest_reg == XED_REG_ESP)
			{
				xed_int64_t imm = GetImmediate(xedd, mark);
				if(mark == 0)
					return false;
				else{
					regPtr -> subESP((int)imm);    // add is sub in our case
				}
			}

			break;
		}
		case XED_ICLASS_SUB:
		{
			int mark = 0;
			if(dest_reg == XED_REG_ESP)
			{
				xed_int64_t imm = GetImmediate(xedd, mark);
				if(mark == 0)
					return false;
				else{
					regPtr -> addESP((int)imm);    // add is sub in our case
				}
			}

			break;
		}
		case XED_ICLASS_ENTER:
		case XED_ICLASS_LEAVE:
		{
			regPtr -> setESP();
			break;
		}

		default: break;
	}

	return true;
}



//--------------------------------------------
// Tracker::trackVariable
//
// Track the propagation of variables in different
// registers.
//--------------------------------------------

bool
Tracker::trackVariable(xed_decoded_inst_t &xedd)
{

	xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);

	xed_reg_enum_t src_reg  = SrcRegister(xedd);
	xed_reg_enum_t dest_reg = DestRegister(xedd);

	if( src_reg == XED_REG_ESP || src_reg == XED_REG_EBP || 
	   dest_reg == XED_REG_ESP || dest_reg == XED_REG_EBP )
		return true;


    switch(iclass)
	{
		// mov
		// reg to reg
		// memory to reg
		// immidiate to reg

		// reg to memory
		// immidiate to memory
		case XED_ICLASS_MOV:               
		{
			if(IsRegisterOperand(xedd, 0))  // the destination is a reg.
			{
				if(IsRegisterOperand(xedd, 1))  // reg to reg
				{
					
					AbstractVariable* var = regPtr -> getRegister(src_reg);
					regPtr -> setRegister(dest_reg, var);

					//printf("src: %d with value: %d, to dest: %d\n", src_reg, var, dest_reg);

					return true;
				}
				else if(IsMemoryOperand(xedd, 1))          // memory to reg 
				{
					return false;                  // we shou know the number of the variable.
				}
				else if(IsImmediateOperand(xedd, 1))      // imm to reg
				{
					regPtr -> setRegister(dest_reg, NULL);
					return true;
					// TODO temporary variable;
				}
				else{
					ASSERT(false);
				}
			}
			else if(IsMemoryOperand(xedd, 0))  // the destination is memory.
			{
				// the contain of the register is remain same.
				// the contain of the register should be parallel with memory cell.
				if(IsRegisterOperand(xedd, 1)){
					return false;
				}
				else if(IsImmediateOperand(xedd, 1)){
					return true;
				}
				else{
					ASSERT(false);
				}
			}
			else
			{
				ASSERT(false);
			}
			
			break;
		}

		case XED_ICLASS_XCHG:
		{
			// TODO
			// AbstractVariable* var1 = 
			break;
		}
		case XED_ICLASS_LEA:
		{
			// TODO
			break;
		}
		case XED_ICLASS_IMUL:
		case XED_ICLASS_MUL:
		{
			int size = RegisterOperandSize(xedd, 0);
			AbstractVariable* var = regPtr -> getRegister(dest_reg);

			if(size == 8){
				regPtr -> setRegister(XED_REG_AX, var);
			}
			else if(size == 16){
				regPtr -> setRegister(XED_REG_EAX, var);
			}
			else if(size == 32){
				regPtr -> setRegister(XED_REG_EAX, var);
				regPtr -> setRegister(XED_REG_EDX, var);
			}
			else{
				ASSERT(false);
			}
			break;
		}
		case XED_ICLASS_XOR:
		{
			if(IsRegisterOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
			{
				if(src_reg == dest_reg){         // XOR EAX, EAX
					regPtr -> setRegister(src_reg, NULL);
				}
			}
			return true;
		}
		default: break;
	}

	return true;

}


bool
Tracker::fixTracking(xed_decoded_inst_t &xedd, AbstractVariable* value)
{
	xed_reg_enum_t dest_reg  = DestRegister(xedd);
	xed_reg_enum_t src_reg   = SrcRegister(xedd);

	if(dest_reg == XED_REG_INVALID && src_reg != XED_REG_INVALID)
	{
		regPtr -> setRegister(src_reg, value);
	}
	else if(dest_reg != XED_REG_INVALID && src_reg == XED_REG_INVALID)
	{
		regPtr -> setRegister(dest_reg, value);
	}
	else
		ASSERT(false);
	//printf("memory with value: %d, to dest: %s\n", value, xed_reg_enum_t2str(dest_reg));

	return true;
}

