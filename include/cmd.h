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

typedef struct sabr_cmd_flag_struct {
	bool compile;
	bool execute;
	bool out;
	bool memory;
	bool run;
	bool bytecode;
	bool preprocess;
	bool version;
	bool help;
} sabr_cmd_flag_t;

typedef struct option option_t;
typedef struct sabr_cmd_struct {
	sabr_cmd_flag_t flags;
	option_t long_opts[9];
	char opts[13];
	char src_filename[PATH_MAX];
	char out_filename[PATH_MAX];
	char bc_filename[PATH_MAX];
	size_t memory_pool_size;
} sabr_cmd_t;

extern sabr_cmd_t cmd;
extern size_t sabr_cmd_opts_len;

void sabr_cmd_get_opt(sabr_cmd_t* cmd, int argc, char** argv);
int sabr_cmd_run(sabr_cmd_t* cmd, sabr_compiler_t* comp, sabr_interpreter_t* inter, int argc, char** argv);

void sabr_cmd_get_opt_compile(sabr_cmd_t* cmd);
void sabr_cmd_get_opt_execute(sabr_cmd_t* cmd);
void sabr_cmd_get_opt_out(sabr_cmd_t* cmd);
void sabr_cmd_get_opt_memory(sabr_cmd_t* cmd);
void sabr_cmd_get_opt_run(sabr_cmd_t* cmd);
void sabr_cmd_get_opt_bytecode(sabr_cmd_t* cmd);
void sabr_cmd_get_opt_preprocess(sabr_cmd_t* cmd);
void sabr_cmd_get_opt_version(sabr_cmd_t* cmd);
void sabr_cmd_get_opt_help(sabr_cmd_t* cmd);

extern void (*opt_functions[])(sabr_cmd_t* cmd);

void cmd_print_version(sabr_cmd_t* cmd);
void cmd_print_help(sabr_cmd_t* cmd);

#endif