#include <stdio.h>

#include "compiler.h"

sabr_compiler comp;

int main() {
	if (!sabr_compiler_init(&comp)) return 1;

	sabr_compiler_compile_file(&comp, "test.sabrc");

	sabr_compiler_del(&comp);
}