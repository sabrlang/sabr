#include "bcop.h"

bytecode_operation sabr_new_bytecode(opcode oc) {
	bytecode_operation bc = {0, };
	bc.oc = oc;
	return bc;
}

bytecode_operation sabr_new_bytecode_with_value(opcode oc, value v) {
	bytecode_operation bc = {0, };
	bc.oc = oc;
	bc.has_operand = true;
	bc.operand = v;
	return bc;
}

bytecode_operation sabr_new_bytecode_with_null(opcode oc) {
	value v = {0, };
	return sabr_new_bytecode_with_value(oc, v);
}