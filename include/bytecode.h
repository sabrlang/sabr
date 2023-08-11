#ifndef __BYTECODE_H__
#define __BYTECODE_H__

#include <stdarg.h>
// #include <stdbool.h>
#include <stdio.h>

#include "cctl_define.h"

#include "bcop.h"

typedef struct bytecode_struct {
	vector(bytecode_operation) bcop_vec;
	size_t current_index;
	size_t current_pos;
} bytecode;

void print_bytecode(bytecode* bc);

#endif