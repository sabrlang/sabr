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

#include "built_in_operation.h"
#include "bytecode.h"
#include "compiler_utils.h"
#include "error_message.h"
#include "kwrd.h"
#include "opcode.h"
#include "preproc.h"
#include "token.h"
#include "utils.h"
#include "word.h"

typedef enum sabr_string_parse_mode_enum {
	STR_PARSE_NONE,
	STR_PARSE_SINGLE,
	STR_PARSE_DOUBLE,
	STR_PARSE_PREPROC
} sabr_string_parse_mode_t;

typedef enum sabr_comment_parse_mode_enum {
	CMNT_PARSE_NONE,
	CMNT_PARSE_LINE,
	CMNT_PARSE_STACK
} sabr_comment_parse_mode_t;

typedef struct sabr_compiler_struct sabr_compiler_t;

struct sabr_compiler_struct {
	trie(size_t) filename_trie;

	vector(cctl_ptr(char)) filename_vector;
	vector(cctl_ptr(char)) textcode_vector;

	trie(sabr_word_t) preproc_dictionary;
	vector(cctl_ptr(trie(sabr_word_t))) preproc_local_dictionary_stack;
	vector(sabr_preproc_stop_flag_t) preproc_stop_stack;

	trie(sabr_word_t) dictionary;
	size_t identifier_count;
	vector(cctl_ptr(vector(sabr_keyword_data_t))) keyword_data_stack;

	size_t tab_size;

	mbstate_t convert_state;
};


bool sabr_compiler_init(sabr_compiler_t* const comp);
bool sabr_compiler_del(sabr_compiler_t* const comp);

sabr_bytecode_t* sabr_compiler_compile_file(sabr_compiler_t* const comp, const char* filename);
bool sabr_compiler_load_file(sabr_compiler_t* const comp, const char* filename, size_t* index);

bool sabr_compiler_save_bytecode(sabr_compiler_t* const comp, sabr_bytecode_t* const bc, const char* filename);

vector(sabr_token_t)* sabr_compiler_preprocess_textcode(sabr_compiler_t* const comp, size_t textcode_index);
vector(sabr_token_t)* sabr_compiler_preprocess_tokens(sabr_compiler_t* const comp, vector(sabr_token_t)* input_tokens, vector(sabr_token_t)* output_tokens);
vector(sabr_token_t)* sabr_compiler_preprocess_eval_token(sabr_compiler_t* const comp, sabr_token_t t, bool has_local, vector(sabr_token_t)* output_tokens);
bool sabr_compiler_preprocess_parse_value(sabr_compiler_t* const comp, sabr_token_t t, sabr_value_t* v);
vector(sabr_token_t)* sabr_compiler_tokenize_string(sabr_compiler_t* const comp, const char* textcode, size_t textcode_index, sabr_pos_t init_pos, bool is_generated);

sabr_bytecode_t* sabr_compiler_compile_tokens(sabr_compiler_t* const comp, vector(sabr_token_t)* tokens);

bool sabr_compiler_parse_zero_begin_num(const char* str, size_t index, bool negate, sabr_value_t* v);
bool sabr_compiler_parse_base_n_num(const char* str, size_t index, bool negate, int base, sabr_value_t* v);
bool sabr_compiler_parse_num(const char* str, sabr_value_t* v);
bool sabr_compiler_parse_identifier(sabr_compiler_t* const comp, const char* str, sabr_value_t* v);
bool sabr_compiler_is_string_can_be_identifier(const char* str);
vector(sabr_value_t)* sabr_compiler_parse_string(sabr_compiler_t* const comp, const char* str);
bool sabr_compiler_parse_string_escape_hex(char** ch_addr, char* num_parse_stop, char* num_parse, size_t* num_parse_count, sabr_value_t* v, int length);
bool sabr_compiler_parse_struct_member(sabr_compiler_t* const comp, const char* str, sabr_value_t* struct_v, sabr_value_t* member_v);

bool sabr_compiler_compile_keyword(sabr_compiler_t* const comp, sabr_bytecode_t* bc_data, sabr_keyword_t kwrd);

sabr_bcop_t* sabr_compiler_get_bcop(sabr_bytecode_t* bc_data, size_t index);

#endif