

#ifndef typeslvr
#define typeslvr


#include "constraint.h"
#include "utility.h"
#include <map>


class TypeSolver
{

private:
	std::list<Constraint*> typeContainer;   //remain working list.
	
	std::list<Constraint*> pastContainer;
	std::list<Constraint*> remainContainer;

	Constraint* curConstraint;
	// the container contains all the constraint information

	std::map<int, TYPE*> lowerBound;
	std::map<int, TYPE*> upperBound;

	std::map<int, AbstractVariable*> varContainer;

	std::map<int, int> equalityPair;	

public:

	TypeSolver(std::list<Constraint*> &container, std::map<int, AbstractVariable*> &variable);

	bool replaceVariable(int var1, int var2);
	bool replaceVariable(int var1, TYPE* type);

	void equalConstraint(Element* item);

	void processSolving();

	void subConstraint(Element* item);

	void searchResult(int var, TYPE* type);
};


#endif

