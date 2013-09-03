
// extract.h
//
// I name it extract h, means that it extract Constraints from different instructions.
// this will be a very big file.


#ifndef EXTRACT_H
#define EXTRACT_H

#include "constraint.h"
#include "utility.h"
#include <list>


Constraint* ConstraintADD(TYPE* type, AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult);
//Constraint* ConstraintADD(AbstractVariable *var1, AbstractVariable* calcResult, TYPE* type);

Constraint* ConstraintSUB(TYPE* type, AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult);
//Constraint* ConstraintSUB(AbstractVariable *var1, AbstractVariable* calcResult, TYPE* type);

//Constraint* ConstraintMUL(AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult, TYPE* type);
//Constraint* ConstraintIMUL(AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult, TYPE* type);

// this function is designed for DIV and IDIV 
Constraint* ConstraintDIV(TYPE* type, AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult, AbstractVariable* remaind);


// DEC INC
Constraint* ConstraintDEC_INC(TYPE* type, AbstractVariable *var1);

// one source operand, and one result.
// SAL, SHL, SAR, SHR
// ROR, ROL, RCL, RCR 
Constraint* ConstraintSHIFT(TYPE* type, AbstractVariable *var1, AbstractVariable *calcResult);

// NOT, NEG
Constraint* ConstraintREVERSE(TYPE* type, AbstractVariable *var1, AbstractVariable *calcResult);



// SETB,   SETBE,  SETL,   SETLE,  SETNB 
// SETNBE, SETNL,  SETNLE, SETNO,  SETNP    
// SETNS,  SETNZ,  SETO,   SETP,   SETS,  SETZ
Constraint* ConstraintSET(TYPE* type, AbstractVariable *calcResult); 

//Constraint* ConstraintJMPcc() 


// Constraint model of cmp is quite the same as sub
Constraint* ConstraintCMP(TYPE* type, AbstractVariable *var1, AbstractVariable* var2);
//Constraint* ConstraintCMP(AbstractVariable *var1, TYPE* type);

Constraint* ConstraintMOV(AbstractVariable *var1, AbstractVariable *var2);


Constraint* ConstraintMOVX(TYPE* type1, TYPE* type2, AbstractVariable *var1, AbstractVariable *var2);


// this two function is designed for merge.
Constraint* ConstraintEqual(int var1, int var2);
Constraint* ConstraintMerge(int var1, std::list<int> var_list);


Constraint* ConstraintLEA(AbstractVariable* contain, AbstractVariable* calcResult); 


// this is designed for the flag.
Constraint* ConditionFeedback(TYPE* type, AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult);

#endif