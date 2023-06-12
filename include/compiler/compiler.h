#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#if defined(_WIN32)
	#include <windows.h>
	#include <io.h>
#else
	#include <libgen.h>
	#include <unistd.h>
	#if defined(__linux__)
		#include <linux/limits.h>
	#endif
#endif

#include "console.h"
#include "value.h"

#include "compiler_cctl_define.h"
#include "cctl_define.h"

#include "bytecode.h"
#include "compiler_utils.h"
#include "error_message.h"
#include "kwrd.h"
#include "opcode.h"
#include "preproc.h"
#include "token.h"
#include "word.h"

typedef enum string_parse_mode_enum {
	STR_PARSE_NONE,
	STR_PARSE_SINGLE,
	STR_PARSE_DOUBLE,
	STR_PARSE_PREPROC
} string_parse_mode;

typedef enum comment_parse_mode_enum {
	CMNT_PARSE_NONE,
	CMNT_PARSE_LINE,
	CMNT_PARSE_STACK
} comment_parse_mode;

typedef struct sabr_compiler_struct sabr_compiler;

struct sabr_compiler_struct {
	trie(size_t) filename_trie;

	vector(cctl_ptr(char)) filename_vector;
	vector(cctl_ptr(char)) textcode_vector;

	trie(word) preproc_dictionary;
	vector(cctl_ptr(trie(word))) preproc_local_dictionary_stack;
	vector(preproc_stop_flag) preproc_stop_stack;

	trie(word) dictionary;
	size_t identifier_count;
	vector(cctl_ptr(vector(keyword_data))) keyword_data_stack;

	size_t tab_size;

	mbstate_t convert_state;
};


bool sabr_compiler_init(sabr_compiler* const comp);
bool sabr_compiler_del(sabr_compiler* const comp);

bool sabr_compiler_compile_file(sabr_compiler* const comp, const char* filename);
bool sabr_compiler_load_file(sabr_compiler* const comp, const char* filename, size_t* index);

vector(token)* sabr_compiler_preprocess_textcode(sabr_compiler* const comp, size_t textcode_index);
vector(token)* sabr_compiler_preprocess_tokens(sabr_compiler* const comp, vector(token)* input_tokens, vector(token)* output_tokens);
vector(token)* sabr_compiler_preprocess_eval_token(sabr_compiler* const comp, token t, bool has_local, vector(token)* output_tokens);
bool sabr_compiler_preprocess_parse_value(sabr_compiler* const comp, token t, value* v);
vector(token)* sabr_compiler_tokenize_string(sabr_compiler* const comp, const char* textcode, size_t textcode_index, pos init_pos, bool is_generated);

bytecode* sabr_compiler_compile_tokens(sabr_compiler* const comp, vector(token)* tokens);

bool sabr_compiler_parse_zero_begin_num(const char* str, size_t index, bool negate, value* v);
bool sabr_compiler_parse_base_n_num(const char* str, size_t index, bool negate, int base, value* v);
bool sabr_compiler_parse_num(const char* str, value* v);
bool sabr_compiler_parse_identifier(sabr_compiler* const comp, const char* str, value* v);
bool sabr_compiler_is_string_can_be_identifier(const char* str);
vector(value)* sabr_compiler_parse_string(sabr_compiler* const comp, const char* str);
bool sabr_compiler_parse_string_escape_hex(char** ch_addr, char* num_parse_stop, char* num_parse, size_t* num_parse_count, value* v, int length);
bool sabr_compiler_parse_struct_member(sabr_compiler* const comp, const char* str, value* struct_v, value* member_v);

bool sabr_compiler_compile_keyword(sabr_compiler* const comp, bytecode* bc_data, keyword kwrd);

bool sabr_compiler_write_bytecode(bytecode* bc_data, opcode oc);
bool sabr_compiler_write_bytecode_with_null(bytecode* bc_data, opcode oc);
bool sabr_compiler_write_bytecode_with_value(bytecode* bc_data, opcode oc, value v);

bytecode_operation* sabr_compiler_get_bytecode_operation(bytecode* bc_data, size_t index);

#endif