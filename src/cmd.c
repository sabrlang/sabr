#include "cmd.h"
#include "interpreter.h"
#include "compiler.h"

sabr_cmd_t cmd = {
	{ false, false, false, false, false, false, false, false, false },
	{
		{ "compile", required_argument, NULL, 0 },
		{ "execute", required_argument, NULL, 0 },
		{ "out", required_argument, NULL, 0 },
		{ "memory", required_argument, NULL, 0 },
		{ "run", no_argument, NULL, 0 },
		{ "bytecode", no_argument, NULL, 0 },
		{ "preprocess", no_argument, NULL, 0 },
		{ "version", no_argument, NULL, 0 },
		{ "help", no_argument, NULL, 0 }
	},
	"c:e:o:m:rbpvh",
	"", "", "",
	1048576
};

size_t sabr_cmd_opts_len = sizeof(cmd.long_opts) / sizeof(option_t);

void sabr_cmd_get_opt(sabr_cmd_t* cmd, int argc, char** argv) {
	int opt;
	int index;
	if (argc == 1) {
		cmd->flags.help = true;
		cmd->flags.version = true;
		return;
	}
	while ((opt = getopt_long(argc, argv, cmd->opts, cmd->long_opts, &index)) != -1) {
		switch (opt) {
			case 0: opt_functions[index](cmd); break;
			case 'c': sabr_cmd_get_opt_compile(cmd); break;
			case 'e': sabr_cmd_get_opt_execute(cmd); break;
			case 'o': sabr_cmd_get_opt_out(cmd); break;
			case 'm': sabr_cmd_get_opt_memory(cmd); break;
			case 'r': sabr_cmd_get_opt_run(cmd); break;
			case 'b': sabr_cmd_get_opt_bytecode(cmd); break;
			case 'p': sabr_cmd_get_opt_preprocess(cmd); break;
			case 'v': sabr_cmd_get_opt_version(cmd); break;
			case 'h': sabr_cmd_get_opt_help(cmd); break;
			default: break;
		}
	}
}

int sabr_cmd_run(sabr_cmd_t* cmd, sabr_compiler_t* comp, sabr_interpreter_t* inter, int argc, char** argv) {
	int result = 1;
	sabr_bytecode_t* bc = NULL;
	sabr_bytecode_t* std_lib_bc = NULL;
	sabr_bytecode_t* src_bc = NULL;

	sabr_cmd_get_opt(cmd, argc, argv);
	if (cmd->flags.version) cmd_print_version(cmd);
	if (cmd->flags.help) cmd_print_help(cmd);
	if (cmd->flags.compile) {

		if (!sabr_compiler_init(comp)) goto FAILURE;

		char std_lib_path[PATH_MAX];
	#if defined(_WIN32)
		if (!sabr_get_std_lib_path(std_lib_path, "std", true, &comp->convert_state)) goto FAILURE;
	#else
		if (!sabr_get_std_lib_path(std_lib_path, "std", true)) goto FAILURE;
	#endif
		std_lib_bc = sabr_compiler_compile_file(comp, std_lib_path);
		if (!std_lib_bc) goto FAILURE;

		if (!cmd->flags.out)
			strncpy(cmd->out_filename, cmd->flags.preprocess ? "out.sabrc": "out.sabre", PATH_MAX);
		if (cmd->flags.preprocess) {
			vector(sabr_token_t)* tokens = sabr_compiler_preprocess_file(comp, cmd->src_filename);
			if (!tokens) goto FAILURE;
		}
		else {
			src_bc = sabr_compiler_compile_file(comp, cmd->src_filename);
			if (!src_bc) goto FAILURE;

			bc = sabr_bytecode_concat(std_lib_bc, src_bc);

			if (cmd->flags.bytecode) sabr_bytecode_print(bc);
			if (!sabr_compiler_save_bytecode(comp, bc, cmd->out_filename)) goto FAILURE;
			if (cmd->flags.run) {
				if (!sabr_interpreter_init(inter)) goto FAILURE;
				if (!sabr_interpreter_memory_pool_init(inter, cmd->memory_pool_size, cmd->memory_pool_size)) goto FAILURE;
				sabr_interpreter_run_bytecode(inter, bc);
				if (!sabr_interpreter_del(inter)) goto FAILURE;
			}
		}
		if (!sabr_compiler_del(comp)) goto FAILURE;

	}
	else if (cmd->flags.execute) {
		if (!sabr_interpreter_init(inter)) goto FAILURE;
		if (!sabr_interpreter_memory_pool_init(inter, cmd->memory_pool_size, cmd->memory_pool_size)) goto FAILURE;
		bc = sabr_interpreter_load_bytecode(inter, cmd->bc_filename);
		if (!bc) goto FAILURE;
		sabr_interpreter_run_bytecode(inter, bc);
		if (!sabr_interpreter_del(inter)) goto FAILURE;
	}
	result = 0;

FAILURE:
	sabr_bytecode_free(std_lib_bc);
	sabr_bytecode_free(src_bc);
	free(std_lib_bc);
	free(src_bc);
	sabr_bytecode_free(bc);
	free(bc);
	return result;
}

void sabr_cmd_get_opt_compile(sabr_cmd_t* cmd) {
	strncpy(cmd->src_filename, optarg, PATH_MAX);
	cmd->flags.compile = true;
}

void sabr_cmd_get_opt_execute(sabr_cmd_t* cmd) {
	strncpy(cmd->bc_filename, optarg, PATH_MAX);
	cmd->flags.execute = true;
}

void sabr_cmd_get_opt_out(sabr_cmd_t* cmd) {
	strncpy(cmd->out_filename, optarg, PATH_MAX);
	cmd->flags.out = true;
}

void sabr_cmd_get_opt_memory(sabr_cmd_t* cmd) {
	cmd->flags.memory = true;
}

void sabr_cmd_get_opt_run(sabr_cmd_t* cmd) {
	cmd->flags.run = true;
}

void sabr_cmd_get_opt_bytecode(sabr_cmd_t* cmd) {
	cmd->flags.bytecode = true;
}

void sabr_cmd_get_opt_preprocess(sabr_cmd_t* cmd) {
	cmd->flags.preprocess = true;
}

void sabr_cmd_get_opt_version(sabr_cmd_t* cmd) {
	cmd->flags.version = true;
}

void sabr_cmd_get_opt_help(sabr_cmd_t* cmd) {
	cmd->flags.help = true;
}

void (*opt_functions[])(sabr_cmd_t* cmd) = {
	sabr_cmd_get_opt_compile,
	sabr_cmd_get_opt_execute,
	sabr_cmd_get_opt_out,
	sabr_cmd_get_opt_memory,
	sabr_cmd_get_opt_run,
	sabr_cmd_get_opt_bytecode,
	sabr_cmd_get_opt_preprocess,
	sabr_cmd_get_opt_version,
	sabr_cmd_get_opt_help
};

void cmd_print_version(sabr_cmd_t* cmd) {
	printf("Sabr version %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
}

void cmd_print_help(sabr_cmd_t* cmd) {
	puts("Help");
}