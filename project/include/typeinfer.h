

#ifndef TYPEINFER_H
#define TYPEINFER_H

#include <map>
#include <list>

#include "register.h"
#include "tracker.h"
#include "trailer.h"

#include "constraint.h"


extern "C"
{
    #include "xed-interface.h"
}


class TypeHunter
{
private:

	Register *regPtr;
	
	Tracker  *TrackState;    //the tracker will assign the id to each variable.
	StackTrailer *trailer;

	std::list<Constraint*> typeContainer;


	void generateConstraint(xed_decoded_inst_t &xedd);

	void insertConstraint(AbstractVariable* var, int size, int level);

	//void var
	//void constraintADD(AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult);

public:

	TypeHunter(){

		TrackState = NULL;
		trailer = NULL;
		regPtr = NULL;

		typeContainer.clear();

		InitialType();
	}

	~TypeHunter()
	{
		if(TrackState != NULL){
			delete TrackState;
		}

		if(trailer != NULL){
			delete trailer;
		}

		if(regPtr != NULL){
			delete regPtr;
		}

	}

	void initState(std::map<int, AbstractVariable*> &container);
 
	void assignConstraint(xed_decoded_inst_t &xedd);

	void getResult(std::list<Constraint*> &container, std::map<int, AbstractVariable*> &variable);

};


#endif