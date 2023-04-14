#ifndef __WORD_H__
#define __WORD_H__

#include "token.h"
#include "preproc.h"

typedef enum word_type_enum word_type;
enum word_type_enum {
	WT_NONE,
	WT_PREPROC_KWRD,
	WT_PREPROC_IDFR,
	WT_KWRD,
	WT_OP,
	WT_IDFR
};

typedef union word_data_union word_data;
union word_data_union {
	token macro_code;
	preproc_keyword p_kwrd;
};

typedef struct word_struct word;
struct word_struct {
	word_type type;
	word_data data;
};

#endif