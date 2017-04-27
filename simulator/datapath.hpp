#ifndef DATAPATH_H
#define DATAPATH_H

#include <iostream>
#include <string>
#include <queue>
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

const int NONE=35;

/* Registers */
extern int reg[36], HI, LO, &PC, &sp;
extern int pre_reg[36], &pre_PC, &pre_sp;
extern State if_id, id_ex, ex_mem, mem_wb;
extern std::queue<int> change;

void inst_fetch();
void inst_decode();
void execution();
void mem_access();
void write_back();
void print_stage();
std::string& get_str(State &who);
void init_pipeline();

#endif

