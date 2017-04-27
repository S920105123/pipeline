#include <iostream>
#include "hazard.hpp"
#include "datapath.hpp"
#include "const.hpp"

extern State if_id, id_ex, ex_mem, mem_wb, wb_temp;
bool branch;
bool ex_stall, id_stall;
bool fwd_exmem_ex_rs, fwd_exmem_ex_rt, fwd_memwb_ex_rs, fwd_memwb_ex_rt;
bool fwd_exmem_id_rs, fwd_exmem_id_rt, fwd_memwb_id_rs, fwd_memwb_id_rt;
int target_addr;

void detect_branch()
{
	branch=false;
	if (ex_stall || id_stall) {
		return;
	}
	
	int rs, rt;
	if (if_id.R_format) {
		if (if_id.opcode==JR) {
			branch=true;
			if (fwd_exmem_id_rs) {
				target_addr=ex_mem.immediate;
			} else if (fwd_memwb_id_rs) {
				target_addr=mem_wb.immediate;
			} else {
				target_addr=reg[if_id.rs];
			}
		}
		return;
	}
	if (if_id.opcode==BEQ) {
		if (fwd_exmem_id_rs) {
			rs=ex_mem.immediate;
		} else if (fwd_memwb_id_rs) {
			rs=mem_wb.immediate;
		} else {
			rs=reg[if_id.rs];
		}
		if (fwd_exmem_id_rt) {
			rt=ex_mem.immediate;
		} else if (fwd_memwb_id_rt) {
			rt=mem_wb.immediate;
		} else {
			rt=reg[if_id.rs];
		}
		//std::cerr<<fwd_exmem_id_rs<<' '<<if_id.rs<<' '<<reg[if_id.rs]<<std::endl;
		branch = (rs==rt); 
		target_addr=PC+4*if_id.immediate;
	} else if (if_id.opcode==BNE) {
		if (fwd_exmem_id_rs) {
			rs=ex_mem.immediate;
		} else if (fwd_memwb_id_rs) {
			rs=mem_wb.immediate;
		} else {
			rs=reg[if_id.rs];
		}	
		if (fwd_exmem_id_rt) {
                        rt=ex_mem.immediate;
                } else if (fwd_memwb_id_rt) {
                        rt=mem_wb.immediate;
                } else {
                        rt=reg[if_id.rs];
                }
		branch = (rs!=rt); 
		target_addr=PC+4*if_id.immediate;
	} else if (if_id.opcode==BGTZ) {
		if (fwd_exmem_id_rs) {
                        rs=ex_mem.immediate;
                } else if (fwd_memwb_id_rs) {
                        rs=mem_wb.immediate;
                } else {
                        rs=reg[if_id.rs];
                }
		branch = (rs>0);
		target_addr=PC+4*if_id.immediate;
	} else if (if_id.opcode==J || if_id.opcode==JAL) {
		branch=true;
		target_addr=4*if_id.immediate; // Bad implementation, but in this project PC<1024.
	}
}

void detect_id_stall() 
{
	int read_reg, write_reg;
	id_stall=false;
	
	/* Case: ID read rs, EX working. */
	read_reg=-1; write_reg=-1;
	if (if_id.R_format && if_id.opcode==JR) {
		read_reg=if_id.rs;
	} else if (!if_id.R_format && (if_id.opcode==BEQ || if_id.opcode==BNE || if_id.opcode==BGTZ)) {
		read_reg=if_id.rs;
	}
	if (id_ex.R_format && write_rd[id_ex.opcode]) {
		write_reg=id_ex.rd;
	} else if (!id_ex.R_format && write_rt[id_ex.opcode]) {
		write_reg=id_ex.rt;
	}
	id_stall = id_stall||(read_reg==write_reg && read_reg!=0 && read_reg!=-1);
	
	/* Case: ID read rs, DM load. */
	write_reg=-1;
	if (!ex_mem.R_format && is_load[ex_mem.opcode]) {
		write_reg=ex_mem.rt;
	}
	id_stall = id_stall||(read_reg==write_reg && read_reg!=0 && read_reg!=-1);
	
	/* Case: ID read rt, EX working. */
	read_reg=-1; write_reg=-1;
	if (!if_id.R_format && (if_id.opcode==BEQ || if_id.opcode==BNE)) {
		read_reg=if_id.rt;
	}
	if (id_ex.R_format && write_rd[id_ex.opcode]) {
		write_reg=id_ex.rd;
	} else if (!id_ex.R_format && write_rt[id_ex.opcode]) {
		write_reg=id_ex.rt;
	}
	id_stall = id_stall||(read_reg==write_reg && read_reg!=0 && read_reg!=-1);
	
	/* Case: ID read rt, DM load. */
	write_reg=-1;
	if (!ex_mem.R_format && is_load[ex_mem.opcode]) {
		write_reg=ex_mem.rt;
	}
	id_stall = id_stall||(read_reg==write_reg && read_reg!=0 && read_reg!=-1);
	
	/* Special case: If stalled, no need for fwd. */
	if (id_stall) {
		fwd_exmem_id_rs=false; 
		fwd_exmem_id_rt=false;
		fwd_memwb_id_rs=false;
		fwd_memwb_id_rt=false;
	}
}

