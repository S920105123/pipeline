#include <iostream>
#include "datapath.hpp"
#include "ALU.hpp"
#include "error.hpp"
#include "loader.hpp"
#include "const.hpp"

State if_id, id_ex, ex_mem, mem_wb;

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
	if (R_format && in.funct==SLL && in.immediate==0 && in.rt==0 &&in.rd==0) {
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
	ex_mem=id_ex;
}

void mem_access() {
	mem_wb=ex_mem;
}

void write_back() {
	
}
