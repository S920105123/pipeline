#include "error.hpp"
#include "datapath.hpp"
#include "ALU.hpp"
#include "loader.hpp" 
#include <climits>
#include <queue>

extern bool stop_simulate;

/* Convesion between big/little endian */
extern int btol(int target);
extern short h_btol(short target);

bool hilo_used=false;

/* Function pointers */
int (*R_func[64])(int rs, int rt, int shamt);
int (*func[64])(int rs, int immediate);

/* R-format instructions */
int inst_add(int rs, int rt, int shamt) {
	if ((rs>0&&rt>0&&rs>INT_MAX-rt) ||
	    (rs<0&&rt<0&&rs<INT_MIN-rt)) {
			//std::cerr<<"ovf\n";
			error(NUM_OVF);
	}
	
	return rs+rt;
}
int inst_addu(int rs, int rt, int shamt) {
	return rs+rt;
}
int inst_sub(int rs, int rt, int shamt) {
	if ((rs>0&&-rt>0&&rs>INT_MAX+rt) ||
	    (rs<0&&-rt<0&&rs<INT_MIN+rt)) {
			error(NUM_OVF);
	}

	return rs-rt;
}
int inst_and(int rs, int rt, int shamt) {
	return rs&rt;
}
int inst_or(int rs, int rt, int shamt) {
	return rs|rt;
}
int inst_xor(int rs, int rt, int shamt) {
	return rs^rt;
}
int inst_nor(int rs, int rt, int shamt) {
	return ~(rs|rt);
}
int inst_nand(int rs, int rt, int shamt) {
	return ~(rs&rt);
}
int inst_slt(int rs, int rt, int shamt) {
	return rs<rt;
}
int inst_sll(int rs, int rt, int shamt) {
	return rt<<shamt;
}
int inst_srl(int rs, int rt, int shamt) {
	int temp;
	if (shamt!=0) {
		temp=rt>>1;
		temp=temp&0x7FFFFFFF;
		temp=temp>>(shamt-1);
		return temp;
	} else {
		return rt;
	}
}
int inst_sra(int rs, int rt, int shamt) {
	return rt>>shamt;
}
int inst_mult(int rs, int rt, int shamt) {
	long long res=rs;
	if (hilo_used) {
		error(OVERWRITE_HILO);
	} else {
		hilo_used=true;
	}
	res=res*rt;
	reg[HI]=res>>32;
	reg[LO]=res&0x00000000FFFFFFFF;
	change.push(HI);
	change.push(LO);
	return reg[LO];
}
int inst_multu(int rs, int rt, int shamt) {
	unsigned long long res=(unsigned int)rs;
	res=res*(unsigned int)rt;
	if (hilo_used) {
		error(OVERWRITE_HILO);
	} else {
		hilo_used=true;
	}
	reg[HI]=res>>32;
	reg[LO]=res&(long long)0x00000000FFFFFFFF;
	change.push(HI);
	change.push(LO);
	return reg[LO];
}
int inst_mfhi(int rs, int rt, int shamt) {
	hilo_used=false;
	return pre_reg[HI];
}
int inst_mflo(int rs, int rt, int shamt) {
	hilo_used=false;
	return pre_reg[LO];
}

