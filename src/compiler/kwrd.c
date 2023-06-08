#include "kwrd.h"

const char* keyword_names[] = {
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

size_t keyword_names_len = sizeof(keyword_names) / sizeof(char*);