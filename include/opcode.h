#ifndef __OPCODE_H__
#define __OPCODE_H__

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

typedef enum sabr_opcode_enum {
	OP_NONE,
	OP_EXIT,

	OP_VALUE,

	OP_IF,
	OP_JUMP,

	OP_FOR,
	OP_FOR_FROM,
	OP_FOR_TO,
	OP_FOR_STEP,
	OP_FOR_CHECK,
	OP_FOR_NEXT,
	OP_FOR_END,

	OP_SWITCH,
	OP_SWITCH_CASE,
	OP_SWITCH_END,

	OP_LAMBDA,
	OP_RETURN,
	OP_LOCAL,
	OP_LOCAL_END,
	OP_DEFINE,

	OP_STRUCT,
	OP_MEMBER,
	OP_STRUCT_END,
	OP_STRUCT_EXEC,
	
	OP_SET,
	OP_EXEC,
	OP_ADDR,
	OP_REF,

	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_UDIV,
	OP_UMOD,
	OP_NEG,
	OP_INC,
	OP_DEC,

	OP_EQU,
	OP_NEQ,
	OP_GRT,
	OP_GEQ,
	OP_LST,
	OP_LEQ,
	OP_UGRT,
	OP_UGEQ,
	OP_ULST,
	OP_ULEQ,

	OP_FADD,
	OP_FSUB,
	OP_FMUL,
	OP_FDIV,
	OP_FMOD,
	OP_FNEG,
	
	OP_FEQU,
	OP_FNEQ,
	OP_FGRT,
	OP_FGEQ,
	OP_FLST,
	OP_FLEQ,

	OP_BAND,
	OP_BOR,
	OP_BXOR,
	OP_BNOT,
	OP_BLSFT,
	OP_BRSFT,

	OP_DROP,
	OP_NIP,
	OP_DUP,
	OP_OVER,
	OP_TUCK,
	OP_SWAP,
	OP_ROT,

	OP_TDROP,
	OP_TNIP,
	OP_TDUP,
	OP_TOVER,
	OP_TTUCK,
	OP_TSWAP,
	OP_TROT,

	OP_ALLOC,
	OP_RESIZE,
	OP_FREE,

	OP_ALLOT,
	
	OP_FETCH,
	OP_STORE,

	OP_ARRAY,
	OP_ARRAY_COMMA,
	OP_ARRAY_END,

	OP_ITOF,
	OP_UTOF,
	OP_FTOI,
	OP_FTOU,

	OP_GETC,
	OP_GETI,
	OP_GETU,
	OP_GETF,
	OP_GETS,

	OP_PUTC,
	OP_PUTI,
	OP_PUTU,
	OP_PUTF,
	OP_PUTS,

	OP_SHOW
} sabr_opcode_t;


extern size_t sabr_opcode_names_len;
extern const char* sabr_opcode_names[];

bool sabr_opcode_has_operand(sabr_opcode_t oc);

#endif