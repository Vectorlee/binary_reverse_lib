


#ifndef TRAILER_H
#define TRAILER_H

#include "register.h"
#include "utility.h"
extern "C"
{
    #include "xed-interface.h"
}


class StackTrailer
{

private:
	Register *regPtr;

public:
	
	//StackTrailer(){
	//	regPtr = new Register();
	//	regPtr -> resetESP();
	//}

	StackTrailer(Register* reg){
		regPtr = reg;
	}

	int getESP(){
		return regPtr -> getESP();
	}

	bool trackESP(xed_decoded_inst_t &xedd);
	
	void resetEnvironment(Register *reg_file);
};


#endif