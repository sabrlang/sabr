#include "cmd.h"
#include "interpreter.h"
#include "compiler.h"

sabr_cmd_t cmd = {
	false, false, false, false, false, false, false, false,
	"", "", "",
	{
		{ "compile", required_argument, NULL, 0 },
		{ "execute", required_argument, NULL, 0 },
		{ "out", required_argument, NULL, 0 },
		{ "run", no_argument, NULL, 0 },
		{ "bytecode", no_argument, NULL, 0 },
		{ "preprocess", no_argument, NULL, 0 },
		{ "version", no_argument, NULL, 0 },
		{ "help", no_argument, NULL, 0 }
	},
	"c:e:o:rbpvh"
};

void sabr_cmd_get_opt(sabr_cmd_t* cmd, int argc, char** argv) {
	int opt;
	int index;
	if (argc == 1) {
		cmd->help = true;
		cmd->version = true;
		return;
	}
	while ((opt = getopt_long(argc, argv, cmd->opts, cmd->long_opts, &index)) != -1) {
		switch (opt) {
			case 0:
				if (!strcmp(cmd->long_opts[index].name, "compile")) {
					strncpy(cmd->src_filename, optarg, PATH_MAX);
					cmd->compile = true;
				}
				else if (!strcmp(cmd->long_opts[index].name, "execute")) {
					strncpy(cmd->bc_filename, optarg, PATH_MAX);
					cmd->execute = true;
				}
				else if (!strcmp(cmd->long_opts[index].name, "out")) {
					strncpy(cmd->out_filename, optarg, PATH_MAX);
					cmd->out = true;
				}
				else if (!strcmp(cmd->long_opts[index].name, "run"))
					cmd->run = true;
				else if (!strcmp(cmd->long_opts[index].name, "bytecode"))
					cmd->bytecode = true;
				else if (!strcmp(cmd->long_opts[index].name, "preprocess"))
					cmd->preprocess = true;
				else if (!strcmp(cmd->long_opts[index].name, "version"))
					cmd->version = true;
				else if (!strcmp(cmd->long_opts[index].name, "help"))
					cmd->help = true;
				break;
			case 'c':
				strncpy(cmd->src_filename, optarg, PATH_MAX);
				cmd->compile = true;
				break;
			case 'e':
				strncpy(cmd->bc_filename, optarg, PATH_MAX);
				cmd->execute = true;
				break;
			case 'o':
				strncpy(cmd->out_filename, optarg, PATH_MAX);
				cmd->out = true;
				break;
			case 'r': cmd->run = true; break;
			case 'b': cmd->bytecode = true; break;
			case 'p': cmd->preprocess = true; break;
			case 'v': cmd->version = true; break;
			case 'h': cmd->help = true; break;
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
	if (cmd->version) cmd_print_version(cmd);
	if (cmd->help) cmd_print_help(cmd);
	if (cmd->compile) {

		if (!sabr_compiler_init(comp)) goto FAILURE;

		char std_lib_path[PATH_MAX];
	#if defined(_WIN32)
		if (!sabr_get_std_lib_path(std_lib_path, "std", true, &comp->convert_state)) goto FAILURE;
	#else
		if (!sabr_get_std_lib_path(std_lib_path, "std", true)) goto FAILURE;
	#endif
		std_lib_bc = sabr_compiler_compile_file(comp, std_lib_path);
		if (!std_lib_bc) goto FAILURE;

		if (!cmd->out)
			strncpy(cmd->out_filename, cmd->preprocess ? "out.sabrc": "out.sabre", PATH_MAX);
		if (cmd->preprocess) {
			vector(sabr_token_t)* tokens = sabr_compiler_preprocess_file(comp, cmd->src_filename);
			if (!tokens) goto FAILURE;
		}
		else {
			src_bc = sabr_compiler_compile_file(comp, cmd->src_filename);
			if (!src_bc) goto FAILURE;

			bc = sabr_bytecode_concat(std_lib_bc, src_bc);

			if (cmd->bytecode) sabr_bytecode_print(bc);
			if (!sabr_compiler_save_bytecode(comp, bc, cmd->out_filename)) goto FAILURE;
			if (cmd->run) {
				if (!sabr_interpreter_init(inter)) goto FAILURE;
				if (!sabr_interpreter_memory_pool_init(inter, 131072, 131072)) goto FAILURE;
				sabr_interpreter_run_bytecode(inter, bc);
				if (!sabr_interpreter_del(inter)) goto FAILURE;
			}
		}
		if (!sabr_compiler_del(comp)) goto FAILURE;

	}
	else if (cmd->execute) {
		if (!sabr_interpreter_init(inter)) goto FAILURE;
		if (!sabr_interpreter_memory_pool_init(inter, 131072, 131072)) goto FAILURE;
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

void cmd_print_version(sabr_cmd_t* cmd) {
	printf("Sabr version %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
}

void cmd_print_help(sabr_cmd_t* cmd) {
	puts("Help");
}