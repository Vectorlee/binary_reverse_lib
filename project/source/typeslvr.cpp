

#include "typeslvr.h"


TypeSolver::TypeSolver(std::list<Constraint*> &container, std::map<int, AbstractVariable*> &variable):varContainer(variable)
{
	typeContainer.clear();

	pastContainer.clear();
	remainContainer.clear();

	curConstraint = NULL;
	lowerBound.clear();
	upperBound.clear();

	equalityPair.clear();


	std::list<Constraint*>::iterator ptr;
	for(ptr = container.begin(); ptr != container.end(); ptr++)
	{
		typeContainer.insert(typeContainer.end(), (*ptr));  // the typeContainer contains all the constraints
		remainContainer.insert(remainContainer.end(), (*ptr)); // this is only a working list.
	}

	std::map<int, AbstractVariable*>::iterator itr;
	for(itr = variable.begin(); itr != variable.end(); itr++)
	{
		int id = itr -> first;
		//lowerBound.insert(std::make_pair(id, NULL));
		lowerBound[id] = NULL;
		upperBound[id] = NULL;
		//upperBound.insert(std::make_pair(id, NULL));
	}
}



bool 
TypeSolver::replaceVariable(int var1, int var2)
{
	//
	std::list<Constraint*>::iterator ptr;
	for(ptr = typeContainer.begin(); ptr != typeContainer.end(); ptr++)
	{
		//typeContainer.insert(typeContainer.end(), (*ptr));
		Constraint* constr = (*ptr);                    // (a^a^a) V (a^a^a)
		int num = constr -> getPartNumber(); 
		for(int i = 0; i < num; i++)
		{
			std::list<Element*> *part = constr -> getPart(i);     //  (a^a^a)
			std::list<Element*>::iterator itr;
			for(itr = part -> begin(); itr != part -> end(); itr++)
			{
				Element* item = (*itr);

				int left_var = item -> lefthand;
				if(left_var == var1){                   //replace
					item -> lefthand = var2;
					continue;
				}
				if(item -> mark == 1)
				{
					int right_var = item -> righthand.var;

					if(right_var == var1){
						item -> righthand.var = var2;   // replace
						continue;
					}
				}
			}

		}
	}
	return true; 
}


bool 
TypeSolver::replaceVariable(int var1, TYPE* type)
{
	return true;
}


void 
TypeSolver::equalConstraint(Element* item)
{
	// A = B
	//A is a memory variable and B is a temporary variable      replace every occurrence of B with A. 
	//A is a memory variable and B is a type                    A is done, replace every occurrence of A with the type.
	//A is a memory variable and B is a memory variable         replace every occurrence of B with A, and mark the equality

	//A is a temporary variable and B is a type.                replace every occurrence of A with the type B 
	//A is a temporary variable and B is a memory variable      replace every occurrence of A with the type B 
	//A is a temporary variable and B is a temporary variable   replace every occurrence of A with B.

    //Correlation relation;  // <, =, >
	//int lefthand;          // the left hand is always variable,  the id of the variable.

	//union{
	//	int   var;
	//	TYPE* type;
	//} righthand;

	//int mark;              // 0 is type, 1 is variable.
	//int category;

	Correlation relate = item -> relation;
	ASSERT(relate == Equal);

	int left_var = item -> lefthand;
	
	if(item -> mark == 1)               // variable = variable
	{
		int right_var = item -> righthand.var;
		VariableLocation region1, region2;

		region1 = varContainer[left_var]  -> region;
		region2 = varContainer[right_var] -> region;

		if(region1 != Temporary && region2 == Temporary){      // memory = temp
			replaceVariable(right_var, left_var);
		}
		else if(region1 != Temporary && region2 != Temporary){  // memory = memory
			replaceVariable(right_var, left_var);      // use any one of them to replace another. but
			equalityPair[right_var] = left_var;        // but we need to record
		}
		else if(region1 == Temporary && region2 == Temporary){  // temp = temp
			replaceVariable(right_var, left_var);  // if temporary equals temporary, use any of it to change the other
		}
		else if(region1 == Temporary && region2 != Temporary){  // temp = memory
			replaceVariable(left_var, right_var);
		}
		else{
			ASSERT(false);
		}
	}
	else if(item -> mark == 0)          // variable = type
	{
		//TODO
	}
	else{
		ASSERT(false);
	}

}