/* I-format instructions */
int inst_addi(int rs, int immediate) {
	if ((rs>0&&immediate>0&&rs>INT_MAX-immediate) ||
	    (rs<0&&immediate<0&&rs<INT_MIN-immediate)) {
			error(NUM_OVF);
	}
	return rs+immediate;
}
int inst_addiu(int rs, int immediate) {
	return rs+immediate;
}
int inst_lw(int rs, int immediate) {
	if ((rs>0&&immediate>0&&rs>INT_MAX-immediate) ||
	    (rs<0&&immediate<0&&rs<INT_MIN-immediate)) {
		error(NUM_OVF);
	}
	return rs+immediate;
}
int inst_lh(int rs, int immediate) {
	if ((rs>0&&immediate>0&&rs>INT_MAX-immediate) ||
	    (rs<0&&immediate<0&&rs<INT_MIN-immediate)) {
		error(NUM_OVF);
	}
	return rs+immediate;
}
int inst_lhu(int rs, int immediate) {
	if ((rs>0&&immediate>0&&rs>INT_MAX-immediate) ||
	    (rs<0&&immediate<0&&rs<INT_MIN-immediate)) {
		error(NUM_OVF);
	}
	return rs+immediate;
}
int inst_lb(int rs, int immediate) {
	if ((rs>0&&immediate>0&&rs>INT_MAX-immediate) ||
	    (rs<0&&immediate<0&&rs<INT_MIN-immediate)) {
		error(NUM_OVF);
	}
	return rs+immediate;
}
int inst_lbu(int rs, int immediate) {
	if ((rs>0&&immediate>0&&rs>INT_MAX-immediate) ||
	    (rs<0&&immediate<0&&rs<INT_MIN-immediate)) {
		error(NUM_OVF);
	}
	return rs+immediate;
}
int inst_sw(int rs, int immediate) {
	if ((rs>0&&immediate>0&&rs>INT_MAX-immediate) ||
	    (rs<0&&immediate<0&&rs<INT_MIN-immediate)) {
		error(NUM_OVF);
	}
	return rs+immediate;
}
int inst_sh(int rs, int immediate) {
	if ((rs>0&&immediate>0&&rs>INT_MAX-immediate) ||
	    (rs<0&&immediate<0&&rs<INT_MIN-immediate)) {
		error(NUM_OVF);
	}
	return rs+immediate;
}
int inst_sb(int rs, int immediate) {
	if ((rs>0&&immediate>0&&rs>INT_MAX-immediate) ||
	    (rs<0&&immediate<0&&rs<INT_MIN-immediate)) {
		error(NUM_OVF);
	}
	return rs+immediate;
}
int inst_lui(int rs, int immediate) {
	return immediate<<16;
}
int inst_andi(int rs, int immediate) {
	return rs&immediate;
}
int inst_ori(int rs, int immediate) {
	return rs|immediate;
}
int inst_nori(int rs, int immediate) {
	return ~(rs|immediate);
}
int inst_slti(int rs, int immediate) {
	return rs<immediate;
}

void init_datapath() {
	/* Init function pointers. */
	
	/* R-format */
	R_func[ADD]=&inst_add;
	R_func[ADDU]=&inst_addu;
	R_func[SUB]=&inst_sub;
	R_func[AND]=&inst_and;
	R_func[OR]=&inst_or;
	R_func[XOR]=&inst_xor;
	R_func[NOR]=&inst_nor;
	R_func[NAND]=&inst_nand;
	R_func[SLT]=&inst_slt;
	R_func[SLL]=&inst_sll;
	R_func[SRL]=&inst_srl;
	R_func[SRA]=&inst_sra;
	R_func[JR]=NULL;
	R_func[MULT]=&inst_mult;
	R_func[MULTU]=&inst_multu;
	R_func[MFHI]=&inst_mfhi;
	R_func[MFLO]=&inst_mflo;
	
	/* I-format and J-format */
	func[ADDI]=&inst_addi;
	func[ADDIU]=&inst_addiu;
	func[LW]=&inst_lw;
	func[LH]=&inst_lh;
	func[LHU]=&inst_lhu;
	func[LB]=&inst_lb;
	func[LBU]=&inst_lbu;
	func[SW]=&inst_sw;
	func[SH]=&inst_sh;
	func[SB]=&inst_sb;
	func[LUI]=&inst_lui;
	func[ANDI]=&inst_andi;
	func[ORI]=&inst_ori;
	func[NORI]=&inst_nori;
	func[SLTI]=&inst_slti;
	func[BEQ]=NULL;
	func[BNE]=NULL;
	func[BGTZ]=NULL;
	func[J]=NULL;
	func[JAL]=NULL;
	func[HALT]=NULL;
}
