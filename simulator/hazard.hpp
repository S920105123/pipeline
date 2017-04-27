#ifndef HARZARD_H
#define HARZARD_H

extern bool fwd_exmem_ex_rs, fwd_exmem_ex_rt, fwd_memwb_ex_rs, fwd_memwb_ex_rt;
extern bool ex_stall;
void detect_hazard();
void detect_stall();

#endif
