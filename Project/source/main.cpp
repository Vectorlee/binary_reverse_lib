#include <string.h>
#include <stdio.h>
#include <map>

extern "C"
{
    #include "xed-interface.h"
}


#include "utility.h"
#include "varinfer.h"


//#include "tracker.h"
//#include "register.h"

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



bool AnalyseProcedure(void *startaddress, void *endAddr, std::map<int, AbstractVariable*> &container)
{

    const unsigned int maxInstructionLength = 15;   // the max length of a x86 instruction is 15byte
    xed_decoded_inst_t xedd;
    int instLength, index;
	
    void *currentAddr = startaddress;
    char buffer[1024];


    VariableHunter *VarHunter = new VariableHunter(); // initialize the variable hunter
    VarHunter -> initState();


    static const xed_state_t dstate = { XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b };
    //currently, we only implement the 32bit machine

    index = 0;
    while(currentAddr < endAddr)
    {
	    memset(buffer, 0, sizeof(buffer));
	
        xed_decoded_inst_zero_set_mode(&xedd, &dstate);
	    xed_error_enum_t xedCode = xed_decode(&xedd, (uint8_t*)currentAddr, maxInstructionLength);
		
	    if(xedCode == XED_ERROR_NONE)
	    {
            instLength = xed_decoded_inst_get_length(&xedd);   //get the length of the instruction in byte

            xed_uint64_t runtime_address = (xed_uint64_t)currentAddr;
            xed_decoded_inst_dump_intel_format(&xedd, buffer, 1024, runtime_address);
            printf("0x%x\t\t%s\n", index, buffer);


            VarHunter -> findVariable(xedd);
            //xed_reg_enum_t reg = DestRegister(xedd);
            //printf("Destination Register: %d\n", reg);

	        currentAddr += instLength;
            index += instLength;

        }
        else
            return false;
    }

    VarHunter -> getResult(container);

    return true;
}



// we put the byte code in a char string.
// and then analysis this string.
unsigned char code[] = "\x55\x89\xe5\x83\xec\x70\xc7\x45\xfc\x0f\x84\x01\x00\xc7\x45\xf8\xe3\x2b\x00\x00\x8b\x45\xf8\x8b\x55\xfc\x8d\x04\x02\x89\x45\xf4\x8b\x45\xfc\x01\xc0\x89\x45\xf0\xc7\x45\xec\x00\x00\x00\x00\xeb\x14\x8b\x45\xec\x8b\x55\xf0\x8b\x4d\xec\x8d\x14\x11\x89\x54\x85\x9c\x83\x45\xec\x01\x83\x7d\xec\x13\x0f\x9e\xc0\x84\xc0\x75\xe1\xb8\x00\x00\x00\x00\xc9\xc3";
//unsigned char code[] = "\x55\x89\xe5\x83\xec\x70\xc7\x45\xfc\x0f\x84\x01\x00\xc7\x45\xf8\xe3\x2b\x00\x00\x8b\x45\xf8\x8b\x55\xfc\x8d\x04\x02\x89\x45\xf4\x8b\x45\xfc\x01\xc0\x89\x45\xf0\xc7\x45\xec\x00\x00\x00\x00\xeb\x1f\x8b\x4d\xec\x8b\x45\xf8\x8b\x55\xfc\x8d\x04\x02\x03\x45\xec\x89\xc2\xc1\xfa\x1f\xf7\x7d\xf4\x89\x44\x8d\x9c\x83\x45\xec\x01\x83\x7d\xec\x13\x0f\x9e\xc0\x84\xc0\x75\xd6\xb8\x00\x00\x00\x00\xc9\xc3";


int main()
{

    xed_tables_init();

    std::map<int, AbstractVariable*> container;


    // main analyse process
    AnalyseProcedure((void*)code, (void*)(code + 87), container);
    

    printf("\n\nThe Variables:\n");

    std::map<int, AbstractVariable*>::iterator ptr;
    int i;

    for(i = 0, ptr = container.begin(); ptr != container.end(); ptr++, i++)
    {
        printf("variable %d: offset: %d, size: %d\n", i, (ptr -> second) -> offset, (ptr -> second) -> size);
    }

    return 0;
}
