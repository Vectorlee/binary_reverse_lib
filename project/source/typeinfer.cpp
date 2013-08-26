
#include "typeinfer.h"
#include <map>
#include <stdio.h>
#include "extract.h"


TypeHunter::TypeHunter(std::map<int, AbstractVariable*> &container)
{
    Register* regPtr = new Register();
    // Tracker and StackTrailer share the Register pointer
    // They can access the register immediately.

    TrackState = new Tracker(container, regPtr);
    trailer = new StackTrailer(regPtr);

    typeContainer.clear();

    InitialType();

    //TrackState -> buildCFG(startAddr, endAddr);
}


TypeHunter::~TypeHunter()
{

    ASSERT(TrackState != NULL);
    delete TrackState;

    ASSERT(trailer != NULL);
    delete trailer;

    //ASSERT(regPtr != NULL);
    //delete regPtr;
}


void
TypeHunter::resetState(Register* reg_file)
{
    //regPtr = reg_file; // we don't need to store the register

    TrackState -> resetEnvironment(reg_file);
    trailer -> resetEnvironment(reg_file);
    // reset the environment.
    return;
}


void 
TypeHunter::addConstraint(Constraint* cont)
{
    typeContainer.insert(typeContainer.end(), cont);
    return;
}


void 
TypeHunter::getResult(std::list<Constraint*> &container, std::map<int, AbstractVariable*> &variable)
{

    std::list<Constraint*>::iterator ptr;
    std::list<Element*>::iterator ets;

    Constraint *Q;

    //std::list<Partial*>::iterator ptr;
    for(ptr = typeContainer.begin(); ptr != typeContainer.end(); ptr++)
    {
        //std::list<Partial*> constrantlist;
        Q = (*ptr);
        //Q -> Print();
        container.insert(container.end(), Q);
    }
    
    TrackState -> getResult(variable);
}


void 
TypeHunter::assignConstraint(xed_decoded_inst_t &xedd)
{
    // assert the initState function has been called.
    ASSERT(TrackState != NULL)
    
	bool trackanswer;
    trackanswer = trailer -> trackESP(xedd);
    ASSERT(trackanswer);

    generateConstraint(xedd);
}


