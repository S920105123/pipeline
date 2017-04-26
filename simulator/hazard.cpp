#include <iostream>
#include "hazard.hpp"
#include "datapath.hpp"
#include "const.hpp"

extern State if_id, id_ex, ex_mem, mem_wb, wb_temp;
bool fwd_exmem_ex_rs, fwd_exmem_ex_rt, fwd_memwb_ex_rs, fwd_memwb_ex_rt;

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
