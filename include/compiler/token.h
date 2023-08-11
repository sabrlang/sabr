#ifndef __TOKEN_H__
#define __TOKEN_H__

// #include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct pos_struct {
	size_t line;
	size_t column;
} pos;

typedef struct token_struct {
	char* data;
	pos begin_pos;
	pos end_pos;
	size_t begin_index;
	size_t end_index;
	size_t textcode_index;
	bool is_generated;
} token;

#endif