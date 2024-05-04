#include "bif_math.h"

const uint32_t sabr_bif_func(math, sin)(sabr_interpreter_t* inter) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.f = sin(a.f);
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(math, cos)(sabr_interpreter_t* inter) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.f = cos(a.f);
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(math, tan)(sabr_interpreter_t* inter) {
	sabr_value_t a;
	if (!sabr_interpreter_pop(inter, &a)) return SABR_OPERR_STACK;
	a.f = tan(a.f);
	if (!sabr_interpreter_push(inter, a)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t (*sabr_bif_math_functions[])(sabr_interpreter_t*) = {
	sabr_bif_func(math, sin),
	sabr_bif_func(math, cos),
	sabr_bif_func(math, tan)
};
size_t sabr_bif_math_functions_len = sizeof(sabr_bif_math_functions) / sizeof(void*);