#include "ast.h"
#include "register.h"
#include "writer.h"
//#include "token.h"

// Generic code generator
// Copyright (c) 2019 Warren Toomey, GPL3

// Given an AST, generate
// assembly code recursively
int gen_genAST(struct ASTnode *n, struct _Writer* w) {
  int leftreg, rightreg;

  // Get the left and right sub-tree values
  if (n->left)
    leftreg = gen_genAST(n->left, w);
  if (n->right)
    rightreg = gen_genAST(n->right, w);

  switch (n->op) {
    case A_ADD:
      return (register_cgadd(w, leftreg,rightreg));
    case A_SUBTRACT:
      return (register_cgsub(w, leftreg,rightreg));
    case A_MULTIPLY:
      return (register_cgmul(w, leftreg,rightreg));
    case A_DIVIDE:
      return (register_cgdiv(w, leftreg,rightreg));
    case A_INTLIT:
      return (register_cgload(w, n->intvalue));
    default:
      fprintf(stderr, "Unknown AST operator %d\n", n->op);
      exit(1);
  }
}

void gen_preamble(struct _Writer *w){
  register_cgpreamble(w);
}
void gen_postamble(struct _Writer *w){
  register_cgpostamble(w);
}
void gen_freeregs(struct _Writer *w){
  register_free_all();
}
void gen_printint(struct _Writer *w, int reg){
  register_cgprintint(w, reg);
}


void gen_genCode(struct ASTnode *n, struct _Writer* w) {
    if(!w->start(w->context, 64)){
        printf("gen_genCode failed for open writer.");
        return;
    }

    int reg;

    register_cgpreamble(w);
    reg = gen_genAST(n, w);
    register_cgprintint(w, reg);
    register_cgpostamble(w);

    w->end(w->context);
}
