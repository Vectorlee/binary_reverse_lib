// utility.h
//
// The file that contain some global define. 
//

#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h> 

extern "C"
{
    #include "xed-interface.h"
}


#define UNKNOWN 0xDEADBEEF

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


#endif