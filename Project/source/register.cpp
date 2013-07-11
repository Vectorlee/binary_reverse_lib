// Register.cpp
// 
// The implementation of the class Register
//

#include "register.h"

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
