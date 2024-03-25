#include "interpreter.h"
#include "interpreter_op.h"

extern inline sabr_value_t* sabr_memory_pool_top(sabr_memory_pool_t* pool);
extern inline sabr_local_data_t* sabr_interpreter_get_local_data(sabr_interpreter_t* inter);

bool sabr_interpreter_init(sabr_interpreter_t* inter) {
    deque_init(sabr_value_t, &inter->data_stack);
    deque_init(sabr_value_t, &inter->switch_stack);
    deque_init(sabr_for_data_t, &inter->for_data_stack);
	deque_init(sabr_local_data_t, &inter->local_data_stack);
    deque_init(sabr_cs_data_t, &inter->call_stack);

    rbt_init(sabr_def_data_t, &inter->global_words);

    vector_init(cctl_ptr(vector(sabr_value_t)), &inter->struct_vector);
    vector_init(cctl_ptr(vector(sabr_value_t)), &inter->array_vector);

    return true;
}

bool sabr_interpreter_del(sabr_interpreter_t* inter) {
    deque_free(sabr_value_t, &inter->data_stack);
    deque_free(sabr_value_t, &inter->switch_stack);
    deque_free(sabr_for_data_t, &inter->for_data_stack);
	for (size_t i = 0; i < inter->local_data_stack.size; i++)
		rbt_free(sabr_def_data_t, deque_at(sabr_local_data_t, &inter->local_data_stack, i)->local_words);
	deque_free(sabr_local_data_t, &inter->local_data_stack);
    deque_free(sabr_cs_data_t, &inter->call_stack);

    rbt_free(sabr_def_data_t, &inter->global_words);

    for (size_t i = 0; i < inter->struct_vector.size; i++) {
		vector(sabr_value_t)* struct_data = *vector_at(cctl_ptr(vector(sabr_value_t)), &inter->struct_vector, i);
        vector_free(sabr_value_t, struct_data);
		free(struct_data);
	}
    vector_free(cctl_ptr(vector(sabr_value_t)), &inter->struct_vector);

    for (size_t i = 0; i < inter->array_vector.size; i++){
		vector(sabr_value_t)* array_data = *vector_at(cctl_ptr(vector(sabr_value_t)), &inter->array_vector, i);
        vector_free(sabr_value_t, array_data);
		free(array_data);
	}
    vector_free(cctl_ptr(vector(sabr_value_t)), &inter->array_vector);

	sabr_memory_pool_del(&inter->memory_pool);
	sabr_memory_pool_del(&inter->global_memory_pool);

    return true;
}

sabr_bytecode_t* sabr_interpreter_load_bytecode(sabr_interpreter_t* inter, const char* filename) {
	FILE* file;
	size_t size;

#if defined(_WIN32)
	wchar_t filename_windows[PATH_MAX] = {0, };
	if (!sabr_convert_string_mbr2c16(filename, filename_windows, &(inter->convert_state))) {
		fputs(sabr_errmsg_open, stderr);
		return NULL;
	}
	if (_waccess(filename_windows, R_OK)) {
		fputs(sabr_errmsg_open, stderr);
		return NULL;
	}
	file = _wfopen(filename_windows, L"rb");
#else
	if (access(filename, R_OK)) {
		fputs(sabr_errmsg_open, stderr);
		return NULL;
	}
	file = fopen(filename, "rb");
#endif

	if (!file) {
		fputs(sabr_errmsg_open, stderr);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);

	uint8_t* code = (uint8_t*) malloc(size);
	if (!code) {
		fclose(file);
		fputs(sabr_errmsg_alloc, stderr);
		return NULL;
	}

	int a = fread(code, size, 1, file);
	fclose(file);

	if (a != 1) {
		free(code);
		fputs(sabr_errmsg_read, stderr);
		return NULL;
	}

	sabr_bytecode_t* bc = (sabr_bytecode_t*) malloc(sizeof(sabr_bytecode_t));
	if (!bc) {
		free(code);
		fputs(sabr_errmsg_alloc, stderr);
		return NULL;
	}
	sabr_bytecode_init(bc);

	size_t index = 0;
	while (index < size) {
		sabr_bcop_t bcop;
		bcop.oc = code[index++];
		if (sabr_opcode_has_operand(bcop.oc)) {
			for (size_t i = 0; i < 8; i++) bcop.operand.bytes[i] = code[index++];
			sabr_bytecode_write_bcop_with_value(bc, bcop.oc, bcop.operand);
		}
		else sabr_bytecode_write_bcop(bc, bcop.oc);
	}

	free(code);

	return bc;
}

