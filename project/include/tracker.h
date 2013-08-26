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
#include <list>
#include <map>
#include <set>


extern "C"
{
    #include "xed-interface.h"
}



class Tracker
{

private:

	int varIndex;
	Register *regPtr;

	FlagFactor flagInfo;

	// we category variables here, just as same as the VariableHunter
	std::map<int, AbstractVariable*> stack_variable;
	std::map<int, AbstractVariable*> heap_variable;
	std::map<int, AbstractVariable*> absolute_variable;

	std::list<AbstractVariable*> temp_variable;
	// these four containers contain the variables

	// These two containers contain the nodes and edges.
	//std::multimap<int, int> *edges_container;
    //std::set<int> *nodes_container;


	//void emptyTrash();
    void setRegister(xed_reg_enum_t dest_reg, AbstractVariable* value) { regPtr -> setRegister(dest_reg, value); }

public:

	Tracker(std::map<int, AbstractVariable*> &container, Register* reg);

	~Tracker();

	// we generate temporary variable, if it can generate temporary variable, we return it.
	// if not, we return none.

	// but thing is more tricky, because we can't eliminate the variable information in the register,
	// before the TypeSolver get a chance to insert type constraints in it.

	// so we should get a opposite approach, TypeSolver generate thing first, if there is a result
	// (so called temporary variable), we will pass it to Tracker, or we only pass NULL

    // we track the movement of variables between registers 
	bool trackVariable(xed_decoded_inst_t &xedd);

    // we can't always track successfully using first function.
    // we need some outer information to alert our tracking work, 
    // this function use outer information and fix result.

	//bool fixTracking(xed_decoded_inst_t &xedd, AbstractVariable* value);

	// we encapsulate the operation of register. 
	// the outer space only need to deal with Tracker

	void setTempVariable(xed_reg_enum_t dest_reg, AbstractVariable* value); // { regPtr -> setRegister(src_reg, value); }


    AbstractVariable* getRegister(xed_reg_enum_t src_reg) { return regPtr -> getRegister(src_reg); }
    AbstractVariable* getMemoryVariable(xed_decoded_inst_t &xedd);


    void setFlagInfo(int size, AbstractVariable* var1, AbstractVariable* var2, AbstractVariable* result);
    FlagFactor getFlagInfo();


    void getResult(std::map<int, AbstractVariable*> &container);

    void resetEnvironment(Register *reg_file);

};

#endif