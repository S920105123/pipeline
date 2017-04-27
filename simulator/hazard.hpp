#ifndef HARZARD_H
#define HARZARD_H

extern bool fwd_exmem_ex_rs, fwd_exmem_ex_rt, fwd_memwb_ex_rs, fwd_memwb_ex_rt;
extern bool fwd_exmem_id_rs, fwd_exmem_id_rt;
extern bool ex_stall,id_stall;
extern bool branch;
extern int target_addr;

void detect_branch();
void detect_hazard();
void detect_stall();
void detect_id_hazard();
void detect_id_stall();

#endif
