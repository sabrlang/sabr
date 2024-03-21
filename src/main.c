#include <stdio.h>
#include "cmd.h"

sabr_compiler_t comp;
sabr_interpreter_t inter;

int main(int argc, char* argv[]) {
	return sabr_cmd_run(&cmd, &comp, &inter, argc, argv);
}