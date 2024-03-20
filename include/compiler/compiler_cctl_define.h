#ifndef __COMPILER_CCTL_DEFINE_H__
#define __COMPILER_CCTL_DEFINE_H__

#include <stdbool.h>
#include <stdint.h>

#include "cctl/vector.h"
#include "cctl/trie.h"

#include "preproc.h"
#include "token.h"
#include "value.h"
#include "word.h"

trie_fd(size_t);
trie_imp_h(size_t);

cctl_ptr_def(char);
vector_fd(cctl_ptr(char));
vector_imp_h(cctl_ptr(char));

vector_fd(sabr_token_t);
vector_imp_h(sabr_token_t);

trie_fd(sabr_word_t);
trie_imp_h(sabr_word_t);

cctl_ptr_def(trie(sabr_word_t));
vector_fd(cctl_ptr(trie(sabr_word_t)));
vector_imp_h(cctl_ptr(trie(sabr_word_t)));

vector_fd(size_t);
vector_imp_h(size_t);

vector_fd(sabr_preproc_stop_flag_t);
vector_imp_h(sabr_preproc_stop_flag_t);

vector_fd(sabr_keyword_data_t);
vector_imp_h(sabr_keyword_data_t);

cctl_ptr_def(vector(sabr_keyword_data_t));
vector_fd(cctl_ptr(vector(sabr_keyword_data_t)));
vector_imp_h(cctl_ptr(vector(sabr_keyword_data_t)));

#endif