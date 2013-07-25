// Utility.cpp
//
//
// implement global functions
//

#include "utility.h"
#include <stdio.h>


const char* TypeToStr(TypeHierarchy type)
{
    switch(type)
    {
        case All:    return "All"; break;
        case Int8:   return "Int8"; break;
        case Uint8:  return "Uint8"; break;
        case Int16:  return "Int16"; break;
        case Uint16: return "Uint16"; break;
        case Int32:  return "Int32"; break;
        case Uint32: return "Uint32"; break;
        case Num8:   return "Num8"; break;
        case Num16:  return "Num16"; break;
        case Num32:  return "Num32"; break;
        case Ptr_s:  return "Ptr_s"; break;
        case Reg8:   return "Reg8"; break;
        case Reg16:  return "Reg16"; break;
        case Reg32:  return "Reg32"; break;
        case Unknown: return "Unknown"; break;
        default: return ""; break;
    }
}



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


bool IsMemoryOperand(xed_decoded_inst_t &xedd, int index)
{
    const xed_inst_t* inst = xed_decoded_inst_inst(&xedd);

    const xed_operand_t* operand = xed_inst_operand(inst, index);  //the first operand is the destination.
    xed_operand_enum_t oper_type = xed_operand_name(operand);


    switch(oper_type)
    {
        case XED_OPERAND_MEM0:
        case XED_OPERAND_MEM1: return true; break;
        default: break; 
    }

    return false;
}


bool IsRegisterOperand(xed_decoded_inst_t &xedd, int index)
{
    const xed_inst_t* inst = xed_decoded_inst_inst(&xedd);

    const xed_operand_t* operand = xed_inst_operand(inst, index);  //the first operand is the destination.
    xed_operand_enum_t oper_type = xed_operand_name(operand);


    switch(oper_type)
    {
        case XED_OPERAND_REG0:
        case XED_OPERAND_REG1:
        case XED_OPERAND_REG2:
        case XED_OPERAND_REG3:
        case XED_OPERAND_REG4:
        case XED_OPERAND_REG5:
        case XED_OPERAND_REG6:
        case XED_OPERAND_REG7:
        case XED_OPERAND_REG8:
        case XED_OPERAND_REG9:
        case XED_OPERAND_REG10:
        case XED_OPERAND_REG11:
        case XED_OPERAND_REG12:
        case XED_OPERAND_REG13:
        case XED_OPERAND_REG14:
        case XED_OPERAND_REG15: return true; break;
        default: break; 
    }

    return false;
}


bool IsImmediateOperand(xed_decoded_inst_t &xedd, int index)
{
    const xed_inst_t* inst = xed_decoded_inst_inst(&xedd);

    const xed_operand_t* operand = xed_inst_operand(inst, index);  //the first operand is the destination.
    xed_operand_enum_t oper_type = xed_operand_name(operand);


    switch(oper_type)
    {
        case XED_OPERAND_IMM0: return true;
        default: break; 
    }

    return false;
}


int RegisterOperandSize(xed_decoded_inst_t &xedd, int index)
{

    ASSERT(IsRegisterOperand(xedd, index));

    const xed_inst_t* inst = xed_decoded_inst_inst(&xedd);
    const xed_operand_t* operand = xed_inst_operand(inst, index);  //the first operand is the destination.
    xed_operand_enum_t oper_type = xed_operand_name(operand);

    xed_reg_enum_t reg = xed_decoded_inst_get_reg(&xedd, oper_type);

    int size = 0;
    switch(reg)
    {
        case XED_REG_AH:
        case XED_REG_AL:
        case XED_REG_BH:
        case XED_REG_BL:
        case XED_REG_CH:
        case XED_REG_CL:
        case XED_REG_DH:
        case XED_REG_DL: size = 8; break;

        case XED_REG_AX:
        case XED_REG_BX:
        case XED_REG_CX:
        case XED_REG_DX: size = 16; break;

        default: size = 32; break;
    }

    ASSERT(size != 0);
    return size;
}

int MemoryOperandSize(xed_decoded_inst_t &xedd, int index)
{
    //size2 = xed_decoded_inst_get_memop_address_width(&xedd, 0);

    ASSERT(IsMemoryOperand(xedd, index));

    const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);
    int size = (int)xed_operand_values_get_memory_operand_length(operandPtr, 0);
    
    // here the memory operation we concerned, is all MEM0
    // currently, I am not very clear what does the MEM1 means.

    return size * 8;
    // here the size is 1, 2, 4;
}


int ImmediateOperandSize(xed_decoded_inst_t &xedd, int index)
{
    ASSERT(IsImmediateOperand(xedd, index));

    int size = xed_decoded_inst_get_immediate_width_bits(&xedd);

    return size;
}