bool sabr_interpreter_run_bytecode(sabr_interpreter_t* inter, sabr_bytecode_t* bc) {
	for (size_t index = 0; index < bc->bcop_vec.size; index++) {
		sabr_bcop_t bcop = *vector_at(sabr_bcop_t, &bc->bcop_vec, index);
		if (bcop.oc == SABR_OP_NONE) continue;
		uint32_t result = interpreter_op_functions[bcop.oc - 1](inter, bcop, &index);
		if (result) {
			fprintf(stderr, "result: %u, index: %zu\n", result, index);
			return false;
		};
	}
	return true;
}

bool sabr_interpreter_memory_pool_init(sabr_interpreter_t* inter, size_t size, size_t global_size) {
	if (!sabr_memory_pool_init(&inter->memory_pool, size)) return false;
	if (!sabr_memory_pool_init(&inter->global_memory_pool, global_size)) return false;
	return true;
}

bool sabr_memory_pool_init(sabr_memory_pool_t* pool, size_t size) {
	pool->data = (sabr_value_t*) malloc(sizeof(size_t) * size);
	pool->size = size;
	pool->index = 0;
	if (pool->data) return true;
	return false;
}

void sabr_memory_pool_del(sabr_memory_pool_t* pool) {
	free(pool->data);
	pool->data = NULL;
}

bool sabr_memory_pool_alloc(sabr_memory_pool_t* pool, size_t size) {
	if (pool->index + size >= pool->size) return false;
	pool->index += size;
	return true;
}

bool sabr_memory_pool_free(sabr_memory_pool_t* pool, size_t size) {
	if ((pool->index - size) && (pool->index - size < pool->index)) return false;
	pool->index -= size;
	return true;
}

bool sabr_interpreter_pop(sabr_interpreter_t* inter, sabr_value_t* v) {
	if (!inter->data_stack.size) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		return false;
	}
	*v = *deque_back(sabr_value_t, &inter->data_stack);
	deque_pop_back(sabr_value_t, &inter->data_stack);
	return true;
}

bool sabr_interpreter_push(sabr_interpreter_t* inter, sabr_value_t v) {
	if (!deque_push_back(sabr_value_t, &inter->data_stack, v)) {
		fputs(sabr_errmsg_alloc, stderr);
		return false;
	}
	return true;
}

uint32_t sabr_interpreter_exec_identifier(sabr_interpreter_t* inter, sabr_value_t identifier, size_t* index) {
	sabr_def_data_t* def_data = NULL;
	rbt(sabr_def_data_t)* local_words = NULL;
	def_data = rbt_find(sabr_def_data_t, &inter->global_words, identifier.u);
	if (!def_data) {
		if (inter->local_data_stack.size > 0) {
			local_words = sabr_interpreter_get_local_data(inter)->local_words;
			def_data = rbt_find(sabr_def_data_t, local_words, identifier.u);
		}
	}
	if (!def_data) return SABR_OPERR_UNDEFINED;

	sabr_cs_data_t csd;

	switch (def_data->dety) {
		case SABR_DETY_NONE: break;
		case SABR_DETY_CALLABLE: {
			csd.pos = *index + 1;
			if (!deque_push_back(sabr_cs_data_t, &inter->call_stack, csd)) return SABR_OPERR_WHAT;
			*index = def_data->data - 1;
		} break;
		case SABR_DETY_VARIABLE: {
			sabr_value_t v;
			sabr_value_t* p = (sabr_value_t*) def_data->data;
			v.u = p->u;
			if (!sabr_interpreter_push(inter, v)) return SABR_OPERR_STACK;
		} break;
		case SABR_DETY_STRUCT: {
			sabr_value_t v;
			vector(sabr_value_t)* struct_data_vector = *vector_at(cctl_ptr(vector(sabr_value_t)), &inter->struct_vector, def_data->data);
			if (!struct_data_vector) return SABR_OPERR_WHAT;
			v.u = struct_data_vector->size * sizeof(sabr_value_t);
			if (!sabr_interpreter_push(inter, v)) return SABR_OPERR_STACK;
		} break;
	}
	return SABR_OPERR_NONE;
}

