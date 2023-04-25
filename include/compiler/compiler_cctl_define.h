#ifndef __COMPILER_CCTL_DEFINE_H__
#define __COMPILER_CCTL_DEFINE_H__

#include <stdbool.h>
#include <stdint.h>

#include "cctl/vector.h"
#include "cctl/trie.h"

#include "token.h"
#include "word.h"


trie_fd(size_t);
trie_imp_h(size_t);

cctl_ptr_def(char);
vector_fd(cctl_ptr(char));
vector_imp_h(cctl_ptr(char));

vector_fd(token);
vector_imp_h(token);

trie_fd(word);
trie_imp_h(word);

cctl_ptr_def(trie(word));
vector_fd(cctl_ptr(trie(word)));
vector_imp_h(cctl_ptr(trie(word)));

vector_fd(size_t);
vector_imp_h(size_t);

#endif