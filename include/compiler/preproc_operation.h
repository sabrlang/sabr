#ifndef __PREPROC_OPERATION_H__
#define __PREPROC_OPERATION_H__

#include "compiler.h"

extern const bool (*preproc_keyword_functions[])(sabr_compiler* const comp, word w, token t, vector(token)* output_tokens);

const bool sabr_compiler_preproc_def_base(sabr_compiler* comp, word w, token t, vector(token)* output_tokens, bool is_local, bool is_func);
const bool sabr_compiler_preproc_isdef_base(sabr_compiler* comp, word w, token t, vector(token)* output_tokens, bool is_local);
const bool sabr_compiler_preproc_undef_base(sabr_compiler* comp, word w, token t, vector(token)* output_tokens, bool is_local);
const bool sabr_compiler_preproc_getdef_base(sabr_compiler* comp, word w, token t, vector(token)* output_tokens, bool is_local);

inline const bool sabr_compiler_preproc_func(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
    return sabr_compiler_preproc_def_base(comp, w, t, output_tokens, false, true);
}
inline const bool sabr_compiler_preproc_macro(sabr_compiler* comp, word w, token t, vector(token)* output_tokens){
	return sabr_compiler_preproc_def_base(comp, w, t, output_tokens, false, false);
}
inline const bool sabr_compiler_preproc_isdef(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	return sabr_compiler_preproc_isdef_base(comp, w, t, output_tokens, false);
}
inline const bool sabr_compiler_preproc_undef(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	return sabr_compiler_preproc_undef_base(comp, w, t, output_tokens, false);
}
inline const bool sabr_compiler_preproc_getdef(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	return sabr_compiler_preproc_getdef_base(comp, w, t, output_tokens, false);
}
inline const bool sabr_compiler_preproc_lfunc(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	return sabr_compiler_preproc_def_base(comp, w, t, output_tokens, true, true);
}
inline const bool sabr_compiler_preproc_lmacro(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	return sabr_compiler_preproc_def_base(comp, w, t, output_tokens, true, false);
}
inline const bool sabr_compiler_preproc_lisdef(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	return sabr_compiler_preproc_isdef_base(comp, w, t, output_tokens, true);
}
inline const bool sabr_compiler_preproc_lundef(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	return sabr_compiler_preproc_undef_base(comp, w, t, output_tokens, true);
}
inline const bool sabr_compiler_preproc_lgetdef(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	return sabr_compiler_preproc_getdef_base(comp, w, t, output_tokens, true);
}

const bool sabr_compiler_preproc_import(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_eval(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_if(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_while(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_break(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_continue(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_concat(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_substr(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_trim(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_compare(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_len(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_drop(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_nip(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_dup(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_over(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_tuck(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_swap(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_rot(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_2drop(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_2nip(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_2dup(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_2over(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_2tuck(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_2swap(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_2rot(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_add(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_sub(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_mul(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_div(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_mod(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_udiv(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_umod(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_equ(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_neq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_grt(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_geq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_lst(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_leq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fadd(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fsub(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fmul(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fdiv(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fmod(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fgrt(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fgeq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_flst(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fleq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_and(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_or(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_xor(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_not(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_lsft(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_rsft(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_ftoi(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_itof(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fmti(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fmtu(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_fmtf(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_echo(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_emit(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);
const bool sabr_compiler_preproc_show(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);

#endif