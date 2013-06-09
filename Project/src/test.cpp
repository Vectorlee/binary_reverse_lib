
#include <string.h>
#include <stdio.h>

extern "C"
{
      #include "xed-interface.h"
}

//  analyseProcedure:
//       analyse a procedure staticaly, check each instrcution from 
//       the frist to the last of the procedure, then output the analysis result
//       in a predefined structure
//   
//       startAddr:  the start address of a procedure
//       endAddr:    the end address of a procedure
//       ProcAnalysis: the pointer to a structure where we put our result
//

bool analyseProcedure(void *startAddr, void *endAddr)
{

    const unsigned int maxInstructionLength = 15;   // the max length of a x86 instruction is 15byte
    xed_decoded_inst_t xedd;
    unsigned int instLength;
	
    void *currentAddr = startAddr;
    char buffer[1024];
	
    static const xed_state_t dstate = { XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b };
    //currently, we only implement the 32bit machine

    xed_decoded_inst_zero_set_mode(&xedd, &dstate);
	
    while(currentAddr < endAddr)
    {
	memset(buffer, 0, sizeof(buffer));
	
	xed_error_enum_t xedCode = xed_decode(&xedd, (uint8_t*)currentAddr, maxInstructionLength);
        //we need to get the length of the instruction
		
	if(xedCode == XED_ERROR_NONE)
	{
             instLength = xed_decoded_inst_get_length(&xedd);   //get the length of the instruction in byte

             xed_uint64_t runtime_address = (xed_uint64_t)currentAddr;
             xed_decoded_inst_dump_intel_format(&xedd, buffer, 1024, runtime_address);

             printf("%s\n", buffer);
             printf("%d\n", instLength);

             //unsigned int operNum = xed_decoded_inst_noperands(&xedd);   //const xed_decoded_inst_t *p)
             //xed_reg_enum_t regNum = xed_decoded_inst_get_base_reg( &xedd, 0);
                                                                   //const xed_decoded_inst_t*, 
                                                                   //unsigned intmem_idx);

             //printf("operand number: %d\n", operNum);
             //printf("register: %d\n\n", regNum);

	     currentAddr += instLength;
        }
        else
            return false;
    }

    return true;
}


unsigned char code[] = "\x55\x89\xe5\x83\xec\x70\xc7\x45\xfc\x03\x00\x00\xc7\x45\xf8\x05\x00\x00\x8b\x45\xf8\x8b\x55\xfc\x8d\x04\x02\x89\x45\xf4\x8b\x45\xfc\x0f\xaf\x45\xf8\x89\x45\xf0\xc7\x45\xec\x00\x00\x00\xeb\x1f\x8b\x4d\xec\x8b\x45\xf8\x8b\x55\xfc\x8d\x04\x02\x03\x45\xec\x89\xc2\xc1\xfa\x1f\xf7\x7d\xf4\x89\x44\x8d\x9c\x83\x45\xec\x01\x83\x7d\xec\x13\x0f\x9e\xc0\x84\xc0\x75\xd6\xb8\x00\x00\x00\x00\xc9\xc3";


int main()
{

    xed_tables_init();

    //int length = strlen(code);
    analyseProcedure((void*)code, (void*)(code + 97));

    return 0;
}


