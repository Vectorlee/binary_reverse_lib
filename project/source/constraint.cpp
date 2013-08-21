
#include "constraint.h"
#include "utility.h"
#include <string.h>


TYPE  *All;
REG_T *Reg32, *Reg16, *Reg8;
NUM_T *Num32, *Num16, *Num8;
INT_T *Int32, *Int16, *Int8, *Uint32, *Uint16, *Uint8; 
PTR_T *Ptr_s;


REG_T *RegType(int size)
{
	switch(size){
		case 8:  return Reg8;  break;
		case 16: return Reg16; break;
		case 32: return Reg32; break;
		default: ASSERT(false); break;
	}
}

NUM_T *NumType(int size)
{
	switch(size){
		case 8:  return Num8;  break;
		case 16: return Num16; break;
		case 32: return Num32; break;
		default: ASSERT(false); break;
	}	
}

INT_T *IntType(int size, bool sign)
{
	if(sign){
		switch(size){
			case 8:  return Int8;  break;
			case 16: return Int16; break;
			case 32: return Int32; break;
			default: ASSERT(false); break;
		}
	}
	else{
		switch(size){
			case 8:  return Uint8;  break;
			case 16: return Uint16; break;
			case 32: return Uint32; break;
			default: ASSERT(false); break;
		}
	}

}


// Initialize the predefined types
//
void InitialType()
{

	All = new TYPE();

	Reg8 = new REG_T(8); Reg16 = new REG_T(16); Reg32 = new REG_T(32);
	Num8 = new NUM_T(8); Num16 = new NUM_T(16); Num32 = new NUM_T(32);

	Int8 = new INT_T(8, true);  Int16 = new INT_T(16, true); Int32 = new INT_T(32, true);
	Uint8 = new INT_T(8, false); Uint16 = new INT_T(16, false); Uint32 = new INT_T(32, false); 

	Ptr_s = new PTR_T(); // this like a void pointer.
}


//
// Compare two types, get the relation between two type
// one is another's subtype, or it is uncomparable.
//
bool CompareType(TYPE *a, TYPE *b, int &answer)
{
	// if it is simple kind of type, the size determines the relation.
	if(a -> getName() == b -> getName()){

		//if(a -> getName == POINTER){
		//	return CompareType((PTR_T)a -> getType(), (PTR_T)b -> getType(), answer);
		//  just assume that pointer doesn't involve this function.
		//}

		answer = (a -> getSize()) - (b -> getSize());
		return true;
	}
	else{     // the name is different.

		if(a -> getName() == ALL){         // a < b
			answer = -1;
			return true;
		}
		else if(b -> getName() == ALL){    // b < a
			answer = 1;
			return true;
		}
		else{
			if(a -> getSize() != b -> getSize())  // if the size is different.
				return false;                  // the two type can not compatible.

			answer = 0 - ((a -> getLevel()) - (b -> getLevel()));
			return true;
		}
	}
}

//
// Calculate the Meet or Join of the type.
// Since the Type Hierarchy is a lattice structure, 
// we can get the lowest upper bound and biggest lower bound
// of two types. 
//
TYPE* TypeMeet(TYPE *a, TYPE *b)
{
	// just assume the pointer doesn't involve.
	return NULL;
}

TYPE* TypeJoin(TYPE *a, TYPE *b)
{
	return NULL;
}



const char* correlation2str(Correlation p)
{

	switch(p)
	{
		case Small: return "<"; break;
		case Large: return ">"; break;
		case Equal: return "="; break;
		default: break;
	}
	// can't reach here.
	return "&&";
}

bool type2str(TYPE* type, char *buffer)
{

	if(type -> getName() == NUMBER)
		strcpy(buffer, "num");
	if(type -> getName() == INTEGER){

		if( ((INT_T*)type) -> isSigned())
			strcpy(buffer, "int");
		else
			strcpy(buffer, "uint");
	}

	if(type -> getName() == POINTER){
		strcpy(buffer, "ptr");
	}

	int size = type -> getSize();
	
	char number[4];
	sprintf(number, "%d", size);
	strcat(buffer, number);

	return true;
}


//----------------------------------------
// class Element
//

void 
Element::subTypeSet(int var1, TYPE* var2, int cate){
	relation = Small;

	lefthand = var1;
	righthand.type = var2;

	mark = 0;
	category = cate;
}

void 
Element::subTypeSet(int var1, int var2, int cate){
	relation = Small;

	lefthand = var1;
	righthand.var = var2;

	mark = 1;
	category = cate;
}

void 
Element::equalTypeSet(int var1, TYPE* var2, int cate)
{
	relation = Equal;

	lefthand = var1;
	righthand.type = var2;

	mark = 0;
	category = cate;
}


void 
Element::equalTypeSet(int var1, int var2, int cate)
{
	relation = Equal;

	lefthand = var1;
	righthand.var = var2;

	mark = 1;
	category = cate;
}


void
Element::Print()
{

	if(mark == 0){ // Type
		char buffer[12];
		type2str(righthand.type, buffer);

		printf("%d %s %s", lefthand, correlation2str(relation), buffer);
	}
	else{ // var
		printf("%d %s %d", lefthand, correlation2str(relation), righthand.var);
	}
}


//----------------------------------------
// class Constraint
//

void
Constraint::addPart(Partial *value)
{
	number++;
	constraintList.insert(constraintList.end(), value);
}


int 
Constraint::getPartNumber()
{
	return number;
}

Partial*
Constraint::getPart(int index)
{
	//	std::list<Partial*> constraintList;
	ASSERT(index >= 0 && index < number );
	
	std::list<Partial*>::iterator ptr;
	int i = 0;

	for(ptr = constraintList.begin(); ptr != constraintList.end(); ptr++)
	{
		if(i == index)
			return (*ptr);

		i++;
	}

	return NULL;
}

Element* 
Constraint::getResultConstraint(Partial* part)
{
	//typedef std::list<Element*> Partial;
	ASSERT(part != NULL);

	// get the last element.
	Element* item = part -> back();

	if(item -> category == 1) // result
		return item;
	else
		return NULL;
}


void 
Constraint::getOperandConstraint(Partial* part)
{

	ASSERT(part != NULL);

	// get the last element.
	Element* item = part -> back();

	if(item -> category == 1) // result, delete the last one.
		part -> pop_back();
	
	//else, remain unchanged
}


void 
Constraint::Print()
{

	std::list<Partial*>::iterator ptr = constraintList.begin();

	while(ptr != constraintList.end() )
	{
		std::list<Element*> *part = (*ptr);
		std::list<Element*>::iterator itr = part -> begin();

		printf("(");
		while(itr != part -> end())
		{
			Element* item = (*itr);
			item -> Print();

			itr++;
			if(itr != part -> end())
				printf(" V ");
		}

		ptr++;
		if(ptr != constraintList.end())
			printf(") ^ ");
		else
			printf(")");
	}

	printf("\n");
}