void 
TypeSolver::searchResult(int var, TYPE* type)
{
	/*
	int answer;

	std::list<Constraint*>::iterator ptr;
	for(ptr = typeContainer.begin(); ptr != typeContainer.end(); ptr++)
	{
		// typeContainer.insert(typeContainer.end(), (*ptr));
		Constraint* constr = (*ptr);                    // (a^a^a) V (a^a^a)

		int num = constr -> getPartNumber(); 
		for(int i = 0; i < num; i++)
		{
			std::list<Element*> *part = constr -> getPart(i);     //  (a^a^a)
		    Element* result = constr -> getResultConstraint(part);

		    if(result == NULL)
		    	continue;
		    else{
		    	int result_id = result -> left_hand;
		    	TYPE* result_type = result -> righthand.type;

		    	if(result_id != var)
		    		continue;

		    	// now the result id equals the argument variable
		    	if(!CompareType(type, result_type, answer)){ // if the two type is not compitable
		    		continue;
		    	}
		    	else{
		    		if(answer == -1){ // type < result_type. the type is more accurate.
		    			// result feedback operand.
		    		}
		    	}
		    }
		}
	}
	*/
}



void
TypeSolver::subConstraint(Element* item)
{

	Correlation relate = item -> relation;
	ASSERT(relate == Small);

	int left_var = item -> lefthand;
	
	if(item -> mark == 1)                  // variable < variable
	{
		//TODO
	}
	else if(item -> mark == 0)             // variable < type
	{
		//int right_var = item -> righthand.var;
		TYPE* type1 = item -> righthand.type;

		if(upperBound[left_var] == NULL){
			upperBound[left_var] = type1;   // here we need to merge.
		}
		else{
			TYPE* type2 = upperBound[left_var];
			int answer;
			bool ans = CompareType(type1, type2, answer);
			// same: 0, small: -1, big: 1

			if(ans){
				if(answer < 0){   // if type1 is smaller than type2, we got more accurate result.
					upperBound[left_var] = type1;
				}
				else{
					// Do nothing
				}
			}
			else{
				//TODO we need to merge
			}
		}
		// search for result.
	}
	else{
		ASSERT(false);
	} // 


}


void
TypeSolver::processSolving()
{
	ASSERT(!typeContainer.empty());

	std::list<Constraint*>::iterator ptr;
	for(ptr = typeContainer.begin(); ptr != typeContainer.end(); ptr++)
	{
		// typeContainer.insert(typeContainer.end(), (*ptr));
		Constraint* constr = (*ptr);                    // (a^a^a) V (a^a^a)
		int num = constr -> getPartNumber(); 
		for(int i = 0; i < num; i++)
		{
			std::list<Element*> *part = constr -> getPart(i);     //  (a^a^a)
			std::list<Element*>::iterator itr;
			for(itr = part -> begin(); itr != part -> end(); itr++)
			{
				Element* item = (*itr); //a  upper  lower.
				// merge point.        T < T1   a = b

				if(item -> relation == Equal){
					equalConstraint(item);
				}
			}
		}
	}

	/*
	for(ptr = typeContainer.begin(); ptr != typeContainer.end(); ptr++)
	{
		Constraint* constr = (*ptr);                    // (a^a^a) V (a^a^a)
		constr -> Print();
	}
	*/

	for(ptr = typeContainer.begin(); ptr != typeContainer.end(); ptr++)
	{
		// typeContainer.insert(typeContainer.end(), (*ptr));
		Constraint* constr = (*ptr);                    // (a^a^a) V (a^a^a)

		int num = constr -> getPartNumber();
		std::list<Element*> *part = constr -> getPart(0);     //  (a^a^a)
		std::list<Element*>::iterator itr;
		
		for(itr = part -> begin(); itr != part -> end(); itr++)
		{
			Element* item = (*itr); //a
			if(item -> relation == Small){
				subConstraint(item);
			}
		}
	}

	std::map<int, TYPE*>::iterator t;
	for(t = upperBound.begin(); t != upperBound.end(); t++)
	{
		if(t -> second != NULL){
			char buffer[12];
			type2str(t -> second, buffer);
			printf("%d: %s, size: %d\n", t -> first, buffer, (varContainer[t -> first]) -> size);
		}
	}

	printf("equal relation:\n");
	std::map<int, int>::iterator p;
	for(p = equalityPair.begin(); p != equalityPair.end(); p++){
		printf("%d = %d\n", p -> first, p -> second);
	}

}
