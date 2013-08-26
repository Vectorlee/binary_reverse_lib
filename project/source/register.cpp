// Register.cpp
// 
// The implementation of the class Register
//

#include "register.h"
#include <stdio.h>

Register::Register()
{
    EAX = NULL;   //the common registers
	EBX = NULL;   //NULL                              
	ECX = NULL;
	EDX = NULL;

	AH = AL = NULL;
	BH = BL = NULL;
	CH = CL = NULL;
	DH = DL = NULL;

	EDI = NULL;
	ESI = NULL;

	EBP = UNKNOWN;
	ESP = UNKNOWN;

	// set the flag
	flagInfo.operandVar1 = NULL;
	flagInfo.operandVar2 = NULL;
	flagInfo.calcResult  = NULL;
	flagInfo.size = 0;
}


void
Register::resetESP() // set the ESP to the unknow state
{
	ESP = UNKNOWN;
}


void
Register::setESP()  // initial the value of ESP
{
	ESP = 0;        // The value of ESP here is aimed to track the distance
}                   // between ebp and esp. so when the ESP = 0, means that the ebp = esp know  


void
Register::addESP(int value)   // add a value to ESP  
{
	if((unsigned int)ESP == UNKNOWN){   // if the ESP is unknow, we ignore the change of it.
		return;
	}

	ESP += value;
} 


void
Register::subESP(int value)   // sub a value to ESP
{
	if((unsigned int)ESP == UNKNOWN){
		return;
	}

	ESP -= value;
}


int
Register::getESP()
{
	return ESP;
}


AbstractVariable* 
Register::getRegister(xed_reg_enum_t src_reg)
{
	AbstractVariable* source = NULL;

	switch(src_reg)
	{
		case XED_REG_EAX:
		case XED_REG_AX:  source = this -> EAX; break;

		case XED_REG_EBX:
		case XED_REG_BX:  source = this -> EBX; break;

		case XED_REG_ECX:
		case XED_REG_CX:  source = this -> ECX; break;

		case XED_REG_EDX:
		case XED_REG_DX:  source = this -> EDX; break;

		case XED_REG_ESI: source = this -> ESI; break;
		case XED_REG_EDI: source = this -> EDI; break;

		case XED_REG_AL:  source = this -> AL;  break;
		case XED_REG_AH:  source = this -> AH;  break;

		case XED_REG_BL:  source = this -> BL;  break;
		case XED_REG_BH:  source = this -> BH;  break;

		case XED_REG_CL:  source = this -> CL;  break;
		case XED_REG_CH:  source = this -> CH;  break;

		case XED_REG_DL:  source = this -> DL;  break;
		case XED_REG_DH:  source = this -> DH;  break;

		default: ASSERT(false); break;
	}

	return source;
}

void 
Register::setRegister(xed_reg_enum_t src_reg, AbstractVariable* value)
{
	switch(src_reg)
	{
		// big picture 1: 
		// when you set a upper register, the lower register will also be set.
		case XED_REG_EAX:
		case XED_REG_AX:  this -> EAX = value; this -> AH = value; this -> AL = value; break;

		case XED_REG_EBX:
		case XED_REG_BX:  this -> EBX = value; this -> BH = value; this -> BL = value; break;

		case XED_REG_ECX:
		case XED_REG_CX:  this -> ECX = value; this -> CH = value; this -> CL = value; break;

		case XED_REG_EDX:
		case XED_REG_DX:  this -> EDX = value; this -> DH = value; this -> DL = value; break;

		case XED_REG_ESI: this -> ESI = value; break;
		case XED_REG_EDI: this -> EDI = value; break;

		// big picture two
		// if you set the lower register, the upper register will be invalid
		case XED_REG_AL:  this -> AL = value;  this -> EAX = NULL; break;
		case XED_REG_AH:  this -> AH = value;  this -> EAX = NULL; break;

		case XED_REG_BL:  this -> BL = value;  this -> EBX = NULL; break;
		case XED_REG_BH:  this -> BH = value;  this -> EBX = NULL; break;

		case XED_REG_CL:  this -> CL = value;  this -> ECX = NULL; break;
		case XED_REG_CH:  this -> CH = value;  this -> ECX = NULL; break;

		case XED_REG_DL:  this -> DL = value;  this -> EDX = NULL; break;
		case XED_REG_DH:  this -> DH = value;  this -> EDX = NULL; break;

		default: ASSERT(false); break;
	}

	return;
}



void 
Register::setFlagInfo(int size, AbstractVariable* var1, AbstractVariable* var2, AbstractVariable* result)
{
	// some of these variable may be NULL.
	flagInfo.operandVar1 = var1;
	flagInfo.operandVar2 = var2;

	flagInfo.calcResult = result;

	ASSERT(size == 8 || size == 16 || size == 32);
	flagInfo.size = size;
}


FlagFactor 
Register::getFlagInfo()
{
	return flagInfo;
}


void 
Register::copyEnvironment(Register* reg_file)
{
	// copy the containt of another register class.
	// this is the environment of the code.
	EAX = reg_file -> EAX;   //the common registers
	EBX = reg_file -> EBX;   //reg_file ->                               
	ECX = reg_file -> ECX;
	EDX = reg_file -> EDX;

	AH = reg_file -> AH;
	AL = reg_file -> AL;
	BH = reg_file -> BH;
	BL = reg_file -> BL;
	CH = reg_file -> CH;
	CL = reg_file -> CL;
	DH = reg_file -> DH;
	DL = reg_file -> DL;

	EDI = reg_file -> EDI;
	ESI = reg_file -> ESI;

	EBP = reg_file -> EBP;   // EBP and ESP is int 
	ESP = reg_file -> ESP;

	// copy the containt of the flag
	flagInfo.operandVar1 = reg_file -> flagInfo.operandVar1;
	flagInfo.operandVar2 = reg_file -> flagInfo.operandVar2;
	flagInfo.calcResult  = reg_file -> flagInfo.calcResult;
	flagInfo.size        = reg_file -> flagInfo.size;
}
