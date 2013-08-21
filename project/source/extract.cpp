
#include "extract.h"
#include <stdio.h>
#include <list>


// thing will be pretty hard, because some times, there is no result.
// and the thing has just mix up.
Constraint* 
ConstraintADD(TYPE* type, AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult)
{
    // the var2 and calcResult can be NULL
    // but the var1 can't be NULL
    ASSERT(var1 != NULL);
    Constraint *Q = new Constraint();
    
    Partial* P = new std::list<Element*>(); 
    Element *cons1, *cons2, *cons3;

    // num + num
    // operand var1
    cons1 = new Element();
    cons1 -> subTypeSet(var1 -> id, type, 0);  // operand
    P -> insert( P -> end(), cons1);

    // operand var2, if the second variable doesn't exists, it is a immediate number.
    if(var2 != NULL){
        cons2 = new Element();
        cons2 -> subTypeSet(var2 -> id, type, 0);  // operand
        P -> insert( P -> end(), cons2);
    }

    // result, if the result doesn't exists, the destination is a memory slot.  
    if(calcResult != NULL){
        cons3 = new Element();
        cons3 -> subTypeSet(calcResult -> id, type, 1);  // result
        P -> insert( P -> end(), cons3);
    }

    Q -> addPart(P);

    if(type -> getSize() == 32){

        // pointer + num
        P = new std::list<Element*>();

        cons1 = new Element();
        cons1 -> subTypeSet(var1 -> id, (TYPE*)Ptr_s, 0);
        P -> insert( P -> end(), cons1);

        if(var2 != NULL){
            cons2 = new Element();
            cons2 -> subTypeSet(var2 -> id, (TYPE*)NumType(32), 0);
            P -> insert( P -> end(), cons2);
        }

        if(calcResult != NULL){
            cons3 = new Element();
            cons3 -> subTypeSet(calcResult -> id, (TYPE*)Ptr_s, 1);
            P -> insert( P -> end(), cons3);
        }

        Q -> addPart(P);

        // num + pointer, the var2 can't be null 
        if(var2 != NULL)
        {
            P = new std::list<Element*>();

            cons1 = new Element();
            cons1 -> subTypeSet(var2 -> id, (TYPE*)Ptr_s, 0);
            P -> insert( P -> end(), cons1);

            cons2 = new Element();
            cons2 -> subTypeSet(var1 -> id, (TYPE*)NumType(32), 0);
            P -> insert( P -> end(), cons2);

            if(calcResult != NULL){
                cons3 = new Element();
                cons3 -> subTypeSet(calcResult -> id, (TYPE*)Ptr_s, 1);
                P -> insert( P -> end(), cons3);
            }

            Q -> addPart(P);
        }
    }

    return Q;
}


Constraint* 
ConstraintSUB(TYPE* type, AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult)
{
    ASSERT(var1 != NULL);
    Constraint *Q = new Constraint();
    
    Partial* P = new std::list<Element*>(); 
    Element *cons1, *cons2, *cons3;

    // num - num
    // operand var1
    cons1 = new Element();
    cons1 -> subTypeSet(var1 -> id, type, 0);  // operand
    P -> insert( P -> end(), cons1);

    // operand var2, if the second variable doesn't exists, it is a immediate number.
    if(var2 != NULL){
        cons2 = new Element();
        cons2 -> subTypeSet(var2 -> id, type, 0);  // operand
        P -> insert( P -> end(), cons2);
    }

    // result, if the result doesn't exists, the destination is a memory slot.  
    if(calcResult != NULL){
        cons3 = new Element();
        cons3 -> subTypeSet(calcResult -> id, type, 1);  // result
        P -> insert( P -> end(), cons3);
    }
    Q -> addPart(P);


    if(type -> getSize() == 32){

        // pointer - num
        P = new std::list<Element*>();

        cons1 = new Element();
        cons1 -> subTypeSet(var1 -> id, (TYPE*)Ptr_s, 0);
        P -> insert(P -> end(), cons1);

        if(var2 != NULL){
            cons2 = new Element();
            cons2 -> subTypeSet(var2 -> id, (TYPE*)NumType(32), 0);
            P -> insert(P -> end(), cons2);
        }

        if(calcResult != NULL){
            cons3 = new Element();
            cons3 -> subTypeSet(calcResult -> id, (TYPE*)Ptr_s, 1);
            P -> insert(P -> end(), cons3);
        }

        Q -> addPart(P);

        // pointer - pointer, the var2 can't be null 
        if(var2 != NULL)
        {
            P = new std::list<Element*>();

            cons1 = new Element();
            cons1 -> subTypeSet(var2 -> id, (TYPE*)Ptr_s, 0);
            P -> insert( P -> end(), cons1);

            cons2 = new Element();
            cons2 -> subTypeSet(var1 -> id, (TYPE*)Ptr_s, 0);
            P -> insert( P -> end(), cons2);

            if(calcResult != NULL){
                cons3 = new Element();
                cons3 -> subTypeSet(calcResult -> id, (TYPE*)NumType(32), 1);
                P -> insert( P -> end(), cons3);
            } // the type of the result will change!

            Q -> addPart(P);
        }
    }
    return Q;
}


