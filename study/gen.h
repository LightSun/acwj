#include "common_headers.h"
#ifndef _GEN_H
#define _GEN_H
CPP_START

struct ASTnode;
struct _Writer;

void gen_genCode(struct ASTnode *n, struct _Writer* w);

CPP_END
#endif