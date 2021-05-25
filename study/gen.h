#ifndef _GEN_H
#define _GEN_H
#include "common_headers.h"
CPP_START

struct ASTnode;
struct _Writer;

int gen_genAST(struct ASTnode *n, struct _Writer *w, int reg);

void gen_preamble(struct _Writer *w);
void gen_postamble(struct _Writer *w);
void gen_freeregs(struct _Writer *w);
void gen_printint(struct _Writer *w, int reg);

void gen_globsym(struct _Writer *w, const char *sym);

CPP_END
#endif