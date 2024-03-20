#include "bytecode.h"

void sabr_bytecode_print(bytecode* bc) {
	size_t j = 0;
	for (size_t i = 0; i < bc->bcop_vec.size; i++) {
		bytecode_operation bcop = *vector_at(bytecode_operation, &bc->bcop_vec, i);
		printf("%5zu\t%5zu\t\t%-10.20s", i, j, opcode_names[bcop.oc]);
		if (sabr_opcode_has_operand(bcop.oc)) {
			printf("\t\t%zu", bcop.operand.u);
			j += 8;
		}
		j++;
		putchar('\n');
	}
}

void sabr_bytecode_init(bytecode* bc) {
	vector_init(bytecode_operation, &bc->bcop_vec);
	bc->current_index = 0;
	bc->current_pos = 0;
}

void sabr_bytecode_free(bytecode* bc) {
	if (!bc) return;
	vector_free(bytecode_operation, &bc->bcop_vec);
}

bool sabr_bytecode_write_bcop(bytecode* bc_data, opcode oc) {
	if (!vector_push_back(bytecode_operation, &bc_data->bcop_vec, sabr_new_bcop(oc))) {
		fputs(sabr_errmsg_alloc, stderr);
		return false;
	}
	bc_data->current_index++;
	bc_data->current_pos++;
	return true;
}

bool sabr_bytecode_write_bcop_with_null(bytecode* bc_data, opcode oc) {
	if (!vector_push_back(bytecode_operation, &bc_data->bcop_vec, sabr_new_bcop_with_null(oc))) {
		fputs(sabr_errmsg_alloc, stderr);
		return false;
	}
	bc_data->current_index++;
	bc_data->current_pos += 9;
	return true;
}

bool sabr_bytecode_write_bcop_with_value(bytecode* bc_data, opcode oc, value v) {
	if (!vector_push_back(bytecode_operation, &bc_data->bcop_vec, sabr_new_bcop_with_value(oc, v))) {
		fputs(sabr_errmsg_alloc, stderr);
		return false;
	}
	bc_data->current_index++;
	bc_data->current_pos += 9;
	return true;
}