uint32_t sabr_interpreter_set_variable(sabr_interpreter_t* inter, sabr_value_t identifier, sabr_value_t v) {
	rbt(sabr_def_data_t)* words = NULL;

	sabr_def_data_t* def_data = NULL;
	sabr_def_data_t new_def_data;

	bool is_global = false;

	def_data = rbt_find(sabr_def_data_t, &inter->global_words, identifier.u);
	if (!def_data) {
		if (inter->local_data_stack.size > 0) {
			words = sabr_interpreter_get_local_data(inter)->local_words;
			def_data = rbt_find(sabr_def_data_t, words, identifier.u);
		}
		else {
			words = &inter->global_words;
			is_global = true;
		}
	}
	if (!def_data) {
		def_data = &new_def_data;

		sabr_value_t* p;
		if (is_global) {
			p = sabr_memory_pool_top(&inter->global_memory_pool);
			if (!sabr_memory_pool_alloc(&inter->global_memory_pool, 1)) return SABR_OPERR_MEMORY;
		}
		else {
			p = sabr_memory_pool_top(&inter->memory_pool);
			if (!sabr_memory_pool_alloc(&inter->memory_pool, 1)) return SABR_OPERR_MEMORY;
			size_t* local_memory_size = &(sabr_interpreter_get_local_data(inter)->local_memory_size);
			(*local_memory_size)++;
		}
		new_def_data.data = (size_t) p;
		new_def_data.dety = SABR_DETY_VARIABLE;

		if (!rbt_insert(sabr_def_data_t, words, identifier.u, new_def_data)) return SABR_OPERR_MEMORY;
	}

	if (def_data->dety == SABR_DETY_VARIABLE) {
		sabr_value_t* p = (sabr_value_t*) def_data->data;
		p->u = v.u;
	}
	else return SABR_OPERR_INVALID_IDENT;

	return SABR_OPERR_NONE;
}

uint32_t sabr_interpreter_ref_variable(sabr_interpreter_t* inter, sabr_value_t identifier, sabr_value_t* addr) {
	rbt(sabr_def_data_t)* words = NULL;

	sabr_def_data_t* def_data = NULL;
	sabr_def_data_t new_def_data;

	def_data = rbt_find(sabr_def_data_t, &inter->global_words, identifier.u);
	if (!def_data) {
		if (inter->local_data_stack.size > 0) {
			words = sabr_interpreter_get_local_data(inter)->local_words;
			def_data = rbt_find(sabr_def_data_t, words, identifier.u);
		}
		else {
			words = &inter->global_words;
		}
	}
	if (!def_data) {
		def_data = &new_def_data;
		new_def_data.dety = SABR_DETY_VARIABLE;
		new_def_data.data = (size_t) addr;

		if (!rbt_insert(sabr_def_data_t, words, identifier.u, new_def_data)) return SABR_OPERR_MEMORY;
	}
	else return SABR_OPERR_REDEFINE;

	return SABR_OPERR_NONE;
}

sabr_value_t* sabr_interpreter_get_variable_addr(sabr_interpreter_t* inter, sabr_value_t identifier) {
	rbt(sabr_def_data_t)* words = NULL;

	sabr_def_data_t* def_data = NULL;
	def_data = rbt_find(sabr_def_data_t, &inter->global_words, identifier.u);
	if (!def_data) {
		if (inter->local_data_stack.size > 0) {
			words = sabr_interpreter_get_local_data(inter)->local_words;
			def_data = rbt_find(sabr_def_data_t, words, identifier.u);
		}
		else {
			words = &inter->global_words;
		}
	}
	if (!def_data) return NULL;
	if (def_data->dety != SABR_DETY_VARIABLE) return NULL;
	return (sabr_value_t*) def_data->data;
}

bool sabr_interpreter_putc(sabr_interpreter_t* inter, sabr_value_t character) {
	if (character.u < 127) putchar(character.u);
	else {
		char out[8];
		size_t rc = c32rtomb(out, (char32_t) character.u, &(inter->convert_state));
		if (rc == -1) return false;
		out[rc] = 0;
		fputs(out, stdout);
	}
	return true;
}