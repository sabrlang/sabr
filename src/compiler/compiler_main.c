#include <stdio.h>

#include "compiler.h"

sabr_compiler comp;

int main() {
	if (!sabr_compiler_init(&comp)) return 1;

	bytecode* bc = sabr_compiler_compile_file(&comp, "test.sabrc");

	print_bytecode(bc);

	if (!sabr_compiler_save_bytecode(&comp, bc, "test.sabre")) {
		return 1;
	}

	sabr_free_bytecode(bc);
	free(bc);

	if (!sabr_compiler_del(&comp)) return 2;
}