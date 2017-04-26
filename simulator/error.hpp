#ifndef ERROR_H
#define ERROR_H

#include <cstdio>
#include <fstream>
#include <string>

enum { WRITE_ZERO, MEM_ADDR_OVF, DATA_MISALIGNED, OVERWRITE_HILO, NUM_OVF };
//extern std::ofstream ferr;
extern FILE *ferr;
extern std::string err_str[5];

void init_error();
void error(int type);
void output_error();

#endif