void detect_id_hazard()
{
	int read_reg, write_reg;
	
	/* 
		id_ex write => stall.
		ex_mem write, and ready => fwd.
		ex_mem write, but load => stall (Will set fwd=false in detect_stall_id).
		mem_wb write => take reg value directly. (Implement a fwd path due to structure problem)
	*/
	fwd_exmem_id_rs=false; 
	fwd_exmem_id_rt=false;
	fwd_memwb_id_rs=false; 
	fwd_memwb_id_rt=false;
	
	/* Case: ID read rs, EX write rs. */
	read_reg=-1; write_reg=-1;
	if (if_id.R_format && if_id.opcode==JR) {
		read_reg=if_id.rs;
	} else if (!if_id.R_format && (if_id.opcode==BEQ || if_id.opcode==BNE || if_id.opcode==BGTZ)) {
		read_reg=if_id.rs;
	}
	if (ex_mem.R_format && write_rd[ex_mem.opcode]) {
		write_reg=ex_mem.rd;
	} else if (!ex_mem.R_format && write_rt[ex_mem.opcode]) {
		write_reg=ex_mem.rt;
	}
	fwd_exmem_id_rs = (write_reg==read_reg && read_reg!=-1 && read_reg!=0);
	
	/* Case: ID read rt, EX write rt */
	read_reg=-1;
	if (!if_id.R_format && (if_id.opcode==BEQ || if_id.opcode==BNE)) {
		read_reg=if_id.rt;
	}
	fwd_exmem_id_rt = (write_reg==read_reg && read_reg!=-1 && read_reg!=0);
	
	/* Case: ID read rs, DM write rs(Not real forwarding path) */
	read_reg=-1; write_reg=-1;
	if (mem_wb.R_format && write_rd[mem_wb.opcode]) {
		write_reg=mem_wb.rd;
	} else if (!mem_wb.R_format && write_rt[mem_wb.opcode]) {
		write_reg=mem_wb.rt;
	}
	if (if_id.R_format && if_id.opcode==JR) {
		read_reg=if_id.rs;
	} else if (!if_id.R_format && (if_id.opcode==BEQ || if_id.opcode==BNE || if_id.opcode==BGTZ)) {
		read_reg=if_id.rs;
	}
	fwd_memwb_id_rs = (write_reg==read_reg && read_reg!=-1 && read_reg!=0);
	
	/* Case: ID read rt, DM write rt(Not readl fwd path) */
	read_reg=-1;
	if (!if_id.R_format && (if_id.opcode==BEQ || if_id.opcode==BNE)) {
		read_reg=if_id.rt;
	}
	fwd_memwb_id_rt = (write_reg==read_reg && read_reg!=-1 && read_reg!=0);
	
	if (fwd_exmem_id_rs) {
		fwd_memwb_id_rs=false;
	}
	if (fwd_exmem_id_rt) {
		fwd_memwb_id_rt=false;
	}
}

