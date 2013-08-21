


#include "trailer.h"
#include <stdio.h>

bool 
StackTrailer::trackESP(xed_decoded_inst_t &xedd)
{
	//xed_category_enum_t category = xed_decoded_inst_get_category(&xedd);
	xed_iclass_enum_t iclass = xed_decoded_inst_get_iclass(&xedd);  //get the class of the instructions

	xed_reg_enum_t src_reg  = SrcRegister(xedd);
	xed_reg_enum_t dest_reg = DestRegister(xedd);
	
	switch(iclass)
	{
		case XED_ICLASS_PUSH: regPtr -> addESP(1);  break;    // we only deal with push, pop, add, sub, enter, leave, mov. ret
		case XED_ICLASS_POP:  regPtr -> subESP(1);  break;    // the operation that can affect the value of esp is just them.
		case XED_ICLASS_MOV:                                  // sometimes, for align purpers, there could be AND operation one ESP  
		{                                                     // but we can simply ignore it, we can still assume a distance between ebp 
			if(dest_reg == XED_REG_EBP)                       // and esp, it works all well.
			{
				if(src_reg == XED_REG_ESP){
					regPtr -> setESP();
				}
				else
					return false;
			}
			if(dest_reg == XED_REG_ESP)
				return false;

			break;
		}
		case XED_ICLASS_ADD:
		{
			int mark = 0;
			if(dest_reg == XED_REG_ESP)
			{
				xed_int64_t imm = GetImmediate(xedd, mark);
				if(mark == 0)
					return false;
				else{
					regPtr -> subESP((int)imm);    // add is sub in our case
				}
			}

			break;
		}
		case XED_ICLASS_SUB:
		{
			int mark = 0;
			if(dest_reg == XED_REG_ESP)
			{
				xed_int64_t imm = GetImmediate(xedd, mark);
				if(mark == 0)
					return false;
				else{
					regPtr -> addESP((int)imm);    // add is sub in our case
				}
			}

			break;
		}
		case XED_ICLASS_ENTER:
		case XED_ICLASS_LEAVE:
		{
			regPtr -> setESP();
			break;
		}

		default: break;
	}

	return true;
}