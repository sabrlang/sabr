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

sabr_bytecode_t* sabr_bytecode_concat(sabr_bytecode_t* a, sabr_bytecode_t* b) {
	sabr_bytecode_t* concated_bc = NULL;
	concated_bc = (sabr_bytecode_t*) malloc(sizeof(sabr_bytecode_t));
	if (!concated_bc) return NULL;
	sabr_bytecode_init(concated_bc);

	if (!sabr_bytecode_join(concated_bc, a)) return NULL;
	if (!sabr_bytecode_join(concated_bc, b)) return NULL;

	return concated_bc;
}

bool sabr_bytecode_join(sabr_bytecode_t* dest, sabr_bytecode_t* src) {
	size_t dest_index = dest->current_index;
	for (size_t i = 0; i < src->bcop_vec.size; i++) {
		sabr_bcop_t bcop = *vector_at(sabr_bcop_t, &src->bcop_vec, i);
		dest->current_index++;
		if (sabr_opcode_has_operand(bcop.oc)) {
			dest->current_pos += 9;
			if (sabr_opcode_has_index_operand(bcop.oc)) bcop.operand.u += dest_index;
		}
		else dest->current_pos ++;
		if (!vector_push_back(sabr_bcop_t, &dest->bcop_vec, bcop)) {
			fputs(sabr_errmsg_alloc, stderr);
			return false;
		}
	}
	return true;
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