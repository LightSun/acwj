#include "common_headers.h"
#ifndef _GEN_H
#define _GEN_H
CPP_START

struct ASTnode;
struct _Writer;

int gen_genAST(struct ASTnode *n, struct _Writer *w);

void gen_genCode(struct ASTnode *n, struct _Writer* w);

void gen_preamble(struct _Writer *w);
void gen_postamble(struct _Writer *w);
void gen_freeregs(struct _Writer *w);
void gen_printint(struct _Writer *w, int reg);

CPP_END
#endif