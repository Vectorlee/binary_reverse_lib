

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

	//Register *regPtr;
	
	Tracker  *TrackState;    //the tracker will assign the id to each variable.
	StackTrailer *trailer;

	std::list<Constraint*> typeContainer;


	void generateConstraint(xed_decoded_inst_t &xedd);

	//void insertConstraint(AbstractVariable* var, int size, int level);

public:

	TypeHunter(std::map<int, AbstractVariable*> &container);

	~TypeHunter();
 
	void assignConstraint(xed_decoded_inst_t &xedd);

	void addConstraint(Constraint* cont);

	void resetState(Register* reg_file);


	// the final function, get the result from the TypeHunter
	void getResult(std::list<Constraint*> &container, std::map<int, AbstractVariable*> &variable);

};


#endif