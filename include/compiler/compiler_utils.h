#ifndef __COMPILER_UTILS_H__
#define __COMPILER_UTILS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uchar.h>

#include "compiler_cctl_define.h"

#include "token.h"
#include "bytecode.h"

char* sabr_new_string_slice(const char* source, size_t begin_index, size_t end_index);
char* sabr_new_string_copy(const char* source);
char* sabr_new_string_append(const char* dest, const char* origin);

void sabr_free_token_vector(vector(token)* tokens);

void sabr_free_word_trie(trie(word)* dictionary);

void sabr_free_bytecode(bytecode* bc);

#if defined (_WIN32)
    bool sabr_convert_string_mbr2c16(const char* src, wchar_t* dest, mbstate_t* convert_state);
    bool sabr_get_full_path(const char* src_mbr, char* dest_mbr, wchar_t* dest_utf16, mbstate_t* convert_state);
#else
    bool sabr_get_full_path(const char* src, char* dest);
#endif

#endif