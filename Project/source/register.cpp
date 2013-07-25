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
		case XED_REG_EAX:
		case XED_REG_AX:  this -> EAX = value; break;

		case XED_REG_EBX:
		case XED_REG_BX:  this -> EBX = value; break;

		case XED_REG_ECX:
		case XED_REG_CX:  this -> ECX = value; break;

		case XED_REG_EDX:
		case XED_REG_DX:  this -> EDX = value; break;

		case XED_REG_ESI: this -> ESI = value; break;
		case XED_REG_EDI: this -> EDI = value; break;

		case XED_REG_AL:  this -> AL = value;  break;
		case XED_REG_AH:  this -> AH = value;  break;

		case XED_REG_BL:  this -> BL = value;  break;
		case XED_REG_BH:  this -> BH = value;  break;

		case XED_REG_CL:  this -> CL = value;  break;
		case XED_REG_CH:  this -> CH = value;  break;

		case XED_REG_DL:  this -> DL = value;  break;
		case XED_REG_DH:  this -> DH = value;  break;

		default: ASSERT(false); break;
	}

	return;
}