// Register.h
//
// The head file of class Register.
//
// This class maintain the state of each common registers
// in the machine. we can't know the value of these registers,
// we focus on the 'things' that this registers 'hold': an immidiate number, or 
// a variable

#ifndef REGISTER_H
#define REGISTER_H

extern "C"
{
    #include "xed-interface.h"
}
#include "utility.h"


struct FlagFactor
{
	int size;

	AbstractVariable* operandVar1;
	AbstractVariable* operandVar2;

	AbstractVariable* calcResult;
};


class Register
{
private:
	AbstractVariable *EAX;   // the common registers
	AbstractVariable *EBX;   // we don't need to separate EAX and AX.                             
	AbstractVariable *ECX;   // if there is a variable in AX, the whole EAX is occupied.
	AbstractVariable *EDX;

    //AX, BX, CX, DX
	AbstractVariable *AH, *AL;
	AbstractVariable *BH, *BL;
	AbstractVariable *CH, *CL;
	AbstractVariable *DH, *DL;

    // if there is a variable hold in AH,
    // may be there is also a variable hold in AL. that's the reason we need to separate them.

	// if there is some variable in AX, we can extend it as EAX.
	AbstractVariable *EDI;
	AbstractVariable *ESI;     //we currently only have used these registers

	int EBP;    // we use them to track the offset of variable in stack. 
	int ESP;    // the esp and ebp can't hold variable

	FlagFactor flagInfo;

public:

	Register();

    // Here actually is the not the real value of ESP, but the relative value, compared to EBP.
    // So the value of ESP here is the distance between ESP and EBP.
	void resetESP();          // set the ESP to the unknow state
	void setESP();            // set the ESP to 0. 

	void addESP(int value);   // add a value to ESP  
    void subESP(int value);   // sub a value to ESP
    int getESP();             // return the value of ESP

    void setRegister(xed_reg_enum_t src_reg, AbstractVariable* value);
    AbstractVariable*  getRegister(xed_reg_enum_t src_reg);

    //copy a register class to a new register class.
    void copyEnvironment(Register* reg_file);


    void setFlagInfo(int size, AbstractVariable* var1, AbstractVariable* var2, AbstractVariable* result);
    FlagFactor getFlagInfo();

};

#endif