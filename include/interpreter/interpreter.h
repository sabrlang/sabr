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

typedef struct sabr_memory_pool_struct sabr_memory_pool;
struct sabr_memory_pool_struct {
    value* data;
	size_t size;
	size_t index;
};

typedef struct sabr_interpreter_struct sabr_interpreter;
struct sabr_interpreter_struct {
	bytecode* bc;

	mbstate_t convert_state;

	deque(value) data_stack;
	deque(value) switch_stack;

	deque(for_data) for_data_stack;
	deque(cs_data) call_stack;

	sabr_memory_pool memory_pool;
	sabr_memory_pool global_memory_pool;
	deque(size_t) local_memory_size_stack;

	rbt(def_data) global_words;
	deque(cctl_ptr(rbt(def_data))) local_words_stack;

	vector(cctl_ptr(vector(value))) struct_vector;
};

bool sabr_interpreter_init(sabr_interpreter* inter);
bool sabr_interpreter_del(sabr_interpreter* inter);
bool sabr_interpreter_memory_pool_init(sabr_interpreter* inter, size_t size, size_t global_size);

bytecode* sabr_interpreter_load_bytecode(sabr_interpreter* inter, const char* filename);
bool sabr_interpreter_run_bytecode(sabr_interpreter* inter, bytecode* bc);

bool sabr_memory_pool_init(sabr_memory_pool* pool, size_t size);
void sabr_memory_pool_del(sabr_memory_pool* pool);
bool sabr_memory_pool_alloc(sabr_memory_pool* pool, size_t size);
bool sabr_memory_pool_free(sabr_memory_pool* pool, size_t size);
inline value* sabr_memory_pool_top(sabr_memory_pool* pool) {
	return pool->data + pool->index;
}

#endif