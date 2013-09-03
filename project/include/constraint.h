
#ifndef CONSTRAINT_H
#define CONSTRAINT_H


#include <list>
#include "utility.h"


enum TypeName { REGISTER, NUMBER, INTEGER, POINTER, ALL };


// this like a void
class TYPE   
{
protected:
	int level;
	int size;
	TypeName name;

public:
	TYPE(){
		size = 0;
		name = ALL;
		level = 4;
	}

	int getSize()   { return size; }
	TypeName getName()   { return name; }   

	int getLevel()  { return level; }
};


class REG_T: public TYPE
{
	// reg32, reg16, reg8

public:
	REG_T(int num){
		size = num;
		name = REGISTER;
		level = 1;

	}

};

class NUM_T: public TYPE
{
	// num32, num16, num8
public:
	NUM_T(int num){
		size = num;
		name = NUMBER;
		level = 2;

	}

};

class INT_T: public TYPE
{
	// int32, uint32, int16, uint16, int8, uint8 
private:
	bool sign;

public:

	INT_T(int num, bool mark){
		size = num;
		sign = mark;
		name = INTEGER;

		level = 3;

	}

	bool isSigned() { return sign; }
	// true is signed int
	// false is unsigned int.

};

class PTR_T: public TYPE
{
	// for pointer, the size is redundent.
private:
	AbstractVariable *contain;

public:

	PTR_T(){
		size = 32;
		name = POINTER;

		level = 2;

		contain = NULL;     //= new TYPE();   // assume the new pointers are all (void*)
	}

	void setContain(AbstractVariable* var){
		contain = var;
	}

	AbstractVariable* getContain(){
		return contain; // this could be NULL.
	}

};

// predefined type:
// global used
// enum TypeHierarchy { All, Int8, Uint8, Int16, Uint16, Int32, Uint32, Num8, Num16, Num32, Ptr_s, Reg8, Reg16, Reg32, Unknown };

extern TYPE *All;
extern REG_T *Reg32, *Reg16, *Reg8;
extern NUM_T *Num32, *Num16, *Num8;
extern INT_T *Int32, *Int16, *Int8, *Uint32, *Uint16, *Uint8; 
extern PTR_T *Ptr_s;


REG_T *RegType(int size);
NUM_T *NumType(int size);
INT_T *IntType(int size, bool sign);

//
// Initialize the predefined types
//
void InitialType();

// Compare two types, get the relation between two type
// one is another's subtype, or it is uncomparable.
//
bool CompareType(TYPE *a, TYPE *b, int &answer);

// Calculate the Meet or Join of the type.
// Since the Type Hierarchy is a lattice structure, 
// we can get the lowest upper bound and biggest lower bound
// of two types. 
//
TYPE* TypeMeet(TYPE *a, TYPE *b);

TYPE* TypeJoin(TYPE *a, TYPE *b);



enum Correlation { Small, Large, Equal }; 

const char* correlation2str(Correlation p);

bool type2str(TYPE* type, char *buffer);



// Define the element of the Constraints.
// int (id of the variable)  < int 
//                           < type()
//

class Element
{
public:
	Correlation relation;  // <, =, >

	int lefthand;          // the left hand is always variable,  the id of the variable.

	union{
		int   var;
		TYPE* type;
	} righthand;

	int mark;              // 0 is type,    1 is variable.
	int category;          // 0 is operand, 1 is result.

	// var <  ...
	void subTypeSet(int var1, TYPE* var2, int cate);
	void subTypeSet(int var1, int var2, int cate);

	// var == ...
	void equalTypeSet(int var1, int var2, int cate);
	void equalTypeSet(int var1, TYPE* var2, int cate);

	void Print();
};


//  (a ^ a) V (a ^ a ^ a) V (a ^ a ^ a)
//   a:     & < &,  & = &

typedef std::list<Element*> Partial;     //Partial < a ^ a ^ a>

// we need to set apart the result of the instruction.
// option1: mark in the abstract variable structure.
// option2: mark in the constraint variable.
// I think the option 1 is more attractive.

// no!, when we store the constraints, we only use the id of the variable.
// without the AbstractVariable structure

// we need to mark in the structure Element.

// we provide good api to access the result and operand.

class Constraint
{

private:
	int number;
	std::list<Partial*> constraintList;

public:

	Constraint(){
		number = 0;
		constraintList.clear();
	}

	void addPart(Partial *value);

	int getPartNumber();

	Partial* getPart(int index);

	Element* getResultConstraint(Partial* part);
	// the result is stored in the argument
	void getOperandConstraint(Partial* part);

	void Print();

};


#endif


