#include <stdio.h>

#include "compiler.h"
#include "interpreter.h"

sabr_compiler comp;
sabr_interpreter inter;

int main() {
	if (!sabr_compiler_init(&comp)) return 1;
	if (!sabr_interpreter_init(&inter)) return 1;
	if (!sabr_interpreter_memory_pool_init(&inter, 131072, 131072)) return 1;

	bytecode* bc = NULL;
	// bc = sabr_compiler_compile_file(&comp, "test.sabrc");
	// if (!bc) return 1;

	// if (!sabr_compiler_save_bytecode(&comp, bc, "test.sabre")) return 1;

	// sabr_bytecode_free(bc);
	// free(bc);

	bc = sabr_interpreter_load_bytecode(&inter, "test.sabre");
	if (!bc) return 1;

	sabr_interpreter_run_bytecode(&inter, bc);
	sabr_bytecode_print(bc);
	
	sabr_bytecode_free(bc);
	free(bc);

	if (!sabr_compiler_del(&comp)) return 1;
	if (!sabr_interpreter_del(&inter)) return 1;

	return 0;
}