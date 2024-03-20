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
	SABR_OP_EXIT,
	SABR_OP_EXEC,
	SABR_OP_DEFINE,
	SABR_OP_SET,
	SABR_OP_ADDR,
	SABR_OP_REF,

	SABR_OP_ADD,
	SABR_OP_SUB,
	SABR_OP_MUL,
	SABR_OP_DIV,
	SABR_OP_MOD,
	SABR_OP_UDIV,
	SABR_OP_UMOD,
	SABR_OP_NEG,
	SABR_OP_INC,
	SABR_OP_DEC,

	SABR_OP_EQU,
	SABR_OP_NEQ,
	SABR_OP_GRT,
	SABR_OP_GEQ,
	SABR_OP_LST,
	SABR_OP_LEQ,

	SABR_OP_UGRT,
	SABR_OP_UGEQ,
	SABR_OP_ULST,
	SABR_OP_ULEQ,

	SABR_OP_FADD,
	SABR_OP_FSUB,
	SABR_OP_FMUL,
	SABR_OP_FDIV,
	SABR_OP_FMOD,
	SABR_OP_FNEG,

	SABR_OP_FEQU,
	SABR_OP_FNEQ,
	SABR_OP_FGRT,
	SABR_OP_FGEQ,
	SABR_OP_FLST,
	SABR_OP_FLEQ,

	SABR_OP_BAND,
	SABR_OP_BOR,
	SABR_OP_BXOR,
	SABR_OP_BNOT,
	SABR_OP_BLSFT,
	SABR_OP_BRSFT,

	SABR_OP_DROP,
	SABR_OP_NIP,
	SABR_OP_DUP,
	SABR_OP_OVER,
	SABR_OP_TUCK,
	SABR_OP_SWAP,
	SABR_OP_ROT,

	SABR_OP_TDROP,
	SABR_OP_TNIP,
	SABR_OP_TDUP,
	SABR_OP_TOVER,
	SABR_OP_TTUCK,
	SABR_OP_TSWAP,
	SABR_OP_TROT,

	SABR_OP_ALLOC,
	SABR_OP_RESIZE,
	SABR_OP_FREE,

	SABR_OP_ALLOT,

	SABR_OP_FETCH,
	SABR_OP_STORE,

	SABR_OP_ITOF,
	SABR_OP_UTOF,
	SABR_OP_FTOI,
	SABR_OP_FTOU,

	SABR_OP_GETC,
	SABR_OP_GETI,
	SABR_OP_GETU,
	SABR_OP_GETF,
	SABR_OP_GETS,

	SABR_OP_PUTC,
	SABR_OP_PUTI,
	SABR_OP_PUTU,
	SABR_OP_PUTF,
	SABR_OP_PUTS,

	SABR_OP_SHOW
};

size_t sabr_bio_names_len = sizeof(sabr_bio_names) / sizeof(char*);