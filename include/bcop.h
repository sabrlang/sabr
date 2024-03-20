#ifndef __BCOP_H__
#define __BCOP_H__

#include <stdarg.h>
#include <stdbool.h>

#include "opcode.h"
#include "value.h"

typedef struct bytecode_operation_struct {
	opcode oc;
	value operand;
} bytecode_operation;

bytecode_operation sabr_new_bcop(opcode oc);
bytecode_operation sabr_new_bcop_with_value(opcode oc, value v);
bytecode_operation sabr_new_bcop_with_null(opcode co);

#endif