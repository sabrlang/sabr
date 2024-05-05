#ifndef __BIF_IO_H__
#define __BIF_IO_H__

#include "bif_def.h"

extern size_t sabr_bif_io_functions_len;
extern sabr_bif_func_t sabr_bif_io_functions[];

bool sabr_interpreter_fputc(sabr_interpreter_t* inter, sabr_value_t character, sabr_value_t file);
bool sabr_interpreter_fputs(sabr_interpreter_t* inter, sabr_value_t addr, sabr_value_t file);

#endif