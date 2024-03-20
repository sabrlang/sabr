#include "built_in_operation.h"

const char* sabr_bio_names[] = {
	"exit",
	"exec",
	"define",
	"set",
	"addr",
	"ref",

	"+",
	"-",
	"*",
	"/",
	"%",
	"u/",
	"u%",
	"neg",
	"++",
	"--",

	"=",
	"!=",
	">",
	">=",
	"<",
	"<=",

	"u>",
	"u>=",
	"u<",
	"u<=",

	"f+",
	"f-",
	"f*",
	"f/",
	"f%",
	"fneg",

	"f=",
	"f!=",
	"f>",
	"f>=",
	"f<",
	"f<=",

	"&",
	"|",
	"^",
	"~",
	"<<",
	">>",

	"drop",
	"nip",
	"dup",
	"over",
	"tuck",
	"swap",
	"rot",

	"2drop",
	"2nip",
	"2dup",
	"2over",
	"2tuck",
	"2swap",
	"2rot",

	"alloc",
	"resize",
	"free",

	"allot",

	"fetch",
	"store",
	
	"itof",
	"utof",
	"ftoi",
	"ftou",

	"getc",
	"geti",
	"getu",
	"getf",
	"gets",

	"putc",
	"puti",
	"putu",
	"putf",
	"puts",

	"show"
};

const sabr_opcode_t sabr_bio_indices[] = {
	OP_EXIT,
	OP_EXEC,
	OP_DEFINE,
	OP_SET,
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
};

size_t sabr_bio_names_len = sizeof(sabr_bio_names) / sizeof(char*);