#include <iostream>
#include "datapath.hpp"
#include "ALU.hpp"
#include "error.hpp"
#include "loader.hpp"
#include "const.hpp"
#include "hazard.hpp"

/* Registers */
int reg[36], HI=32, LO=33, &PC=reg[34], &sp=reg[29];
int pre_reg[36], &pre_PC=pre_reg[34], &pre_sp=pre_reg[29];
State if_id, id_ex, ex_mem, mem_wb;
std::queue<int> change;
bool stall;
extern bool stop_simulate,fwd_exmem_ex_rs, fwd_exmem_ex_rt, fwd_memwb_ex_rs, fwd_memwb_ex_rt;

State::State() {
	/* Default constructor, create a stalled state. */
	NOP=true;
	R_format=true;
	opcode=rs=rt=rd=immediate=0;
}
	
State::State(State &in) {
	/* Copy constructor */
	NOP=in.NOP;
	R_format=in.R_format; opcode=in.opcode;
	rs=in.rs; rt=in.rt; rd=in.rd;
	immediate=in.immediate;
}

State::State(Instruction &in) {
	if (in.opcode==0) {
		R_format = true;
		opcode=in.funct;
	} else {
		R_format = false;
		opcode=in.opcode;
	}
	if (R_format && in.funct==SLL && in.immediate==0 && in.rt==0 && in.rd==0) {
		NOP=true;
	} else {
		NOP=false;
	}
	rs=in.rs; rt=in.rt; rd=in.rd;
	immediate=in.immediate;
}

std::string& get_str(State &who) {
	if (who.NOP) {
		return str_nop;
	}
	if (who.R_format) {
		return inst_str_r[who.opcode];
	}
	return inst_str[who.opcode];
}

void init_pipeline() {
	if_id=State();
	id_ex=State();
	ex_mem=State();
	mem_wb=State();
}

void inst_fetch() {
	if_id=State(inst[PC>>2]);	
	PC=PC+4;
}

void inst_decode() {
	id_ex=if_id;
}

void execution() {
	int alu_rs, alu_rt;
	
	/* Forwarding, note that pipeline stages have advanced. */
	if (fwd_exmem_ex_rs) {
		alu_rs=ex_mem.immediate;
	} else {
		alu_rs=reg[id_ex.rs];
	}
	if (fwd_exmem_ex_rt) {
		alu_rs=ex_mem.immediate;
	} else {
		alu_rs=reg[id_ex.rt];
	}
	
	ex_mem=id_ex;
	
	if (ex_mem.R_format && R_func[ex_mem.opcode]!=NULL) {
		ex_mem.immediate=R_func[ex_mem.opcode](alu_rs, alu_rt, ex_mem.immediate);
	} else if (func[ex_mem.opcode]!=NULL) {
		ex_mem.immediate=func[ex_mem.opcode](alu_rs, ex_mem.immediate);
	}
}

void mem_access() {
	int opcode=ex_mem.opcode, immediate=ex_mem.immediate, rt=ex_mem.rt;
	
	mem_wb=ex_mem;
	if (!mem_wb.R_format) {
		if (opcode==SW) {
			if (immediate<0 || immediate+3>=1024) {
				error(MEM_ADDR_OVF);
				stop_simulate=true;
			}
			if (immediate%4!=0) {
				error(DATA_MISALIGNED);
				stop_simulate=true;
			}
			if (!stop_simulate) {
				mem[immediate>>2]=btol(reg[rt]);
			}
		} else if (opcode==SH) {
			if (immediate<0 || immediate+1>=1024)  {
				error(MEM_ADDR_OVF);
				stop_simulate=true;
			}
			if ((immediate)%2!=0) {
				error(DATA_MISALIGNED);
				stop_simulate=true;
			}
			if (!stop_simulate) {
				*((short*)mem+(immediate>>1))=h_btol(reg[rt]&0x0000FFFF);
			}
		} else if (opcode==SB) {
			if (immediate<0 || immediate>=1024)  {
				error(MEM_ADDR_OVF);
				stop_simulate=true;
			}
			if (!stop_simulate) {
				*((char*)mem+immediate)=reg[rt]&0x000000FF;
			}
		} else if (opcode==LW) {
			if (immediate<0 || immediate+3>=1024)  {
				error(MEM_ADDR_OVF);
				stop_simulate=true;
			}
			if ((immediate)%4!=0) {
				error(DATA_MISALIGNED);
				stop_simulate=true;
			}
			if (!stop_simulate) {
				mem_wb.immediate=mem[immediate>>2];
				mem_wb.immediate=btol(mem_wb.immediate);
			}
		} else if (opcode==LH)  {
			if (immediate<0 || immediate+1>=1024)  {
				error(MEM_ADDR_OVF);
				stop_simulate=true;
			}
			if ((immediate)%2!=0) {
				error(DATA_MISALIGNED);
				stop_simulate=true;
			}
			if (!stop_simulate) {
				mem_wb.immediate=*((short*)mem+(immediate>>1));
				mem_wb.immediate=h_btol((short)mem_wb.immediate);
			}
		} else if (opcode==LHU) {
			if (immediate<0 || immediate+1>=1024)  {
				error(MEM_ADDR_OVF);
				stop_simulate=true;
			}
			if ((immediate)%2!=0) {
				error(DATA_MISALIGNED);
				stop_simulate=true;
			}
			if (!stop_simulate) {
				mem_wb.immediate=*((unsigned short*)mem+(immediate>>1));
				mem_wb.immediate=(unsigned short)h_btol(mem_wb.immediate);
				mem_wb.immediate=mem_wb.immediate&0x0000FFFF;  // Just in case.
			}
		} else if (opcode==LB) {
			if (immediate<0 || immediate>=1024)  {
				error(MEM_ADDR_OVF);
				stop_simulate=true;
			}
			if (!stop_simulate){
				mem_wb.immediate=*((char*)mem+immediate);
			}
		} else if (opcode==LBU) {
			if (immediate<0 || immediate>=1024)  {
				error(MEM_ADDR_OVF);
				stop_simulate=true;
			}
			if (!stop_simulate) {
				mem_wb.immediate=*((unsigned char*)mem+immediate);
				mem_wb.immediate=mem_wb.immediate&0x000000FF;  // Just in case.
			}
		}
	}
}

void write_back() {
	if (mem_wb.R_format && write_rd[mem_wb.opcode]) {
		if (mem_wb.rd==0) {
			error(WRITE_ZERO);
		} else {
			reg[mem_wb.rd]=mem_wb.immediate;
			change.push(mem_wb.rd);
		}
	} else if (write_rt[mem_wb.opcode]) {
		if (mem_wb.rt==0) {
			error(WRITE_ZERO);
		} else {
			reg[mem_wb.rt]=mem_wb.immediate;
			change.push(mem_wb.rt);
		}
	}
}
