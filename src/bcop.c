#include "bcop.h"

bytecode_operation sabr_new_bcop(opcode oc) {
	bytecode_operation bc = {0, };
	bc.oc = oc;
	return bc;
}

bytecode_operation sabr_new_bcop_with_value(opcode oc, value v) {
	bytecode_operation bc = {0, };
	bc.oc = oc;
	bc.operand = v;
	return bc;
}

bytecode_operation sabr_new_bcop_with_null(opcode oc) {
	value v = {0, };
	return sabr_new_bcop_with_value(oc, v);
}