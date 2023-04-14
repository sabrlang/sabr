#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <errno.h>
#include <limits.h>
#include <locale.h>
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

#include "compiler_utils.h"
#include "error_message.h"
#include "preproc.h"
#include "preproc_operation.h"
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

	mbstate_t convert_state;
};


bool sabr_compiler_init(sabr_compiler* const comp);
bool sabr_compiler_del(sabr_compiler* const comp);

bool sabr_compiler_compile_file(sabr_compiler* const comp, const char* filename);
bool sabr_compiler_load_file(sabr_compiler* const comp, const char* filename, size_t* index);

vector(token)* sabr_compiler_preprocess_textcode(sabr_compiler* const comp, size_t textcode_index);
vector(token)* sabr_compiler_preprocess_tokens(sabr_compiler* const comp, vector(token)* input_tokens);
vector(token)* sabr_compiler_tokenize_string(sabr_compiler* const comp, const char* textcode, size_t textcode_index, pos init_pos, bool is_generated);

#endif