#include <string>
#include "const.hpp"

bool legal[64], legal_r[64];
std::string inst_str_r[64], inst_str[64], str_nop;

void init_str_const()
{
	/* This function initialize inst_str with corresponding
	   instructions' name. These names are for debugging. */
	for (int i=0;i<64;i++) {
		inst_str_r[i] = inst_str[i] = "undefined";
	}
	
	/* R-format */
	inst_str_r[ADD]="ADD";
	inst_str_r[ADDU]="ADDU";
	inst_str_r[SUB]="SUB";
	inst_str_r[AND]="AND";
	inst_str_r[OR]="OR";
	inst_str_r[XOR]="XOR";
	inst_str_r[NOR]="NOR";
	inst_str_r[NAND]="NAND";
	inst_str_r[SLT]="SLT";
	inst_str_r[SLL]="SLL";
    inst_str_r[SRL]="SRL";
    inst_str_r[SRA]="SRA";
    inst_str_r[JR]="JR";
    inst_str_r[MULT]="MULT";
    inst_str_r[MULTU]="MULTU";
    inst_str_r[MFHI]="MFHI";
    inst_str_r[MFLO]="MFLO";
    
    /* Others */
    inst_str[ADDI]="ADDI";
    inst_str[ADDIU]="ADDIU";
    inst_str[LW]="LW";
    inst_str[LH]="LH";
    inst_str[LHU]="LHU";
    inst_str[LB]="LB";
    inst_str[LBU]="LBU";
    inst_str[SW]="SW";
    inst_str[SH]="SH";
    inst_str[SB]="SB";
    inst_str[LUI]="LUI";
    inst_str[ANDI]="ANDI";
    inst_str[ORI]="ORI";
    inst_str[NORI]="NORI";
    inst_str[SLTI]="SLTI";
    inst_str[BEQ]="BEQ";
    inst_str[BNE]="BNE";
    inst_str[BGTZ]="BGTZ";
    inst_str[J]="J";
    inst_str[JAL]="JAL";
    inst_str[HALT]="HALT";
    
    str_nop="NOP";
}

void init_const()
{
	/* Init bool legal */
	/* R-format */
	legal_r[ADD]=true;
	legal_r[ADDU]=true;
	legal_r[SUB]=true;
	legal_r[AND]=true;
	legal_r[OR]=true;
	legal_r[XOR]=true;
	legal_r[NOR]=true;
	legal_r[NAND]=true;
	legal_r[SLT]=true;
	legal_r[SLL]=true;
    legal_r[SRL]=true;
    legal_r[SRA]=true;
    legal_r[JR]=true;
    legal_r[MULT]=true;
    legal_r[MULTU]=true;
    legal_r[MFHI]=true;
    legal_r[MFLO]=true;
    
    /* Others */
    legal[ADDI]=true;
    legal[ADDIU]=true;
    legal[LW]=true;
    legal[LH]=true;
    legal[LHU]=true;
    legal[LB]=true;
    legal[LBU]=true;
    legal[SW]=true;
    legal[SH]=true;
    legal[SB]=true;
    legal[LUI]=true;
    legal[ANDI]=true;
    legal[ORI]=true;
    legal[NORI]=true;
    legal[SLTI]=true;
    legal[BEQ]=true;
    legal[BNE]=true;
    legal[BGTZ]=true;
    legal[J]=true;
    legal[JAL]=true;
    legal[HALT]=true;
}
