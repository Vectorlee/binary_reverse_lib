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

// ----------------------------------------------------------
// Type data structure
//

enum TypeHierarchy { All, Int8, Uint8, Int16, Uint16, Int32, Uint32, Num8, Num16, Num32, Ptr_s, Reg8, Reg16, Reg32, Unknown };

const char* TypeToStr(TypeHierarchy type);


class Constraint
{
public:
	TypeHierarchy up_bound;
	TypeHierarchy low_bound;

	Constraint() { up_bound = All; low_bound = Unknown; }
	Constraint(TypeHierarchy a, TypeHierarchy b) { up_bound = a, low_bound = b; }
	
	void Print(){
		printf("up bound: %s, low bound: %s\n", TypeToStr(up_bound), TypeToStr(low_bound)); 
	}
};


// -----------------------------------------------------------
// Variable data structure.
//

enum VariableLocation {Heap, Stack, Absolute, Temporary };  // the possible location of a variables
// Temporary means the variable in Register.

typedef xed_reg_enum_t RegisterType;

class AbstractVariable
{
public:
	//int id;
	VariableLocation region;
	RegisterType base;

	int offset;
	int size;

	std::list<Constraint*> typeinfo; 

	AbstractVariable()  { typeinfo.clear(); }
	~AbstractVariable() {
        std::list<Constraint*>::iterator ptr;
        for(ptr = typeinfo.begin(); ptr != typeinfo.end(); ptr++){
        	delete (Constraint*)(*ptr);
        }  // we should prevent memory leak.
	}      // delete the dynamic memory block;
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










