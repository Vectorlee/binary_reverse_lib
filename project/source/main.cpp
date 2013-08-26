#include <string.h>
#include <stdio.h>
#include <map>
#include <list>

extern "C"
{
    #include "xed-interface.h"
}


#include "utility.h"
#include "varinfer.h"
#include "typeinfer.h"
#include "typeslvr.h"
#include "constraint.h"

#include "typemgr.h"


//#include "hunter.h"

//#include "tracker.h"
//#include "register.h"

//  analyseProcedure:
//       analyse a procedure staticaly, check each instrcution from 
//       the frist to the last of the procedure, 
//   
//       startAddr:  the start address of a procedure
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



bool VariableRecovery(BYTE *startAddr, BYTE *endAddr, std::map<int, AbstractVariable*> &container)
{

    const unsigned int maxInstructionLength = 15;   // the max length of a x86 instruction is 15byte
    xed_decoded_inst_t xedd;
    int instLength, index;
	
    BYTE *currentAddr = startAddr;
    char buffer[1024];


    VariableHunter *varHunter = new VariableHunter(); // initialize the variable hunter

    static const xed_state_t dstate = { XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b };
    //currently, we only implement the 32bit machine

    index = 0;
    while(currentAddr < endAddr) // only small than, can't be equal.
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


            varHunter -> findVariable(xedd);

	        currentAddr += instLength;
            index += instLength;

        }
        else
            return false;
    }

    varHunter -> getResult(container);

    delete varHunter;
    return true;
}



bool TypeInference(BYTE *startAddr, BYTE *endAddr, std::map<int, AbstractVariable*> &container)
{
    /*
    const unsigned int maxInstructionLength = 15;   // the max length of a x86 instruction is 15byte
    xed_decoded_inst_t xedd;
    int instLength, index;
    
    BYTE *currentAddr = startAddr;
    TypeHunter *typeHunter = new TypeHunter(container);

    static const xed_state_t dstate = { XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b };

    index = 0;
    while(currentAddr < endAddr)
    {
        //memset(buffer, 0, sizeof(buffer));
        xed_decoded_inst_zero_set_mode(&xedd, &dstate);
        xed_error_enum_t xedCode = xed_decode(&xedd, (uint8_t*)currentAddr, maxInstructionLength);
        
        if(xedCode == XED_ERROR_NONE)
        {
            instLength = xed_decoded_inst_get_length(&xedd);   //get the length of the instruction in byte

            //xed_uint64_t runtime_address = (xed_uint64_t)currentAddr;
            //xed_decoded_inst_dump_intel_format(&xedd, buffer, 1024, runtime_address);
            //printf("0x%x\t\t%s\n", index, buffer);

            typeHunter -> assignConstraint(xedd);

            currentAddr += instLength;
            index += instLength;

        }
        else
            return false;
    }
    */
    
    TypeHunter *typeHunter = new TypeHunter(container);
    
    TypeManager *mgr = new TypeManager(typeHunter, startAddr, endAddr);
    mgr -> buildCFG();
    mgr -> buildMatrix();
    mgr -> calcDeep();

    mgr -> analyzeBinary();


    // get the constraints and print it.
    std::list<Constraint*> typeContainer;
    std::map<int, AbstractVariable*> variable;

    typeHunter -> getResult(typeContainer, variable); 
    std::list<Constraint*>::iterator ptr;

    printf("Constraints:\n");
    for(ptr = typeContainer.begin(); ptr != typeContainer.end(); ptr++)
    {
        Constraint *Q = (*ptr);
        Q -> Print();
    }

    printf("\n\nTypes:\n");

    // solve the constraints and print it.
    TypeSolver *solver = new TypeSolver(typeContainer, variable);
    solver -> processSolving();
    
    delete mgr;
    delete typeHunter;
    delete solver;

    return true;
}



// we put the byte code in a char string.
// and then analysis this string.
//unsigned char code[] = "\x55\x89\xe5\x83\xec\x70\xc7\x45\xfc\x0f\x84\x01\x00\xc7\x45\xf8\xe3\x2b\x00\x00\x8b\x45\xf8\x8b\x55\xfc\x8d\x04\x02\x89\x45\xf4\x8b\x45\xfc\x01\xc0\x89\x45\xf0\xc7\x45\xec\x00\x00\x00\x00\xeb\x14\x8b\x45\xec\x8b\x55\xf0\x8b\x4d\xec\x8d\x14\x11\x89\x54\x85\x9c\x83\x45\xec\x01\x83\x7d\xec\x13\x0f\x9e\xc0\x84\xc0\x75\xe1\xb8\x00\x00\x00\x00\xc9\xc3";
char code[] = "\x55\x89\xe5\x83\xe4\xf0\x83\xec\x30\x65\xa1\x14\x00\x00\x00\x89\x44\x24\x2c\x31\xc0\xc7\x44\x24\x10\x0f\x84\x01\x00\xc7\x44\x24\x0c\x6f\x04\x00\x00\x8b\x44\x24\x0c\x8b\x54\x24\x10\x8d\x04\x02\x89\x44\x24\x08\x8b\x44\x24\x10\x0f\xaf\x44\x24\x0c\x89\x44\x24\x04\xc6\x44\x24\x17\x43\xc7\x04\x24\x00\x00\x00\x00\xeb\x2c\x8b\x04\x24\x8b\x14\x24\x89\xd1\x0f\xb6\x54\x24\x17\x8d\x14\x11\x88\x54\x04\x18\x8b\x04\x24\x0f\xb6\x44\x04\x18\x3c\x50\x75\x08\x8b\x04\x24\xc6\x44\x04\x18\x5a\x83\x04\x24\x01\x83\x3c\x24\x13\x0f\x9e\xc0\x84\xc0\x75\xc9\xb8\x00\x00\x00\x00\x8b\x54\x24\x2c\x65\x33\x15\x14\x00\x00\x00\x74\x05\xe8\xfc\xff\xff\xff\xc9\xc3";
//unsigned char code[] = "\x55\x89\xe5\x83\xec\x10\xc7\x45\xfc\x03\x00\x00\x00\xc7\x45\xf8\x01\x00\x00\x00\xc7\x45\xf0\x00\x00\x00\x00\xeb\x0e\x8b\x45\xf8\x0f\xaf\x45\xfc\x89\x45\xf8\x83\x45\xf0\x01\x83\x7d\xf0\x06\x0f\x9e\xc0\x84\xc0\x75\xe7\x8b\x45\xf8\x83\xe8\x64\x89\x45\xf4\x8b\x45\xf8\xba\x00\x00\x00\x00\xf7\x75\xf4\xc9\xc3";


int main()
{

    xed_tables_init();

    std::map<int, AbstractVariable*> container;


    // main analyse process
    VariableRecovery(code, (code + 159), container);
    

    // print the variables.
    printf("\n\nThe Variables:\n");
    std::map<int, AbstractVariable*>::iterator ptr;
    for(ptr = container.begin(); ptr != container.end(); ptr++)
    {
        printf("variable %d: offset: %d, size: %d\n", (ptr -> first + 6), (ptr -> second) -> offset, (ptr -> second) -> size);
    }
    printf("\n\n");
    
    // infere the type and print it.
    TypeInference(code, (code +  159), container);
    
    return 0;
}
