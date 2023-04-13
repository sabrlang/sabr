#ifndef __COMPILER_CCTL_DEFINE_H__
#define __COMPILER_CCTL_DEFINE_H__

#include <stdbool.h>
#include <stdint.h>

#include "cctl/vector.h"
#include "cctl/trie.h"

#include "token.h"

cctl_ptr_def(char);

trie_fd(size_t);
trie_imp_h(size_t);

vector_fd(cctl_ptr(char));
vector_imp_h(cctl_ptr(char));

vector_fd(token);
vector_imp_h(token);

#endif