#ifndef __BYTECODE_H__
#define __BYTECODE_H__

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "cctl_define.h"

#include "error_message.h"

#include "bcop.h"

typedef struct bytecode_struct {
	vector(bytecode_operation) bcop_vec;
	size_t current_index;
	size_t current_pos;
} bytecode;

void sabr_bytecode_print(bytecode* bc);
void sabr_bytecode_init(bytecode* bc);
void sabr_bytecode_free(bytecode* bc);

bool sabr_bytecode_write_bcop(bytecode* bc_data, opcode oc);
bool sabr_bytecode_write_bcop_with_null(bytecode* bc_data, opcode oc);
bool sabr_bytecode_write_bcop_with_value(bytecode* bc_data, opcode oc, value v);

#endif