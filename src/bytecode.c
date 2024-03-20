#include "bytecode.h"

void sabr_bytecode_print(sabr_bytecode_t* bc) {
	size_t j = 0;
	for (size_t i = 0; i < bc->bcop_vec.size; i++) {
		sabr_bcop_t bcop = *vector_at(sabr_bcop_t, &bc->bcop_vec, i);
		printf("%5zu\t%5zu\t\t%-10.20s", i, j, sabr_opcode_names[bcop.oc]);
		if (sabr_opcode_has_operand(bcop.oc)) {
			printf("\t\t%zu", bcop.operand.u);
			j += 8;
		}
		j++;
		putchar('\n');
	}
}

void sabr_bytecode_init(sabr_bytecode_t* bc) {
	vector_init(sabr_bcop_t, &bc->bcop_vec);
	bc->current_index = 0;
	bc->current_pos = 0;
}

void sabr_bytecode_free(sabr_bytecode_t* bc) {
	if (!bc) return;
	vector_free(sabr_bcop_t, &bc->bcop_vec);
}

bool sabr_bytecode_write_bcop(sabr_bytecode_t* bc_data, sabr_opcode_t oc) {
	if (!vector_push_back(sabr_bcop_t, &bc_data->bcop_vec, sabr_new_bcop(oc))) {
		fputs(sabr_errmsg_alloc, stderr);
		return false;
	}
	bc_data->current_index++;
	bc_data->current_pos++;
	return true;
}

bool sabr_bytecode_write_bcop_with_null(sabr_bytecode_t* bc_data, sabr_opcode_t oc) {
	if (!vector_push_back(sabr_bcop_t, &bc_data->bcop_vec, sabr_new_bcop_with_null(oc))) {
		fputs(sabr_errmsg_alloc, stderr);
		return false;
	}
	bc_data->current_index++;
	bc_data->current_pos += 9;
	return true;
}

bool sabr_bytecode_write_bcop_with_value(sabr_bytecode_t* bc_data, sabr_opcode_t oc, sabr_value_t v) {
	if (!vector_push_back(sabr_bcop_t, &bc_data->bcop_vec, sabr_new_bcop_with_value(oc, v))) {
		fputs(sabr_errmsg_alloc, stderr);
		return false;
	}
	bc_data->current_index++;
	bc_data->current_pos += 9;
	return true;
}