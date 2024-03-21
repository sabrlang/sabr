#include "cmd.h"
#include "interpreter.h"
#include "compiler.h"

sabr_cmd_t cmd = {
	false, false, false, false, false, false, false,
	"", "", "",
	{
		{ "compile", required_argument, NULL, 0 },
		{ "execute", required_argument, NULL, 0 },
		{ "out", required_argument, NULL, 0 },
		{ "run", no_argument, NULL, 0 },
		{ "preprocess", no_argument, NULL, 0 },
		{ "version", no_argument, NULL, 0 },
		{ "help", no_argument, NULL, 0 }
	},
	"c:e:o:rpvh"
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
					strcpy_s(cmd->src_filename, PATH_MAX, optarg);
					cmd->compile = true;
				}
				else if (!strcmp(cmd->long_opts[index].name, "execute")) {
					strcpy_s(cmd->bc_filename, PATH_MAX, optarg);
					cmd->execute = true;
				}
				else if (!strcmp(cmd->long_opts[index].name, "out")) {
					strcpy_s(cmd->out_filename, PATH_MAX, optarg);
					cmd->out = true;
				}
				else if (!strcmp(cmd->long_opts[index].name, "run"))
					cmd->run = true;
				else if (!strcmp(cmd->long_opts[index].name, "preprocess"))
					cmd->preprocess = true;
				else if (!strcmp(cmd->long_opts[index].name, "version"))
					cmd->version = true;
				else if (!strcmp(cmd->long_opts[index].name, "help"))
					cmd->help = true;
				break;
			case 'c':
				strcpy_s(cmd->src_filename, PATH_MAX, optarg);
				cmd->compile = true;
				break;
			case 'e':
				strcpy_s(cmd->bc_filename, PATH_MAX, optarg);
				cmd->execute = true;
				break;
			case 'o':
				strcpy_s(cmd->out_filename, PATH_MAX, optarg);
				cmd->out = true;
				break;
			case 'r': cmd->run = true; break;
			case 'p': cmd->preprocess = true; break;
			case 'v': cmd->version = true; break;
			case 'h': cmd->help = true; break;
			default: break;
		}
	}
}

int sabr_cmd_run(sabr_cmd_t* cmd, sabr_compiler_t* comp, sabr_interpreter_t* inter, int argc, char** argv) {
	sabr_bytecode_t* bc = NULL;
	sabr_cmd_get_opt(cmd, argc, argv);
	if (cmd->version) cmd_print_version(cmd);
	if (cmd->help) cmd_print_help(cmd);
	if (cmd->compile) {
		if (!sabr_compiler_init(comp)) return 1;

		if (!cmd->out)
			strcpy_s(cmd->out_filename, PATH_MAX, cmd->preprocess ? "out.sabrc": "out.sabre");
		if (cmd->preprocess) {
			vector(sabr_token_t)* tokens = sabr_compiler_preprocess_file(comp, cmd->src_filename);
			if (!tokens) return 1;
		}
		else {
			bc = sabr_compiler_compile_file(comp, cmd->src_filename);
			if (!bc) return 1;
			if (!sabr_compiler_save_bytecode(comp, bc, cmd->out_filename)) return 1;
			if (cmd->run) {
				if (!sabr_interpreter_init(inter)) return 1;
				if (!sabr_interpreter_memory_pool_init(inter, 131072, 131072)) return 1;	
				sabr_interpreter_run_bytecode(inter, bc);
				if (!sabr_interpreter_del(inter)) return 1;
			}
		}
		if (!sabr_compiler_del(comp)) return 1;
	}
	else if (cmd->execute) {
		if (!sabr_interpreter_init(inter)) return 1;
		if (!sabr_interpreter_memory_pool_init(inter, 131072, 131072)) return 1;	
		bc = sabr_interpreter_load_bytecode(inter, cmd->bc_filename);
		if (!bc) return 1;
		sabr_interpreter_run_bytecode(inter, bc);
		if (!sabr_interpreter_del(inter)) return 1;
	}
	
	sabr_bytecode_free(bc);
	free(bc);
	return 0;
}

void cmd_print_version(sabr_cmd_t* cmd) {
	printf("Sabr version %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
}

void cmd_print_help(sabr_cmd_t* cmd) {
	puts("Help");
}