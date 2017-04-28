#include <iostream>
#include <iomanip>
#include <cstdio>
#include <fstream>
#include "const.hpp"
#include "loader.hpp"
#include "datapath.hpp"
#include "ALU.hpp"
#include "error.hpp"
#include "hazard.hpp"
//#define DEBUG

FILE *fout;
int num_inst, num_word, cycle;
char buf[512];
bool stop_simulate;

inline void print_reg(int idx) {
	/* Print changed registers into snapshot.rpt. */
	int len;
	static std::string reg_str[3]={"$HI","$LO","PC"};

	if (idx>31) {
		if (idx>33) {
			fwrite(reg_str[idx-32].c_str(),1,2,fout);
		} else {
			fwrite(reg_str[idx-32].c_str(),1,3,fout);
		}
	} else {
		//fout<<"$"<<std::dec<<std::setw(2)<<std::setfill('0')<<idx;
		len=sprintf(buf,"$%02d",idx);
		fwrite(buf,1,len,fout);
	}
	len=sprintf(buf,": 0x%08X\n",reg[idx]);
	//fout<<": 0x"<<std::hex<<std::uppercase<<std::setw(8)<<std::setfill('0')<<reg[idx]<<std::endl;
	fwrite(buf,1,len,fout);
}

inline void print_pipeline()
{
	/* Pipeline stages */
	int len=0;
	len+=sprintf(buf,"IF: 0x%08X",inst[PC>>2].origin);
	if (ex_stall || id_stall) len+=sprintf(buf+len," to_be_stalled");
	if (branch) len+=sprintf(buf+len," to_be_flushed");
	buf[len++]='\n';
	len+=sprintf(buf+len,"ID: %s",get_str(if_id).c_str());
	if (ex_stall || id_stall) len+=sprintf(buf+len," to_be_stalled");
	if (fwd_exmem_id_rs) len+=sprintf(buf+len," fwd_EX-DM_rs_$%d",if_id.rs);
	if (fwd_exmem_id_rt) len+=sprintf(buf+len," fwd_EX-DM_rt_$%d",if_id.rt);
	buf[len++]='\n';
	len+=sprintf(buf+len,"EX: %s",get_str(id_ex).c_str());
	if (fwd_exmem_ex_rs) len+=sprintf(buf+len," fwd_EX-DM_rs_$%d",id_ex.rs);
	if (fwd_memwb_ex_rs) len+=sprintf(buf+len," fwd_DM-WB_rs_$%d",id_ex.rs);
	if (fwd_exmem_ex_rt) len+=sprintf(buf+len," fwd_EX-DM_rt_$%d",id_ex.rt);
	if (fwd_memwb_ex_rt) len+=sprintf(buf+len," fwd_DM-WB_rt_$%d",id_ex.rt);
	buf[len++]='\n';
	len+=sprintf(buf+len,"DM: %s\n",get_str(ex_mem).c_str());
	len+=sprintf(buf+len,"WB: %s\n",get_str(mem_wb).c_str());
	fwrite(buf,1,len,fout);
}

void output()
{
	/* This function check whether register value is changed and output it if yes. */
	int len=sprintf(buf,"cycle %d\n",cycle);
        fwrite(buf,1,len,fout);
	while (!change.empty()) {
		int idx=change.front();
		change.pop();
		if (reg[idx]!=pre_reg[idx]) {
			print_reg(idx);
			pre_reg[idx]=reg[idx];
		}
	}
	
	print_reg(34);
	pre_PC=PC;
	//print_stage();
	print_pipeline();
	buf[0]=buf[1]='\n';
	fwrite(buf,1,2,fout);
}

void simulate()
{	
	/* This function control the flow of simulate. */
	int idx=(PC>>2), opcode=inst[idx].opcode, funct=inst[idx].funct;
	
	while (!stop_simulate) {
		if (PC>=1024 || PC<0) {
			return;
		}
		cycle++;
		
		write_back();
		mem_access();
		execution();
		inst_decode();
		inst_fetch();
		detect_hazard();
		detect_stall();
		detect_id_hazard();
		detect_id_stall();
		detect_branch();
		if (!stop_simulate) {
			output();
		}
		output_error();
		
		idx=PC>>2;
		opcode=inst[idx].opcode;
		funct=inst[idx].funct;
		if ( opcode==HALT                              &&
		     (!if_id.R_format && if_id.opcode==HALT)   && 
		     (!id_ex.R_format && id_ex.opcode==HALT)   &&
			 (!ex_mem.R_format && ex_mem.opcode==HALT) &&
			 (!mem_wb.R_format && mem_wb.opcode==HALT) ) {
			stop_simulate=true;
		}
	}
}

int main()
{	
	bool first=true;

	/* Initialization */	
	init_error();
	init_const();
	init_str_const();
	init_datapath();
	init_pipeline();
	load_img(PC,num_inst,num_word,sp,pre_sp);
	fout=fopen("snapshot.rpt","wb");
	
	#ifdef DEBUG
	for (int i=PC>>2;i<(PC>>2)+num_inst;i++) {
		print_inst(&inst[i]);
	}
	#endif
	
	int len=sprintf(buf,"cycle %d\n",cycle);
        fwrite(buf,1,len,fout);
	for (int i=0;i<35;i++) {
		print_reg(i);
	}
	print_pipeline();
	buf[0]=buf[1]='\n';
	fwrite(buf,1,2,fout);
	
	/* Start simulation */
	simulate();
	fclose(ferr);
	fclose(fout);
}
