#ifndef __BCOP_H__
#define __BCOP_H__

#include <stdarg.h>
#include <stdbool.h>

#include "opcode.h"
#include "value.h"

typedef struct sabr_bcop_struct {
	sabr_opcode_t oc;
	sabr_value_t operand;
} sabr_bcop_t;

sabr_bcop_t sabr_new_bcop(sabr_opcode_t oc);
sabr_bcop_t sabr_new_bcop_with_value(sabr_opcode_t oc, sabr_value_t v);
sabr_bcop_t sabr_new_bcop_with_null(sabr_opcode_t co);

#endif