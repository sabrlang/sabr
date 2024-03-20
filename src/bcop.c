#include "bcop.h"

sabr_bcop_t sabr_new_bcop(sabr_opcode_t oc) {
	sabr_bcop_t bc = {0, };
	bc.oc = oc;
	return bc;
}

sabr_bcop_t sabr_new_bcop_with_value(sabr_opcode_t oc, sabr_value_t v) {
	sabr_bcop_t bc = {0, };
	bc.oc = oc;
	bc.operand = v;
	return bc;
}

sabr_bcop_t sabr_new_bcop_with_null(sabr_opcode_t oc) {
	sabr_value_t v = {0, };
	return sabr_new_bcop_with_value(oc, v);
}