#ifndef __KWRD_H__
#define __KWRD_H__

#include "stddef.h"
#include "stdint.h"

typedef enum sabr_keyword_enum {
	SABR_KWRD_END,
	SABR_KWRD_IF,
	SABR_KWRD_ELSE,
	SABR_KWRD_LOOP,
	SABR_KWRD_WHILE,
	SABR_KWRD_BREAK,
	SABR_KWRD_CONTINUE,
	SABR_KWRD_FOR,
	SABR_KWRD_UFOR,
	SABR_KWRD_FFOR,
	SABR_KWRD_FROM,
	SABR_KWRD_TO,
	SABR_KWRD_STEP,
	SABR_KWRD_SWITCH,
	SABR_KWRD_CASE,
	SABR_KWRD_PASS,
	SABR_KWRD_FUNC,
	SABR_KWRD_MACRO,
	SABR_KWRD_DEFER,
	SABR_KWRD_RETURN,
	SABR_KWRD_STRUCT,
	SABR_KWRD_ENUM,
	SABR_KWRD_MEMBER,
	SABR_KWRD_BRACKET_BEGIN,
	SABR_KWRD_COMMA,
	SABR_KWRD_BRACKET_END
} sabr_keyword_t;

extern size_t sabr_keyword_names_len;
extern const char* sabr_keyword_names[];

typedef struct sabr_keyword_data_struct {
	size_t index;
	size_t pos;
	sabr_keyword_t kwrd;
} sabr_keyword_data_t;

#endif