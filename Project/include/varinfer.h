// varinfer.h
//
// Inferenece the variable like memory block.
//
//
//


#ifndef VARINFER_H  //VarInfer
#define VARINFER_H

#include <map>
extern "C"
{
    #include "xed-interface.h"
}
#include "register.h"
#include "tracker.h"

// we need to trace change of esp;
// Register *RegPtr = new Register();
// Tracker  *TrackState = new Tracker(RegPtr);

enum VariableLocation {Heap, Stack, Abosulte};  // the possible location of a variables

//typedef xed_reg_enum_t RegisterType;

struct AbstractVariable
{
	VariableLocation region;
	//RegisterType base;
	int offset;
	int size;
};



class VariableHunter
{

private:
	std::map<int, AbstractVariable*> stack_variable;
	std::map<int, AbstractVariable*> heap_variable;
	std::map<int, AbstractVariable*> abosulte_variable;

	Register *RegPtr;
	Tracker  *TrackState;       // Tracker will be much more useful when we try to find heap variable.

public:

	VariableHunter()
	{
		RegPtr = NULL;
		TrackState = NULL;
	}

	~VariableHunter()
	{
		if(RegPtr != NULL)
			delete RegPtr;

		if(TrackState != NULL)
			delete TrackState;
	}

	void initState();

	void findVariable(xed_decoded_inst_t &xedd);

	void getResult(std::map<int, AbstractVariable*> &container);


private:

	void findStackVariable(xed_decoded_inst_t &xedd);

	void findHeapVariable(xed_decoded_inst_t &xedd);

	void findAbosulteVariable(xed_decoded_inst_t &xedd);

};


#endif

