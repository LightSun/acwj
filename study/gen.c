#include "ast.h"
#include "register.h"
#include "writer.h"
#include "sym.h"
//#include "token.h"

// Generic code generator
// Copyright (c) 2019 Warren Toomey, GPL3

// Given an AST, generate assembly code recursively.
// Return the register id with the tree's final value
int gen_genAST(struct ASTnode *n, struct _Writer* w, int reg) {
  int leftreg, rightreg;

  // Get the left and right sub-tree values
  if (n->left)
    leftreg = gen_genAST(n->left, w, -1);
  if (n->right)
    rightreg = gen_genAST(n->right, w, leftreg);

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
      return (register_cgload(w, n->v.intvalue));

    case A_IDENT:
     return (register_cgloadglob(w, sym_getGlob(n->v.id)->name));
    case A_LVIDENT:
     return (register_cgstoreglob(w, reg, sym_getGlob(n->v.id)->name));
    case A_ASSIGN:
    // The work has already been done, return the result
     return (rightreg);
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

void gen_globsym(struct _Writer *w, const char *s) {
  register_cgglobsym(w, s);
}
