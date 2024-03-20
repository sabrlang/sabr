#include "kwrd.h"

const char* sabr_keyword_names[] = {
	"end",
	"if",
	"else",
	"loop",
	"while",
	"break",
	"continue",
	"for",
	"ufor",
	"ffor",
	"from",
	"to",
	"step",
	"switch",
	"case",
	"pass",
	"func",
	"macro",
	"defer",
	"return",
	"struct",
	"member",
	"[",
	",",
	"]"
};

size_t sabr_keyword_names_len = sizeof(sabr_keyword_names) / sizeof(char*);