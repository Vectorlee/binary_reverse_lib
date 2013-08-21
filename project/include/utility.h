// utility.h
//
// The file that contain some global define. 
//

#ifndef UTILITY_H
#define UTILITY_H

#include <stdlib.h> 
#include <stdio.h>
#include <list>

extern "C"
{
    #include "xed-interface.h"
}


#define UNKNOWN 0xDEADBEEF


// -----------------------------------------------------------
// Variable data structure.
//

enum VariableLocation { Heap, Stack, Absolute, Temporary };  // the possible location of a variables
// Temporary means the variable in Register.

typedef xed_reg_enum_t RegisterType;

class AbstractVariable
{
public:
	
	int id;    // the id of the variable, this is used in the constraints generation.
	
	VariableLocation region;
	RegisterType base;

	int offset;
	int size;

	AbstractVariable(){
		id = 0;
		offset = 0;
		size = 0;
	}
};


//----------------------------------------------------------------------
// ASSERT
//  If condition is false,  print a message and dump core.
//  Useful for documenting assumptions in the code.
//
//----------------------------------------------------------------------
#define ASSERT(condition)                                                     \
    if (!(condition)) {                                                       \
        fprintf(stderr, "Assertion failed: line %d, file \"%s\"\n",           \
                __LINE__, __FILE__);                                          \
        exit(1);                                                              \
    }



typedef int   DWORD;
typedef short WORD;
typedef char  BYTE;


xed_reg_enum_t SrcRegister(xed_decoded_inst_t &xedd);
xed_reg_enum_t DestRegister(xed_decoded_inst_t &xedd);

xed_int64_t GetImmediate(xed_decoded_inst_t &xedd, int &mark);

bool IsMemoryOperand(xed_decoded_inst_t &xedd, int index); ///// {return false; }

bool IsRegisterOperand(xed_decoded_inst_t &xedd, int index);

bool IsImmediateOperand(xed_decoded_inst_t &xedd, int index);


int RegisterOperandSize(xed_decoded_inst_t &xedd, int index);

int MemoryOperandSize(xed_decoded_inst_t &xedd, int index);

int ImmediateOperandSize(xed_decoded_inst_t &xedd, int index);

#endif