Constraint* 
ConstraintDIV(TYPE* type, AbstractVariable *var1, AbstractVariable* var2, AbstractVariable* calcResult, AbstractVariable* remaind)
{
	ASSERT(calcResult -> id != 0);
    ASSERT(remaind -> id != 0);
    ASSERT(var1 != NULL);

    Constraint *Q = new Constraint();

    Partial* P = new std::list<Element*>(); 
    Element *cons1, *cons2, *cons3, *cons4;

    // num / num.
    cons1 = new Element();
    cons1 -> subTypeSet(var1 -> id, type, 0);
    P -> insert( P -> end(), cons1);

    if(var2 != NULL){
        cons2 = new Element();
        cons2 -> subTypeSet(var2 -> id, type, 0);
        P -> insert( P -> end(), cons2);
    }
    
    cons3 = new Element();
    cons3 -> subTypeSet(calcResult -> id, type, 1);
    P -> insert( P -> end(), cons3);

    Q -> addPart(P);


    // num % num
    cons1 = new Element();
    cons1 -> subTypeSet(var1 -> id, type, 0);
    P -> insert( P -> end(), cons1);

    if(var2 != NULL){
        cons2 = new Element();
        cons2 -> subTypeSet(var2 -> id, type, 0);
        P -> insert( P -> end(), cons2);
    }

    cons4 = new Element();
    cons4 -> subTypeSet(remaind -> id, type, 1);
    P -> insert( P -> end(), cons4);

    Q -> addPart(P);
    return Q;
}


Constraint* 
ConstraintDEC_INC(TYPE* type, AbstractVariable *var1)
{

    ASSERT(var1 != NULL)
    Constraint *Q = new Constraint();

    Partial* P = new std::list<Element*>(); 
    Element *cons1;

    // num++ --.
    cons1 = new Element();
    cons1 -> subTypeSet(var1 -> id, type, 0);

    P -> insert(P -> end(), cons1);
    Q -> addPart(P);

    if(type -> getSize() == 32){
        
        P = new std::list<Element*>();

        cons1 = new Element();
        cons1 -> subTypeSet(var1 -> id, (TYPE*)Ptr_s, 0);

        // I don't know whether the DEC, INC can provide us some precious information 
        // about the type of the pointer.
        P -> insert( P -> end(), cons1);
        Q -> addPart(P);    	
    }

    return Q;
}



Constraint* 
ConstraintCMP(TYPE* type, AbstractVariable *var1, AbstractVariable* var2)
{
    ASSERT(var1 != NULL);

    Constraint *Q = new Constraint();
                    
    Partial* P = new std::list<Element*>(); 
    Element *cons1, *cons2;

    // num compared with num
    cons1 = new Element();
    cons1 -> subTypeSet(var1 -> id, type, 0);
    P -> insert( P -> end(), cons1);

    if(var2 != NULL){
        cons2 = new Element();
        cons2 -> subTypeSet(var2 -> id, type, 0);
        P -> insert( P -> end(), cons2);
    }

    Q -> addPart(P);

    if(type -> getSize() == 32 && var2 != NULL){

        // pointer compared with pointer
        P = new std::list<Element*>();

        cons1 = new Element();
        cons1 -> subTypeSet(var2 -> id, (TYPE*)Ptr_s, 0);
        P -> insert( P -> end(), cons1);

        cons2 = new Element();
        cons2 -> subTypeSet(var1 -> id, (TYPE*)Ptr_s, 0); 
        P -> insert( P -> end(), cons2);

        Q -> addPart(P);
    }

    return Q;
}


