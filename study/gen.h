#ifndef _GEN_H
#define _GEN_H
#include "common_headers.h"
CPP_START

struct ASTnode;
struct _Writer;
struct _Content;

// Given an AST, the register (if any) that holds
// the previous rvalue, and the AST op of the parent,
// generate assembly code recursively.
// Return the register id with the tree's final value
/**
 * @param n: the ast tree
 * @param w: the writer to write asm
 * @param reg: register index
 * @param parentASTop: parent ast type
 **/
int gen_genAST(struct _Content* cd, struct ASTnode *n, struct _Writer *w, int reg, int parentASTop);

void gen_preamble(struct _Writer *w);
void gen_postamble(struct _Writer *w, int sym_id);
void gen_freeregs(struct _Writer *w);
void gen_printint(struct _Writer *w, int reg);

void gen_globsym(struct _Writer *w, int sym_id);
//get the type size in bytes.
int gen_primsize(struct _Writer *w, int type);

//gen label of string contant
int gen_globstr(struct _Writer *w, const char *strvalue);

CPP_END
#endif