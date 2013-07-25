// Tracker.h
// 
// The head file of class Tracker
// this class track the change of registers
// maintain the things that different registers hold.
//

#ifndef TRACKER_H
#define TRACKER_H

#include "register.h"
#include "utility.h"
extern "C"
{
    #include "xed-interface.h"
}


class Tracker
{

private:
	Register *regPtr;

	void setRegister(xed_reg_enum_t src_reg, AbstractVariable* value) { regPtr -> setRegister(src_reg, value); }

public:
	Tracker(){
		regPtr = new Register();
		regPtr -> resetESP();
	}

	~Tracker(){
		delete regPtr;
	}

	int getESP(){
		return regPtr -> getESP();
	}

	bool trackESP(xed_decoded_inst_t &xedd);

    // we track the movement of variables between registers 
	bool trackVariable(xed_decoded_inst_t &xedd);

    // we can't always track successfully using first function.
    // we need some outer information to alert our tracking work, 
    // this function use outer information and fix result.
	bool fixTracking(xed_decoded_inst_t &xedd, AbstractVariable* value);

	// we encapsulate the operation of register. 
	// the outer space only need to deal with Tracker

    AbstractVariable* getRegister(xed_reg_enum_t src_reg) { return regPtr -> getRegister(src_reg); } 

};


#endif