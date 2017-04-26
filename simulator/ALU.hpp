#ifndef ALU_H
#define ALU_H

#include <queue>

/* Function pointers, R_func[ADD]=add function, etc. */
extern int (*R_func[64])(int rs, int rt, int shamt);
extern int (*func[64])(int rs, int immediate);

/* Call this to init pointer */
void init_datapath();

/* Pointed functions */
/* R-format instructions */
void inst_add(); 
void inst_addu();
void inst_sub();
void inst_and();
void inst_or();
void inst_xor();
void inst_nor();
void inst_nand();
void inst_slt(); 
void inst_sll();
void inst_srl();
void inst_sra();
void inst_jr();
void inst_mult();
void inst_multu();
void inst_mfhi();
void inst_mflo();

/* I-format instructions */
void inst_addi();
void inst_addiu();
void inst_lw();
void inst_lh();
void inst_lhu();
void inst_lb();
void inst_lbu();
void inst_sw();
void inst_sh();
void inst_sb();
void inst_lui();
void inst_andi();
void inst_ori();
void inst_nori(); 
void inst_slti();
void inst_beq();
void inst_bne();
void inst_bgtz();
void inst_j();
void inst_jal();

#endif
