#ifndef __WORD_H__
#define __WORD_H__

#include "kwrd.h"
#include "opcode.h"
#include "preproc.h"
#include "token.h"

typedef enum sabr_word_type_enum sabr_word_type_t;
enum sabr_word_type_enum {
	SABR_WT_NONE,
	SABR_WT_PREPROC_KWRD,
	SABR_WT_PREPROC_IDFR,
	SABR_WT_KWRD,
	SABR_WT_OP,
	SABR_WT_IDFR
};

typedef struct sabr_preproc_def_data_struct sabr_preproc_def_data_t;
struct sabr_preproc_def_data_struct {
	sabr_token_t def_code;
	bool is_func;
};

typedef union sabr_word_data_union sabr_word_data_t;
union sabr_word_data_union {
	sabr_preproc_def_data_t preproc_def_data;
	sabr_preproc_keyword_t preproc_kwrd;
	sabr_keyword_t kwrd;
	size_t identifer_index;
	sabr_opcode_t oc;
};

typedef struct sabr_word_struct sabr_word_t;
struct sabr_word_struct {
	sabr_word_type_t type;
	sabr_word_data_t data;
};

#endif