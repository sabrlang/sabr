#ifndef __BYTECODE_H__
#define __BYTECODE_H__

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "cctl_define.h"

#include "error_message.h"

#include "bcop.h"

typedef struct sabr_bytecode_struct {
	vector(sabr_bcop_t) bcop_vec;
	size_t current_index;
	size_t current_pos;
} sabr_bytecode_t;

void sabr_bytecode_print(sabr_bytecode_t* bc);
void sabr_bytecode_init(sabr_bytecode_t* bc);
void sabr_bytecode_free(sabr_bytecode_t* bc);

bool sabr_bytecode_write_bcop(sabr_bytecode_t* bc_data, sabr_opcode_t oc);
bool sabr_bytecode_write_bcop_with_null(sabr_bytecode_t* bc_data, sabr_opcode_t oc);
bool sabr_bytecode_write_bcop_with_value(sabr_bytecode_t* bc_data, sabr_opcode_t oc, sabr_value_t v);

#endif