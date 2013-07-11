// Utility.cpp
//
//
// implement global functions
//

#include "utility.h"
#include <stdio.h>


xed_reg_enum_t SrcRegister(xed_decoded_inst_t &xedd)
{

    const xed_inst_t* inst = xed_decoded_inst_inst(&xedd);

    unsigned int operandNum = xed_inst_noperands(inst);   // get the number of the operands 

    // if the instruction has no operands, return invalid
    if(operandNum < 2)
        return XED_REG_INVALID;


    const xed_operand_t* operand = xed_inst_operand(inst, 1);  //the first operand is the destination.
    xed_operand_enum_t oper_type = xed_operand_name(operand);

    xed_uint_t mark = xed_operand_is_register(oper_type);
    
    // check whether the operand is a register operand
    if(mark == 1)
    {
        xed_reg_enum_t reg = xed_decoded_inst_get_reg(&xedd, oper_type);
        ASSERT(reg != XED_REG_INVALID);

        return reg;
    }

    return XED_REG_INVALID;

}





// Return the destination register of one instruction
xed_reg_enum_t DestRegister(xed_decoded_inst_t &xedd)
{

    const xed_inst_t* inst = xed_decoded_inst_inst(&xedd);

    unsigned int operandNum = xed_inst_noperands(inst);   // get the number of the operands 

    // if the instruction has no operands, return invalid
    if(operandNum == 0)
        return XED_REG_INVALID;


    const xed_operand_t* operand = xed_inst_operand(inst, 0);  //the first operand is the destination.
    xed_operand_enum_t oper_name = xed_operand_name(operand);

    xed_uint_t mark = xed_operand_is_register(oper_name);
    
    // check whether the operand is a register operand
    if(mark == 1)
    {
        xed_reg_enum_t reg = xed_decoded_inst_get_reg(&xedd, oper_name);
        ASSERT(reg != XED_REG_INVALID);

        return reg;
    }

    return XED_REG_INVALID;
}


xed_int64_t GetImmediate(xed_decoded_inst_t &xedd, int &mark)
{

    const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);

    mark = 0;
    //Return true if there is an immediate operand. 
    if(xed_operand_values_has_immediate(operandPtr))
    {
        //Return true if the first immediate (IMM0) is signed. 
        xed_uint_t sign = xed_operand_values_get_immediate_is_signed(operandPtr);
        if(sign)
        {
            mark = -1;
            return xed_operand_values_get_immediate_int64(operandPtr);
        }
        else
        {
            mark = 1;
            return (xed_int64_t)xed_operand_values_get_immediate_uint64(operandPtr);

        }
    }

    return 0;
}