void detect_stall()
{
	int read_reg, write_reg;
	
	ex_stall=false;
	
	/* Case: EX read rs, DM load rs. */
	read_reg=-1;
	write_reg=-1;
	if (!id_ex.R_format && is_load[id_ex.opcode]) {
		write_reg=id_ex.rt;
	}
	if (if_id.R_format && read_rs_r[if_id.opcode]) {
		read_reg=if_id.rs;
	} else if (!if_id.R_format && read_rs[if_id.opcode]) {
		read_reg=if_id.rs;
	}
	ex_stall = (write_reg==read_reg && read_reg!=0 && read_reg!=-1);
	
	/* Case: EX read rt, DM load rt. */
	read_reg=-1;
	write_reg=-1;
	if (!id_ex.R_format && is_load[id_ex.opcode]) {
		write_reg=id_ex.rt;
	}
	if (if_id.R_format && read_rt_r[if_id.opcode]) {
		read_reg=if_id.rt;
	} else if (!if_id.R_format && read_rt[if_id.opcode]) {
		read_reg=if_id.rt;
	}
	ex_stall = ex_stall || (write_reg==read_reg && read_reg!=0 && read_reg!=-1);
	
	/* If stalled, no EX-fwd there. */
	if (ex_stall) {
		fwd_exmem_ex_rs=false;
		fwd_exmem_ex_rt=false;
		fwd_memwb_ex_rs=false;
		fwd_memwb_ex_rt=false;
	}
}

void detect_hazard()
{
	int read_reg, write_reg;
	
	fwd_exmem_ex_rs=false;
	fwd_exmem_ex_rt=false;
	fwd_memwb_ex_rs=false;
	fwd_memwb_ex_rt=false;
	
	/* Case: ex read rs, exmem write rs => fwd_exmem_ex_rs */
	read_reg=-1;
	write_reg=-1;
	if (ex_mem.R_format && write_rd[ex_mem.opcode]) {
		write_reg=ex_mem.rd;
	} else if (!ex_mem.R_format && write_rt[ex_mem.opcode]) {
		write_reg=ex_mem.rt;
	}
	if (id_ex.R_format && read_rs_r[id_ex.opcode]) {
		read_reg=id_ex.rs;
	} else if (!id_ex.R_format && read_rs[id_ex.opcode]) {
		read_reg=id_ex.rs;
	}
	fwd_exmem_ex_rs = (read_reg==write_reg && read_reg!=-1 && read_reg!=0);
	
	/* Case: ex read rs, memwb write rs => fwd_memwb_ex_rs */
	read_reg=-1;
	write_reg=-1;
	if (mem_wb.R_format && write_rd[mem_wb.opcode]) {
		write_reg=mem_wb.rd;
	} else if (!mem_wb.R_format && write_rt[mem_wb.opcode]) {
		write_reg=mem_wb.rt;
	}
	if (id_ex.R_format && read_rs_r[id_ex.opcode]) {
		read_reg=id_ex.rs;
	} else if (!id_ex.R_format && read_rs[id_ex.opcode]) {
		read_reg=id_ex.rs;
	}
	fwd_memwb_ex_rs = (read_reg==write_reg && read_reg!=-1 && read_reg!=0);
	
	/* Case: ex read rt, exmem write rt => fwd_exmem_ex_rt */
	read_reg=-1;
	write_reg=-1;
	if (ex_mem.R_format && write_rd[ex_mem.opcode]) {
		write_reg=ex_mem.rd;
	} else if (!ex_mem.R_format && write_rt[ex_mem.opcode]) {
		write_reg=ex_mem.rt;
	}
	if (id_ex.R_format && read_rt_r[id_ex.opcode]) {
		read_reg=id_ex.rt;
	} else if (!id_ex.R_format && read_rt[id_ex.opcode]) {
		read_reg=id_ex.rt;
	}
	fwd_exmem_ex_rt = (read_reg==write_reg && read_reg!=-1 && read_reg!=0);
	
	/* Case: ex read rt, memwb write rt => fwd_memwb_ex_rt */
	read_reg=-1;
	write_reg=-1;
	if (mem_wb.R_format && write_rd[mem_wb.opcode]) {
		write_reg=mem_wb.rd;
	} else if (!mem_wb.R_format && write_rt[mem_wb.opcode]) {
		write_reg=mem_wb.rt;
	}
	if (id_ex.R_format && read_rt_r[id_ex.opcode]) {
		read_reg=id_ex.rt;
	} else if (!id_ex.R_format && read_rt[id_ex.opcode]) {
		read_reg=id_ex.rt;
	}
	fwd_memwb_ex_rt = (read_reg==write_reg && read_reg!=-1 && read_reg!=0);
	
	/* Special case: double hazard, fwd the lattest data. */
	if (fwd_exmem_ex_rs) {
		fwd_memwb_ex_rs=false;
	}
	if (fwd_exmem_ex_rt) {
		fwd_memwb_ex_rt=false;
	}
}
