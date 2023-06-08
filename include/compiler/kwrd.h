#ifndef __KWRD_H__
#define __KWRD_H__

#include "stddef.h"
#include "stdint.h"

typedef enum keyword_enum {
	KWRD_END,
	KWRD_IF,
	KWRD_ELSE,
	KWRD_LOOP,
	KWRD_WHILE,
	KWRD_BREAK,
	KWRD_CONTINUE,
	KWRD_FOR,
	KWRD_UFOR,
	KWRD_FFOR,
	KWRD_FROM,
	KWRD_TO,
	KWRD_STEP,
	KWRD_SWITCH,
	KWRD_CASE,
	KWRD_PASS,
	KWRD_FUNC,
	KWRD_MACRO,
	KWRD_DEFER,
	KWRD_RETURN,
	KWRD_STRUCT,
	KWRD_MEMBER,
	KWRD_BRACKET_BEGIN,
	KWRD_COMMA,
	KWRD_BRACKET_END
} keyword;

extern size_t keyword_names_len;
extern const char* keyword_names[];

typedef struct keyword_data_struct {
	size_t index;
	size_t pos;
	keyword kwrd;
} keyword_data;

#endif