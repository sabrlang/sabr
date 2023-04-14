#ifndef __COMPILER_UTILS_H__
#define __COMPILER_UTILS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler_cctl_define.h"

#include "token.h"

char* sabr_new_string_slice(const char* source, size_t begin_index, size_t end_index);
char* sabr_new_string_copy(const char* source);
char* sabr_new_string_append(const char* dest, const char* origin);

void sabr_free_token_vector(vector(token)* tokens);

void sabr_free_word_trie(trie(word)* dictionary);

#endif