#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <inttypes.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <uchar.h>

#if defined(_WIN32)
	#include <fcntl.h>
	#include <io.h>
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
#endif

#include "opcode.h"
#include "rbt.h"
#include "value.h"
#include "bytecode.h"
#include "error_message.h"
// #include "encoding.h"
#include "utils.h"

#include "interpreter_cctl_define.h"
#include "interpreter_data.h"

typedef struct sabr_memory_pool_struct sabr_memory_pool_t;
struct sabr_memory_pool_struct {
    sabr_value_t* data;
	size_t size;
	size_t index;
};

typedef struct sabr_interpreter_struct sabr_interpreter_t;
struct sabr_interpreter_struct {
	sabr_bytecode_t* bc;

	mbstate_t convert_state;

	deque(sabr_value_t) data_stack;

	deque(sabr_value_t) switch_stack;
	deque(sabr_for_data_t) for_data_stack;
	deque(sabr_local_data_t) local_data_stack;

	deque(sabr_cs_data_t) call_stack;

	sabr_memory_pool_t memory_pool;
	sabr_memory_pool_t global_memory_pool;

	rbt(sabr_def_data_t) global_words;

	vector(cctl_ptr(vector(sabr_value_t))) struct_vector;
	vector(cctl_ptr(vector(sabr_value_t))) array_vector;
};

bool sabr_interpreter_init(sabr_interpreter_t* inter);
bool sabr_interpreter_del(sabr_interpreter_t* inter);
bool sabr_interpreter_memory_pool_init(sabr_interpreter_t* inter, size_t size, size_t global_size);

sabr_bytecode_t* sabr_interpreter_load_bytecode(sabr_interpreter_t* inter, const char* filename);
bool sabr_interpreter_run_bytecode(sabr_interpreter_t* inter, sabr_bytecode_t* bc);

bool sabr_memory_pool_init(sabr_memory_pool_t* pool, size_t size);
void sabr_memory_pool_del(sabr_memory_pool_t* pool);
bool sabr_memory_pool_alloc(sabr_memory_pool_t* pool, size_t size);
bool sabr_memory_pool_free(sabr_memory_pool_t* pool, size_t size);
inline sabr_value_t* sabr_memory_pool_top(sabr_memory_pool_t* pool) {
	return pool->data + pool->index;
}

bool sabr_interpreter_pop(sabr_interpreter_t* inter, sabr_value_t* v);
bool sabr_interpreter_push(sabr_interpreter_t* inter, sabr_value_t v);

uint32_t sabr_interpreter_exec_identifier(sabr_interpreter_t* inter, sabr_value_t identifier, size_t* index);
uint32_t sabr_interpreter_set_variable(sabr_interpreter_t* inter, sabr_value_t identifier, sabr_value_t v);
uint32_t sabr_interpreter_ref_variable(sabr_interpreter_t* inter, sabr_value_t identifier, sabr_value_t* addr);
sabr_value_t* interpreter_get_variable_addr(sabr_interpreter_t* inter, sabr_value_t identifier);

inline sabr_local_data_t* sabr_interpreter_get_local_data(sabr_interpreter_t* inter) {
	return deque_back(sabr_def_data_t, &inter->local_data_stack);
}

#endif