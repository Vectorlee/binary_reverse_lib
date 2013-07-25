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
#include "utility.h"


class VariableHunter
{

private:
	std::map<int, AbstractVariable*> stack_variable;
	std::map<int, AbstractVariable*> heap_variable;
	std::map<int, AbstractVariable*> absolute_variable;
	std::map<int, AbstractVariable*> temp_variable;  // variables in register.
	// it would be multimap after wards


	//Register *RegPtr;
	Tracker  *TrackState;       // Tracker will be much more useful when we try to find heap variable.

public:

	~VariableHunter()
	{
		if(TrackState != NULL)
			delete TrackState;
	}

	void initState();

	void findVariable(xed_decoded_inst_t &xedd);

	void getResult(std::map<int, AbstractVariable*> &container);


private:

	void findStackVariable(xed_decoded_inst_t &xedd);

	void findHeapVariable(xed_decoded_inst_t &xedd);

	void findAbsoluteVariable(xed_decoded_inst_t &xedd);

};


#endif

