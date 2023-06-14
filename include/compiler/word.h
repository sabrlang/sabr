#ifndef __WORD_H__
#define __WORD_H__

#include "kwrd.h"
#include "opcode.h"
#include "preproc.h"
#include "token.h"

typedef enum word_type_enum word_type;
enum word_type_enum {
	WT_NONE,
	WT_PREPROC_KWRD,
	WT_PREPROC_IDFR,
	WT_KWRD,
	WT_OP,
	WT_IDFR
};

typedef struct preproc_def_data_struct preproc_def_data;
struct preproc_def_data_struct {
	token def_code;
	bool is_func;
};

typedef union word_data_union word_data;
union word_data_union {
	preproc_def_data def_data;
	preproc_keyword preproc_kwrd;
	keyword kwrd;
	size_t identifer_index;
	opcode oc;
};

typedef struct word_struct word;
struct word_struct {
	word_type type;
	word_data data;
};

#endif