Constraint* 
ConstraintSHIFT(TYPE* type, AbstractVariable *var1, AbstractVariable *calcResult)
{

    ASSERT(calcResult -> id != 0);

    Constraint *Q = new Constraint();

    Partial* P = new std::list<Element*>(); 
    Element *cons1, *cons2;

    // num >> n.
    // the shift instruction can release time information.
    cons1 = new Element();
    cons1 -> subTypeSet(var1 -> id, type, 0);
    P -> insert( P -> end(), cons1);

    if(calcResult != NULL){
        cons2 = new Element();
        cons2 -> subTypeSet(calcResult -> id, type, 0);
        P -> insert( P -> end(), cons2);
    }

    Q -> addPart(P);

    return Q;
}


Constraint* 
ConstraintREVERSE(TYPE* type, AbstractVariable *var1, AbstractVariable *calcResult)
{
    return ConstraintSHIFT(type, var1, calcResult);
}


Constraint* 
ConstraintSET(TYPE* type, AbstractVariable *calcResult)
{
    //return ConstraintCMP(calcResult, type);
    Constraint *Q = new Constraint();
                    
    Partial* P = new std::list<Element*>(); 
    Element *cons1;

    // num compared with num
    cons1 = new Element();
    cons1 -> subTypeSet(calcResult -> id, type, 0);

    P -> insert( P -> end(), cons1);

    // pointer will not compared with a number.
    Q -> addPart(P);
    return Q;
}


Constraint* 
ConstraintMOV(AbstractVariable *var1, AbstractVariable *var2)
{
    ASSERT(var1 != NULL && var2 != NULL);
    // only for mem <- reg

    Constraint *Q = new Constraint();
    
    Partial* P = new std::list<Element*>(); 
    Element *cons1;

    // operand var1
    cons1 = new Element();
    cons1 -> equalTypeSet(var1 -> id, var2 -> id, 0);  // operand
    P -> insert( P -> end(), cons1);

    Q -> addPart(P);

    return Q;
}

Constraint* 
ConstraintMOVX(TYPE* type1, TYPE* type2, AbstractVariable *var1, AbstractVariable *var2)
{
    ASSERT(var1 != NULL && var2 != NULL);

    Constraint *Q = new Constraint();
    
    Partial* P = new std::list<Element*>(); 
    Element *cons1, *cons2;

    cons2 = new Element();
    cons2 -> subTypeSet(var2 -> id, type2, 0);  // operand
    P -> insert( P -> end(), cons2);

    // operand var1
    cons1 = new Element();
    cons1 -> subTypeSet(var1 -> id, type1, 1);  // result
    P -> insert( P -> end(), cons1);

    Q -> addPart(P);

    return Q;
}



Constraint*
ConditionFeedback(TYPE* type, AbstractVariable *var1, AbstractVariable *var2, AbstractVariable* calcResult)
{
    // the var2 and calcResult can be NULL
    // but the var1 can't be NULL
    ASSERT(var1 != NULL);
    Constraint *Q = new Constraint();
    
    Partial* P = new std::list<Element*>(); 
    Element *cons1, *cons2, *cons3;

    // operand var1
    cons1 = new Element();
    cons1 -> subTypeSet(var1 -> id, type, 0);  // operand
    P -> insert( P -> end(), cons1);

    // operand var2, if the second variable doesn't exists, it is a immediate number.
    if(var2 != NULL){
        cons2 = new Element();
        cons2 -> subTypeSet(var2 -> id, type, 0);  // operand
        P -> insert( P -> end(), cons2);
    }

    // result, if the result doesn't exists, the destination is a memory slot.  
    if(calcResult != NULL){
        cons3 = new Element();
        cons3 -> subTypeSet(calcResult -> id, type, 1);  // result
        P -> insert( P -> end(), cons3);
    }

    Q -> addPart(P);
    return Q;
}




