// Tracker.h
// 
// The head file of class Tracker
// this class track the change of registers
// maintain the things that different registers hold.
//

#ifndef TRACKER_H
#define TRACKER_H

#include "register.h"
extern "C"
{
    #include "xed-interface.h"
}


class Tracker
{
private:
	Register *regPtr;
	
public:
	Tracker(Register *pointer){
		regPtr = pointer;
		regPtr -> resetESP();
	}

	bool trackESP(xed_decoded_inst_t &xedd);

	int getESP(){
		return regPtr -> getESP();
	}

	bool trackVariable()
	{
		return false;
	}

};


#endif