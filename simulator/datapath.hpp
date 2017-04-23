#ifndef DATAPATH_H
#define DATAPATH_H

#include <iostream>
#include <string>
#include "loader.hpp"

struct State {
	bool R_format;
	int opcode;
	int rs,rt,rd;
	int immediate;
	bool NOP;
	State();
	State(State &in);
	State(Instruction &in);
};

extern State if_id, id_ex, ex_mem, mem_wb;

void inst_fetch();
void inst_decode();
void execution();
void mem_access();
void write_back();
std::string& get_str(State &who);
void init_pipeline();

#endif

