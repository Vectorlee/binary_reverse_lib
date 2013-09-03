// Register.cpp
// 
// The implementation of the class Register
//

#include "register.h"
#include "tracker.h"
#include <stdio.h>
#include <algorithm>


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


// private function

void 
Register::mergeSet(xed_reg_enum_t reg, AbstractVariable* mergeVar, void *trackState, std::multimap<int, int> &container)
{

	Tracker* TrackState = (Tracker*)trackState;
	AbstractVariable *regVar = this -> getRegister(reg);

	// if reg_file -> EAX != NULL
	if(mergeVar != NULL)
	{
		//if(this -> EAX == NULL)
		//	this -> EAX =  mergeVar  //reg_file -> EAX;   // if the current register is NULL, just copy.
		if(regVar == NULL)
			this -> setRegister(reg, mergeVar);
		else{
			// we need to judge whether this variable is a variable we create newly, or just a variable before.
			// search the container.

			int id = regVar -> id;  // (this -> EAX) -> id;
			if(container.find(id) != container.end()){    // this is a newly create merge variable
			// merege again.
			    int from = mergeVar -> id;  //reg_file -> EAX -> id;
			    int to = regVar -> id;      //this -> EAX -> id;

			    //this -> EAX -> size = std::min(reg_file -> EAX -> size, this -> EAX -> size); // set size to smaller one.
			    regVar -> size = std::min(mergeVar -> size, regVar -> size);
			    container.insert(std::make_pair(to, from));
			}
			else{  // we haven't found any variable in the map, this is an old variable, we need to create a new one for merge.

				int from1 = regVar -> id;    // this -> EAX -> id;
			    int from2 = mergeVar -> id;  // reg_file -> EAX -> id;

			    if(from1 == from2)       // if each branch share same variable in same variable, just copy once.
			    	return;

			    AbstractVariable* tmpVar = new AbstractVariable();
			    tmpVar -> region = Temporary;
			    // var -> size = std::min(reg_file-> EAX -> size, this -> EAX -> size);
			    tmpVar -> size = std::min(regVar -> size, mergeVar -> id);

			    TrackState -> setTempVariable(reg, tmpVar);

			    ASSERT(tmpVar -> id != 0);
			    int to = tmpVar -> id;

			    container.insert(std::make_pair(to, from1));
			    container.insert(std::make_pair(to, from2));
			}
		}
	}

	return;
}


void 
Register::mergeEnvironment(Register* reg_file, void *trackState, std::multimap<int, int> &container)
{

	//ASSERT(this -> EAX != NULL);
	//ASSERT((this -> EAX -> region == Temporary) && (this -> EAX -> id != 0));
	// void mergeSet(xed_reg_enum_t reg, AbstractVariable* mergeVar, void *trackState, std::multimap<int, int> &container)

	this -> mergeSet(XED_REG_EAX, reg_file -> EAX, trackState, container);
	this -> mergeSet(XED_REG_EBX, reg_file -> EBX, trackState, container);
	this -> mergeSet(XED_REG_ECX, reg_file -> ECX, trackState, container);
	this -> mergeSet(XED_REG_EDX, reg_file -> EDX, trackState, container);
	this -> mergeSet(XED_REG_ESI, reg_file -> ESI, trackState, container);
	this -> mergeSet(XED_REG_EDI, reg_file -> EDI, trackState, container);

	
	this -> mergeSet(XED_REG_AH, reg_file -> AH, trackState, container);
	this -> mergeSet(XED_REG_AL, reg_file -> AL, trackState, container);
	this -> mergeSet(XED_REG_BH, reg_file -> BH, trackState, container);
	this -> mergeSet(XED_REG_BL, reg_file -> BL, trackState, container);
	this -> mergeSet(XED_REG_CH, reg_file -> CH, trackState, container);
	this -> mergeSet(XED_REG_CL, reg_file -> CL, trackState, container);
	this -> mergeSet(XED_REG_DH, reg_file -> DH, trackState, container);
	this -> mergeSet(XED_REG_DL, reg_file -> DL, trackState, container);

	return;
}


void 
Register::Print()
{
	if(EAX != NULL)
		printf("EAX: %d\n", EAX -> id);
	else
		printf("EAX: NULL\n");

	if(EBX != NULL)
		printf("EBX: %d\n", EBX -> id);
	else
		printf("EBX: NULL\n");
	
	if(ECX != NULL)
		printf("ECX: %d\n", ECX -> id);
	else
		printf("ECX: NULL\n");
	
	if(EDX != NULL)
		printf("EDX: %d\n", EDX -> id);
	else
		printf("EDX: NULL\n");
	
	if(EDI != NULL)
		printf("EDI: %d\n", EDI -> id);
	else
		printf("EDI: NULL\n");
	
	if(ESI != NULL)
		printf("ESI: %d\n", ESI -> id);
	else
		printf("ESI: NULL\n");
	
	if(AH != NULL)
		printf("AH: %d\n", AH -> id);
	else
		printf("AH: NULL\n");

	if(AL != NULL)
		printf("AL: %d\n", AL -> id);
	else
		printf("AL: NULL\n");
		
	if(BH != NULL)
		printf("BH: %d\n", BH -> id);
	else
		printf("BH: NULL\n");
	
	if(BL != NULL)
		printf("BL: %d\n", BL -> id);
	else
		printf("BL: NULL\n");
	
	if(CH != NULL)
		printf("CH: %d\n", CH -> id);
	else
		printf("CH: NULL\n");
	
	if(CL != NULL)
		printf("CL: %d\n", CL -> id);
	else
		printf("CL: NULL\n");
	
	if(DH != NULL)
		printf("DH: %d\n", DH -> id);
	else
		printf("DH: NULL\n");
	
	if(DL != NULL)
		printf("DL: %d\n", DL -> id);
	else
		printf("DL: NULL\n");

}


