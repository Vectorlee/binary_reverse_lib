#include <string.h>
#include <stdio.h>
#include <map>

extern "C"
{
    #include "xed-interface.h"
}

//  analyseProcedure:
//       analyse a procedure staticaly, check each instrcution from 
//       the frist to the last of the procedure, 
//   
//       startaddress:  the start address of a procedure
//       endAddr:    the end address of a procedure
//


//register number:
//    eax: 52
//    ecx: 53
//    edx: 54
//    ebx: 55
//    esp: 56
//    ebp: 57
//
//    cs:  139
//    ds:  140
//    es:  141
//    ss:  142


// the key is the displacement of the variable from ebp
std::map<xed_int64_t, int> variable_map;


//we need to trace change of esp;
// TODO


void  CheckOperand(xed_decoded_inst_t &xedd)
{

    char buffer[0x100];

    unsigned int operandNum = xed_decoded_inst_noperands(&xedd);
    //printf("Operand Number: %d\n", operandNum);
    const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);

    xed_bool_t mark = xed_operand_values_accesses_memory(operandPtr);

    if(!mark)
    {
        //printf("haven't access memory\n");
        return;
    }
    //xed_operand_values_dump(operandPtr, buffer, 0x100);
    //printf("Operand: %s\n", buffer);

      
    // XED_CATEGORY_POP    
    // XED_CATEGORY_PUSH 
    // we ignore the push or pop instruction
    xed_category_enum_t category = xed_decoded_inst_get_category(&xedd);
    if(category == XED_CATEGORY_PUSH || category  == XED_CATEGORY_POP)
    {
        return;
    }


    xed_int64_t          disp = xed_operand_values_get_memory_displacement_int64(operandPtr);
    xed_reg_enum_t   base_reg = xed_operand_values_get_base_reg(operandPtr, 0);
    xed_reg_enum_t  index_reg = xed_operand_values_get_index_reg(operandPtr, 0);
    unsigned int       length = xed_operand_values_get_memory_operand_length(operandPtr, 0);
    unsigned int        scale = xed_operand_values_get_scale(operandPtr);
    xed_reg_enum_t    seg_reg = xed_operand_values_get_seg_reg(operandPtr, 0); //segment reg


    //XED_REG_ESP     
    //XED_REG_EBP 
    if(base_reg == XED_REG_EBP && index_reg == XED_REG_INVALID)
    {
       variable_map[disp] = 1;
    }

    //printf("      base reg:  %d\n", base_reg);
    //printf("     index reg:  %d\n", index_reg);
    //printf("operand length:  %d\n", length);
    //printf("         scale:  %d\n", scale);
    //printf("   segment reg:  %d\n", seg_reg);
    //printf("  displacement:  %d\n", disp);
    //printf("\n");

    return;
}


bool analyseProcedure(void *startaddress, void *endAddr)
{

    const unsigned int maxInstructionLength = 15;   // the max length of a x86 instruction is 15byte
    xed_decoded_inst_t xedd;
    int instLength, index;
	
    void *currentAddr = startaddress;
    char buffer[1024];

    static const xed_state_t dstate = { XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b };
    //currently, we only implement the 32bit machine

    index = 0;
    while(currentAddr < endAddr)
    {
	    memset(buffer, 0, sizeof(buffer));
	
        xed_decoded_inst_zero_set_mode(&xedd, &dstate);
	    xed_error_enum_t xedCode = xed_decode(&xedd, (uint8_t*)currentAddr, maxInstructionLength);
        //we need to get the length of the instruction
		
	    if(xedCode == XED_ERROR_NONE)
	    {
            instLength = xed_decoded_inst_get_length(&xedd);   //get the length of the instruction in byte

            xed_uint64_t runtime_address = (xed_uint64_t)currentAddr;
            xed_decoded_inst_dump_intel_format(&xedd, buffer, 1024, runtime_address);
            printf("0x%x\t\t%s\n", index, buffer);

            CheckOperand(xedd);
            //printf("%d\n", instLength);

	        currentAddr += instLength;
            index += instLength;

        }
        else
            return false;
    }

    return true;
}



// we put the byte code in a char string.
// and then analysis this string.
unsigned char code[] = "\x55\x89\xe5\x83\xec\x70\xc7\x45\xfc\x0f\x84\x01\x00\xc7\x45\xf8\xe3\x2b\x00\x00\x8b\x45\xf8\x8b\x55\xfc\x8d\x04\x02\x89\x45\xf4\x8b\x45\xfc\x01\xc0\x89\x45\xf0\xc7\x45\xec\x00\x00\x00\x00\xeb\x14\x8b\x45\xec\x8b\x55\xf0\x8b\x4d\xec\x8d\x14\x11\x89\x54\x85\x9c\x83\x45\xec\x01\x83\x7d\xec\x13\x0f\x9e\xc0\x84\xc0\x75\xe1\xb8\x00\x00\x00\x00\xc9\xc3";
//unsigned char code[] = "\x55\x89\xe5\x83\xec\x70\xc7\x45\xfc\x0f\x84\x01\x00\xc7\x45\xf8\xe3\x2b\x00\x00\x8b\x45\xf8\x8b\x55\xfc\x8d\x04\x02\x89\x45\xf4\x8b\x45\xfc\x01\xc0\x89\x45\xf0\xc7\x45\xec\x00\x00\x00\x00\xeb\x1f\x8b\x4d\xec\x8b\x45\xf8\x8b\x55\xfc\x8d\x04\x02\x03\x45\xec\x89\xc2\xc1\xfa\x1f\xf7\x7d\xf4\x89\x44\x8d\x9c\x83\x45\xec\x01\x83\x7d\xec\x13\x0f\x9e\xc0\x84\xc0\x75\xd6\xb8\x00\x00\x00\x00\xc9\xc3";


int main()
{

    int i;
    xed_tables_init();

    //int length = strlen(code);
    analyseProcedure((void*)code, (void*)(code + 92));
    printf("\n\nThe Variables:\n");


    std::map<xed_int64_t, int>::iterator ptr;
    for(i = 0, ptr = variable_map.begin(); ptr != variable_map.end(); ptr++, i++)
    {
        printf("variable %d: ebp%d\n", i, ptr -> first);
    }

    return 0;
}
