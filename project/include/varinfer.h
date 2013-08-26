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
#include "trailer.h"
#include "utility.h"


class VariableHunter
{

private:
	std::map<int, AbstractVariable*> stack_variable;
	std::map<int, AbstractVariable*> heap_variable;
	std::map<int, AbstractVariable*> absolute_variable;
	std::map<int, AbstractVariable*> temp_variable;  // variables in register.
	// it would be multimap after wards


	Register* regPtr;
	StackTrailer  *TrackState;       // Tracker will be much more useful when we try to find heap variable.


public:

	VariableHunter();

	~VariableHunter();

	void findVariable(xed_decoded_inst_t &xedd);

	void getResult(std::map<int, AbstractVariable*> &container);


private:

	void findStackVariable(xed_decoded_inst_t &xedd);

	void findHeapVariable(xed_decoded_inst_t &xedd);

	void findAbsoluteVariable(xed_decoded_inst_t &xedd);

};


#endif

