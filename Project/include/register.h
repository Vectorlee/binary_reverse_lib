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


#include "utility.h"

class Register
{
private:
	int EAX;   //the common registers
	int EBX;
	int ECX;
	int EDX;

	int EBP;
	int ESP;

	int EDI;
	int ESI;
	           //we currently only have used these registers

    int CS;    //the segment registers   
	int SS;
	int ES;
	int DS;
	int FS;
	int GS;

	int EIP;   //this state registers
	int EFLAGS;

public:

    // Here actually is the not the real value of ESP, but the relative value, compared to EBP.
    // So the value of ESP here is the distance between ESP and EBP.
	void resetESP();          // set the ESP to the unknow state
	void setESP();            // set the ESP to 0. 

	void addESP(int value);   // add a value to ESP  
    void subESP(int value);   // sub a value to ESP
    int getESP();             // return the value of ESP

};

#endif