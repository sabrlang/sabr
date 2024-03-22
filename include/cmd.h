#ifndef __CMD_H__
#define __CMD_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>

#include "compiler.h"
#include "interpreter.h"
#include "cmake_config.h"

typedef struct option option_t;
typedef struct sabr_cmd_struct {
	bool compile;
	bool execute;
	bool out;
	bool run;
	bool bytecode;
	bool preprocess;
	bool version;
	bool help;
	char src_filename[PATH_MAX];
	char out_filename[PATH_MAX];
	char bc_filename[PATH_MAX];
	option_t long_opts[8];
	char opts[12];
} sabr_cmd_t;

extern sabr_cmd_t cmd;

void sabr_cmd_get_opt(sabr_cmd_t* cmd, int argc, char** argv);
int sabr_cmd_run(sabr_cmd_t* cmd, sabr_compiler_t* comp, sabr_interpreter_t* inter, int argc, char** argv);

void cmd_print_version(sabr_cmd_t* cmd);
void cmd_print_help(sabr_cmd_t* cmd);

#endif