#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct sabr_pos_struct {
	size_t line;
	size_t column;
} sabr_pos_t;

typedef struct sabr_token_struct {
	char* data;
	sabr_pos_t begin_pos;
	sabr_pos_t end_pos;
	size_t begin_index;
	size_t end_index;
	size_t textcode_index;
	bool is_generated;
} sabr_token_t;

#endif