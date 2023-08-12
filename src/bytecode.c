#include "bytecode.h"

void print_bytecode(bytecode* bc) {
	size_t j = 0;
	for (size_t i = 0; i < bc->bcop_vec.size; i++) {
		bytecode_operation bcop = *vector_at(bytecode_operation, &bc->bcop_vec, i);
		printf("%5zu\t%5zu\t\t%-10.20s", i, j, opcode_names[bcop.oc]);
		if (bcop.has_operand) {
			printf("\t\t%zu", bcop.operand.u);
			j += 8;
		}
		j++;
		putchar('\n');
	}
}

void sabr_free_bytecode(bytecode* bc) {
	if (!bc) return;
	vector_free(bytecode_operation, &bc->bcop_vec);
}