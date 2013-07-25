

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <map>
#include "utility.h"
#include "register.h"
#include "tracker.h"

extern "C"
{
    #include "xed-interface.h"
}


class TypeSolver
{
private:

	// we category variables here, just as same as the VariableHunter
	std::map<int, AbstractVariable*> stack_variable;
	std::map<int, AbstractVariable*> heap_variable;
	std::map<int, AbstractVariable*> absolute_variable;
	std::map<int, AbstractVariable*> temp_variable;  


	//Register *RegPtr;
	Tracker  *TrackState;  

	AbstractVariable* getVariableDescriptor(xed_decoded_inst_t &xedd); //, std::map<int, AbstractVariable*> &container);

	void generateConstraint(xed_decoded_inst_t &xedd);

	void insertConstraint(AbstractVariable* var, int size, int level);

public:

	~TypeSolver()
	{
		if(TrackState != NULL)
			delete TrackState;
	}

	void initState(std::map<int, AbstractVariable*> &container);
 
	void getConstraint(xed_decoded_inst_t &xedd);

	void getResult(std::map<int, AbstractVariable*> &container);

};


#endif