void
TypeHunter::generateConstraint(xed_decoded_inst_t &xedd)
{

	xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);

    xed_reg_enum_t reg1 = DestRegister(xedd);
    xed_reg_enum_t reg2 = SrcRegister(xedd);

    if(reg1 == XED_REG_ESP || reg1 == XED_REG_EBP || 
       reg2 == XED_REG_ESP || reg2 == XED_REG_EBP )
        return;

    // So the TypeHunter is responsible for generating
    // temporary variable, well Tracker only responsible for 
    // track them.

    AbstractVariable* calcResult = NULL;
    int size = 0;

    // we should put the treaking first, if it return true
    // we return directly, 
    if(TrackState -> trackVariable(xedd)){
        return;
    }

	switch(iclass)
	{
        // mov provide no meaningful information about Constraints
        // but there exists instruction like movzx, what should we do .... ?
        case XED_ICLASS_MOV:
        {
            // mem <- reg
            if(IsMemoryOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
                AbstractVariable* var1 = TrackState -> getMemoryVariable(xedd);

                xed_reg_enum_t src_reg = SrcRegister(xedd);
                AbstractVariable* var2 = TrackState -> getRegister(src_reg);

                if(var1 != NULL && var2 != NULL){
                    Constraint* Q = ConstraintMOV(var1, var2);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");                    
                }
            }
            else{
                ASSERT(false);
            }
            break;
        }
        case XED_ICLASS_MOVSX:
        case XED_ICLASS_MOVZX:
        {
            // reg <- r/m
            // question1: size;
            // question2: constraint;
            AbstractVariable* var2 = NULL;
            calcResult = new AbstractVariable();
            calcResult -> region = Temporary;

            xed_reg_enum_t dest_reg = DestRegister(xedd);
            int size1 = RegisterOperandSize(xedd, 0);
            int size2 = 0;

            if(IsMemoryOperand(xedd, 1)){

                var2 = TrackState -> getMemoryVariable(xedd);
                size2 = MemoryOperandSize(xedd, 1);
            }
            else if(IsRegisterOperand(xedd, 1)){

                xed_reg_enum_t src_reg = SrcRegister(xedd);
                var2 = TrackState -> getRegister(src_reg);
                size2 = RegisterOperandSize(xedd, 1);
            }
            else{
                ASSERT(false);
            }

            if(var2 != NULL)
            {
                calcResult -> size = size1 / 8;
                TrackState -> setTempVariable(dest_reg, calcResult);
                
                Constraint *Q = NULL;
                if(iclass == XED_ICLASS_MOVZX)
                    Q = ConstraintMOVX((TYPE*)IntType(size1, false), (TYPE*)IntType(size2, false), calcResult, var2);
                else
                    Q = ConstraintMOVX((TYPE*)IntType(size1, true), (TYPE*)IntType(size2, true), calcResult, var2);

                typeContainer.insert(typeContainer.end(), Q);
            }
            else{
                printf("one of the operand is NULL\n");
            }
            break;
        }
		
        case XED_ICLASS_ADD:
		{
            AbstractVariable* var1 = NULL;
            AbstractVariable* var2 = NULL;
            calcResult = NULL;

            xed_reg_enum_t dest_reg = DestRegister(xedd);
            xed_reg_enum_t src_reg  = SrcRegister(xedd);

            // reg <- reg
            if(IsRegisterOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
                calcResult = new AbstractVariable();      // we put it in to avoid memory leak
                calcResult -> region = Temporary;

            	// if two register operand calculate, the size must be same.
            	size = RegisterOperandSize(xedd, 0);

            	var1 = TrackState -> getRegister(dest_reg);
                var2 = TrackState -> getRegister(src_reg);

                if(var1 != NULL && var2 != NULL)
                {
                    calcResult -> size = size / 8;   // the size of the result.
                    TrackState -> setTempVariable(dest_reg, calcResult);

                    // we set the temporary variable first, so we can get the id of this variable.
                    Constraint *Q = ConstraintADD((TYPE*)NumType(size), var1, var2, calcResult);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }

            }
            // reg <- mem
            else if(IsRegisterOperand(xedd, 0) && IsMemoryOperand(xedd, 1))   //IsMemoryOperand
            {
                // how to extract the proper variable. make it a
                size = RegisterOperandSize(xedd, 0);

                calcResult = new AbstractVariable();
                calcResult -> region = Temporary;

                var1 = TrackState -> getRegister(dest_reg);
                var2 = TrackState -> getMemoryVariable(xedd);

                if(var1 != NULL && var2 != NULL)
                {
                    calcResult -> size = size / 8;
                    TrackState -> setTempVariable(dest_reg, calcResult);
                    // we set the temporary variable first, so we can get the id of this variable.
                    Constraint *Q = ConstraintADD((TYPE*)NumType(size), var1, var2, calcResult);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }
            }
            // reg <- imm
            else if(IsRegisterOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {
                size = RegisterOperandSize(xedd, 0);
                
                calcResult = new AbstractVariable();
                calcResult -> region = Temporary;

                var1 = TrackState -> getRegister(dest_reg);
                var2 = NULL;

                if(var1 != NULL)
                {
                    calcResult -> size = size / 8;
                    TrackState -> setTempVariable(dest_reg, calcResult);                    
                    // without the var2.
                    Constraint *Q = ConstraintADD((TYPE*)NumType(size), var1, NULL, calcResult);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }

            }
            // mem <- reg
            else if(IsMemoryOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
                //add mem reg
                size = RegisterOperandSize(xedd, 1);

                var1 = TrackState -> getMemoryVariable(xedd);
                var2 = TrackState -> getRegister(src_reg);

                if(var1 != NULL && var2 != NULL)
                {
                    Constraint *Q = ConstraintADD((TYPE*)NumType(size), var1, var2, NULL);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }
            }
            // mem <- imm
            else if(IsMemoryOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {
                size = MemoryOperandSize(xedd, 0);

                var1 = TrackState -> getMemoryVariable(xedd);
                var2 = NULL;

                if(var1 != NULL)
                {                    
                    // without the var2.
                    Constraint *Q = ConstraintADD((TYPE*)NumType(size), var1, NULL, NULL);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }
                // no need for temporary variable
            }
            else
            {
            	ASSERT(false);
            }

            TrackState -> setFlagInfo(size, var1, var2, calcResult);
			break;
        }
                
        case XED_ICLASS_SUB:
        {
            AbstractVariable* var1 = NULL;
            AbstractVariable* var2 = NULL;
            calcResult = NULL;

            xed_reg_enum_t dest_reg = DestRegister(xedd);
            xed_reg_enum_t src_reg  = SrcRegister(xedd);

            // reg <- reg
            if(IsRegisterOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
                // if two register operand calculate, the size must be same.
                calcResult = new AbstractVariable();
                calcResult -> region = Temporary;

                size = RegisterOperandSize(xedd, 0);

                var1 = TrackState -> getRegister(dest_reg);
                var2 = TrackState -> getRegister(src_reg);

                if(var1 != NULL && var2 != NULL)
                {
                    calcResult -> size = size / 8;
                    TrackState -> setTempVariable(dest_reg, calcResult);
                    // we set the temporary variable first, so we can get the id of this variable.
                    Constraint *Q = ConstraintSUB((TYPE*)NumType(size), var1, var2, calcResult);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }

            }
            // reg <- mem
            else if(IsRegisterOperand(xedd, 0) && IsMemoryOperand(xedd, 1))   //IsMemoryOperand
            {
                calcResult = new AbstractVariable();
                calcResult -> region = Temporary;

                size = RegisterOperandSize(xedd, 0);

                var1 = TrackState -> getRegister(dest_reg);
                var2 = TrackState -> getMemoryVariable(xedd);

                if(var1 != NULL && var2 != NULL)
                {
                    calcResult -> size = size / 8;
                    TrackState -> setTempVariable(dest_reg, calcResult);
                    // we set the temporary variable first, so we can get the id of this variable.
                    Constraint *Q = ConstraintSUB((TYPE*)NumType(size), var1, var2, calcResult);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }

            }
            // reg <- imm
            else if(IsRegisterOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {
                calcResult = new AbstractVariable();
                calcResult -> region = Temporary;                
                
                size = RegisterOperandSize(xedd, 0);
                
                var1 = TrackState -> getRegister(dest_reg);
                var2 = NULL;

                if(var1 != NULL)
                {
                    calcResult -> size = size / 8;
                    TrackState -> setTempVariable(dest_reg, calcResult);                    
                    // without the var2.
                    Constraint *Q = ConstraintSUB((TYPE*)NumType(size), var1, NULL, calcResult);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }
            }
            // mem <- reg
            else if(IsMemoryOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
                size = RegisterOperandSize(xedd, 1);

                var1 = TrackState -> getMemoryVariable(xedd);
                var2 = TrackState -> getRegister(src_reg);

                if(var1 != NULL && var2 != NULL)
                {
                    Constraint *Q = ConstraintSUB((TYPE*)NumType(size), var1, var2, NULL);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }
                // no temporary variable will be created here.
            }
            // mem <- imm
            else if(IsMemoryOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {
                size = MemoryOperandSize(xedd, 0);

                var1 = TrackState -> getMemoryVariable(xedd);
                var2 = NULL;

                if(var1 != NULL)
                {                    
                    // without the var2.
                    Constraint *Q = ConstraintSUB((TYPE*)NumType(size), var1, NULL, NULL);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }
                // no need for temporary variable
            }
            else
            {
                ASSERT(false);
            }

            // the sub instruction will influence the flag.
            TrackState -> setFlagInfo(size, var1, var2, calcResult);
            break;
        }

        case XED_ICLASS_DEC:
        case XED_ICLASS_INC:
        {
            // we don't generate temporary variable on these two instructions.
            AbstractVariable* var = NULL;
            size = 0;

            if(IsMemoryOperand(xedd, 0)){

                var = TrackState -> getMemoryVariable(xedd);
                size = MemoryOperandSize(xedd, 0);
            }
            else if(IsRegisterOperand(xedd, 0)){

                xed_reg_enum_t reg = DestRegister(xedd);
                var = TrackState -> getRegister(reg);

                size = RegisterOperandSize(xedd, 0);
            }
            else{
                ASSERT(false);
            }

            if(var != NULL){
                // for decrease and increase, it could be number and pointer.
                // can this instruction give us some information about the type of the pointer?
                Constraint *Q = ConstraintDEC_INC((TYPE*)NumType(size), var); // can't tell whether it is signed or not.
                typeContainer.insert(typeContainer.end(), Q);
            }
            else{
                printf("one of the operand is NULL\n");
            }

            TrackState -> setFlagInfo(size, var, NULL, NULL);
            break;
        }

        // next: SETcc
        case XED_ICLASS_SETB:     
        case XED_ICLASS_SETBE:
        case XED_ICLASS_SETNB:    
        case XED_ICLASS_SETNBE: // unsigned  

        case XED_ICLASS_SETL:    
        case XED_ICLASS_SETLE:  
        case XED_ICLASS_SETNL:    
        case XED_ICLASS_SETNLE:
        case XED_ICLASS_SETNS:    
        case XED_ICLASS_SETS:   // signed

        case XED_ICLASS_SETNO:    
        case XED_ICLASS_SETNP:    
        case XED_ICLASS_SETNZ:    
        case XED_ICLASS_SETO:    
        case XED_ICLASS_SETP:    
        case XED_ICLASS_SETZ:   // unknown
        {
            // this instruction will create a temporary variable from no where.
            Constraint *Q = NULL;

            if(IsRegisterOperand(xedd, 0)){

                calcResult = new AbstractVariable();
                calcResult -> region = Temporary;

                xed_reg_enum_t dest_reg = DestRegister(xedd);
                size = RegisterOperandSize(xedd, 0);

                calcResult -> size = size / 8;
                TrackState -> setTempVariable(dest_reg, calcResult);                    
                Q = ConstraintSET((TYPE*)NumType(size), calcResult);

                typeContainer.insert(typeContainer.end(), Q);
            }
            else if(IsMemoryOperand(xedd, 0)){
                
                size = MemoryOperandSize(xedd, 0);
                AbstractVariable* var = TrackState -> getMemoryVariable(xedd);

                if(var != NULL){
                    // no temporary variable
                    Q = ConstraintSET((TYPE*)NumType(size), var);                    
                    typeContainer.insert(typeContainer.end(), Q);
                }
            }
            else{
                ASSERT(false);
            }

            FlagFactor flagInfo = TrackState -> getFlagInfo();
            size = flagInfo.size;

            AbstractVariable* var1 = flagInfo.operandVar1;
            AbstractVariable* var2 = flagInfo.operandVar2;
            AbstractVariable* result = flagInfo.calcResult;

            int mark = -1;
            if(iclass == XED_ICLASS_SETB  || iclass == XED_ICLASS_SETBE  || 
               iclass == XED_ICLASS_SETNB || iclass == XED_ICLASS_SETNBE )
            {
                mark = 0;  // unsigned
            }
            else if(iclass == XED_ICLASS_SETL  || iclass == XED_ICLASS_SETLE || iclass == XED_ICLASS_SETNL || 
                    iclass == XED_ICLASS_SETNL || iclass == XED_ICLASS_SETNS || iclass == XED_ICLASS_SETS  )
            {
                mark = 1;  // signed
            }
            else{
                mark = -1; // unknown
            }

            // the condition instruction can offer some feed back for the variable type.
            if(mark != -1){
                Q = ConditionFeedback((TYPE*)IntType(size, mark), var1, var2, result);
                typeContainer.insert(typeContainer.end(), Q);
            }
            break;
        }

        case XED_ICLASS_CMP:
        {
            AbstractVariable* var1 = NULL;
            AbstractVariable* var2 = NULL;

            xed_reg_enum_t dest_reg = DestRegister(xedd);
            xed_reg_enum_t src_reg  = SrcRegister(xedd);

            // reg <- reg
            if(IsRegisterOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
                // if two register operand calculate, the size must be same.
                size = RegisterOperandSize(xedd, 0);

                var1 = TrackState -> getRegister(dest_reg);
                var2 = TrackState -> getRegister(src_reg);

                if(var1 != NULL && var2 != NULL){

                    Constraint *Q = ConstraintCMP((TYPE*)NumType(size), var1, var2);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }

            }
            // reg <- mem
            else if(IsRegisterOperand(xedd, 0) && IsMemoryOperand(xedd, 1))   //IsMemoryOperand
            {
                // how to extract the proper variable. make it a
                size = RegisterOperandSize(xedd, 0);

                var1 = TrackState -> getRegister(dest_reg);
                var2 = TrackState -> getMemoryVariable(xedd);

                if(var1 != NULL && var2 != NULL){

                    Constraint *Q = ConstraintCMP((TYPE*)NumType(size), var1, var2);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }

            }
            // reg <- imm
            else if(IsRegisterOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {
                size = RegisterOperandSize(xedd, 0);
                
                var1 = TrackState -> getRegister(dest_reg);
                var2 = NULL;

                if(var1 != NULL){

                    // without the var2.
                    Constraint *Q = ConstraintCMP((TYPE*)NumType(size), var1, NULL);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }

            }
            // mem <- reg
            else if(IsMemoryOperand(xedd, 0) && IsRegisterOperand(xedd, 1))
            {
                //add mem reg
                size = RegisterOperandSize(xedd, 1);

                var1 = TrackState -> getMemoryVariable(xedd);
                var2 = TrackState -> getRegister(src_reg);

                if(var1 != NULL && var2 != NULL)
                {
                    Constraint *Q = ConstraintCMP((TYPE*)NumType(size), var1, var2);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }
                // no temporary variable will be created here.
 
            }
            // mem <- imm
            else if(IsMemoryOperand(xedd, 0) && IsImmediateOperand(xedd, 1))
            {
                size = MemoryOperandSize(xedd, 0);

                var1 = TrackState -> getMemoryVariable(xedd);
                var2 = NULL;

                if(var1 != NULL)
                {                    
                    // without the var2.
                    Constraint *Q = ConstraintCMP((TYPE*)NumType(size), var1, NULL);
                    typeContainer.insert(typeContainer.end(), Q);
                }
                else{
                    printf("one of the operand is NULL\n");
                }
            }
            else{
                ASSERT(false);
            }
            // the CMP instruction will influence the flag.
            TrackState -> setFlagInfo(size, var1, var2, NULL);
            break;
        }

        case XED_ICLASS_NEG:     // signed.
        case XED_ICLASS_NOT:     // unsigned 
        {
            // negtive will create a temporary variable.
            AbstractVariable* var = NULL;
            calcResult = NULL;

            Constraint *Q = NULL;

            if(IsRegisterOperand(xedd, 0)){                
                calcResult = new AbstractVariable();    // memory leak.
                calcResult -> region = Temporary;

                xed_reg_enum_t dest_reg = DestRegister(xedd);
                size = RegisterOperandSize(xedd, 0);

                var = TrackState -> getRegister(dest_reg);

                if(var != NULL){

                    calcResult -> size = size / 8;
                    TrackState -> setTempVariable(dest_reg, calcResult);                    
                    if(iclass == XED_ICLASS_NEG)
                        Q = ConstraintREVERSE((TYPE*)IntType(size, true), var, calcResult);
                    else
                        Q = ConstraintREVERSE((TYPE*)IntType(size, false), var, calcResult);

                    typeContainer.insert(typeContainer.end(), Q);
                }

            }
            else if(IsMemoryOperand(xedd, 0)){
                size = MemoryOperandSize(xedd, 0);
                var = TrackState -> getMemoryVariable(xedd);

                if(var != NULL){
                    // no temporary variable
                    if(iclass == XED_ICLASS_NEG)
                        Q = ConstraintREVERSE((TYPE*)IntType(size, true), var, NULL);
                    else
                        Q = ConstraintREVERSE((TYPE*)IntType(size, false), var, NULL);
                                        
                    typeContainer.insert(typeContainer.end(), Q);
                }
            }
            else{
                ASSERT(false);
            }

            TrackState -> setFlagInfo(size, var, NULL, calcResult);
            break;
        }

        case XED_ICLASS_SHL:   // also XED_ICLASS_SAL
        case XED_ICLASS_ROL:
        case XED_ICLASS_ROR:
        case XED_ICLASS_RCL:
        case XED_ICLASS_RCR:
        {
            // the operand is number, and we can't tell whether it is signed or unsigned.
            AbstractVariable* var = NULL;
            calcResult = NULL;

            if(IsRegisterOperand(xedd, 0)){
                calcResult = new AbstractVariable();
                calcResult -> region = Temporary;

                xed_reg_enum_t dest_reg = DestRegister(xedd);
                size = RegisterOperandSize(xedd, 0);

                var = TrackState -> getRegister(dest_reg);

                if(var != NULL){

                    calcResult -> size = size / 8;
                    TrackState -> setTempVariable(dest_reg, calcResult);
                    Constraint *Q = ConstraintSHIFT((TYPE*)NumType(size), var, calcResult);
                    typeContainer.insert(typeContainer.end(), Q);
                }

            }
            else if(IsMemoryOperand(xedd, 0)){

                size = MemoryOperandSize(xedd, 0);
                var = TrackState -> getMemoryVariable(xedd);

                if(var != NULL){
                    // no temporary variable
                    Constraint *Q = ConstraintSHIFT((TYPE*)NumType(size), var, NULL);
                    typeContainer.insert(typeContainer.end(), Q);
                }
            }
            else{
                ASSERT(false);
            }
            TrackState -> setFlagInfo(size, var, NULL, calcResult);

            break;
        }

        // it is signed and unsigned shift
        case XED_ICLASS_SAR:
        case XED_ICLASS_SHR:
        {
            AbstractVariable* var = NULL;
            calcResult = NULL;
            Constraint *Q = NULL;

            if(IsRegisterOperand(xedd, 0)){
                calcResult = new AbstractVariable();
                calcResult -> region = Temporary;

                xed_reg_enum_t dest_reg = DestRegister(xedd);
                size = RegisterOperandSize(xedd, 0);

                var = TrackState -> getRegister(dest_reg);

                if(var != NULL){
                    // shift will also create temp variable
                    calcResult -> size = size / 8;
                    TrackState -> setTempVariable(dest_reg, calcResult);                    
                    if(iclass == XED_ICLASS_SAR)
                        Q = ConstraintSHIFT((TYPE*)IntType(size, true), var, calcResult);
                    else
                        Q = ConstraintSHIFT((TYPE*)IntType(size, false), var, calcResult);

                    typeContainer.insert(typeContainer.end(), Q);
                }

            }
            else if(IsMemoryOperand(xedd, 0)){
                size = MemoryOperandSize(xedd, 0);
                var = TrackState -> getMemoryVariable(xedd);

                if(var != NULL){
                    // no temporary variable
                    if(iclass == XED_ICLASS_SAR)
                        Q = ConstraintSHIFT((TYPE*)IntType(size, true), var, NULL);
                    else
                        Q = ConstraintSHIFT((TYPE*)IntType(size, false), var, NULL);
                                        
                    typeContainer.insert(typeContainer.end(), Q);
                }
            }
            else{
                ASSERT(false);
            }
            TrackState -> setFlagInfo(size, var, NULL, calcResult);

            break;
        }

        case XED_ICLASS_LEA:
        {
            // dest_reg, [ base + index * scale + displacement ]
            // the LEA instruction can be used to implement add.
            calcResult = new AbstractVariable();
            calcResult -> region = Temporary;

            xed_reg_enum_t dest_reg = DestRegister(xedd);
            size = RegisterOperandSize(xedd, 0);

            const xed_operand_values_t* operandPtr = xed_decoded_inst_operands_const(&xedd);

            xed_reg_enum_t   base_reg = xed_operand_values_get_base_reg(operandPtr, 0);
            xed_reg_enum_t  index_reg = xed_operand_values_get_index_reg(operandPtr, 0);

            if(base_reg == XED_REG_ESP || base_reg == XED_REG_EBP)
                break;

            AbstractVariable* var1 = TrackState -> getRegister(base_reg);
            AbstractVariable* var2 = TrackState -> getRegister(index_reg);

            if(var1 != NULL && var2 != NULL){

                TrackState -> setTempVariable(dest_reg, calcResult);
                    // we set the temporary variable first, so we can get the id of this variable.
                Constraint *Q = ConstraintADD((TYPE*)NumType(size), var1, var2, calcResult);
                typeContainer.insert(typeContainer.end(), Q);
            }
            else{

                printf("one of the operand is NULL\n");
            }

            break;
        }


        case XED_ICLASS_DIV:
        case XED_ICLASS_IDIV:
        {
            // idiv r/mem.
            // EDX:EAX / mem = EAX ... EDX
            // create another temporary variable, two temporary variable here.

            calcResult = new AbstractVariable();
            calcResult -> region = Temporary;

            AbstractVariable* remaind = new AbstractVariable();
            remaind -> region = Temporary;

            // big picture !!!
            // the implicit register operand, the second operand, with index 1.
            xed_reg_enum_t reg1 = SrcRegister(xedd);
            AbstractVariable* var1 = TrackState -> getRegister(reg1);

            size = RegisterOperandSize(xedd, 1);
            AbstractVariable* var2 = NULL; //= TrackState -> getMemoryVariable(xedd);

            if(IsMemoryOperand(xedd, 0)){
                var2 = TrackState -> getMemoryVariable(xedd);
            }
            else if(IsRegisterOperand(xedd, 0)){
                xed_reg_enum_t reg2 = DestRegister(xedd);
                var2 = TrackState -> getRegister(reg2);
            }
            else{
                ASSERT(false);
            }

            if(var1 != NULL && var2 != NULL){

                // result al,  remainder ah,
                // result ax,  remainder dx,
                // result eax, remainder edx,
                if(size == 8){

                    calcResult -> size = size / 8;
                    remaind -> size = size / 8;

                    TrackState -> setTempVariable(XED_REG_AL, calcResult);
                    TrackState -> setTempVariable(XED_REG_AH, remaind);
                }
                else if(size == 16){

                    calcResult -> size = size / 8;
                    remaind -> size = size / 8;

                    TrackState -> setTempVariable(XED_REG_AX, calcResult);
                    TrackState -> setTempVariable(XED_REG_DX, remaind);
                }
                else if(size == 32){

                    calcResult -> size = size / 8;
                    remaind -> size = size / 8;

                    TrackState -> setTempVariable(XED_REG_EAX, calcResult);
                    TrackState -> setTempVariable(XED_REG_EDX, remaind);
                }
                else{
                    ASSERT(false);
                }

                Constraint *Q = NULL;
                if(iclass == XED_ICLASS_IDIV){
                    Q = ConstraintDIV((TYPE*)IntType(size, true), var1, var2, calcResult, remaind);
                }
                else{
                    Q = ConstraintDIV((TYPE*)IntType(size, false), var1, var2, calcResult, remaind);
                }
                typeContainer.insert(typeContainer.end(), Q);        
            
            }
            else{
                printf("one of the operand is NULL\n");
            }
            // the div & idiv instruction will not influence the flag.
            break;
        }
        default: break;
	}

    // mov instruction and xchg something will change the location of the variable.
    // we must integrate the Tracker with the TypeHunter




    return;
}