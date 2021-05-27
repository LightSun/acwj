#include "gen.h"
#include "ast.h"
#include "register.h"
#include "sym.h"
#include "writer.h"
//#include "token.h"

// Generic code generator
// Copyright (c) 2019 Warren Toomey, GPL3

// Generate and return a new label number
static int label(struct _Writer *w)
{
  return (w->context->label++);
}

// Generate the code for an IF statement
// and an optional ELSE clause
static int genIFAST(struct ASTnode *n, struct _Writer *w)
{
  int Lfalse, Lend;

  // Generate two labels: one for the
  // false compound statement, and one
  // for the end of the overall IF statement.
  // When there is no ELSE clause, Lfalse _is_
  // the ending label!
  Lfalse = label(w);
  if (n->right)
    Lend = label(w);

  // Generate the condition code followed
  // by a zero jump to the false label.
  // We cheat by sending the Lfalse label as a register.
  gen_genAST(n->left, w, Lfalse, n->op);
  gen_freeregs(w);

  // Generate the true compound statement
  gen_genAST(n->mid, w, NOREG, n->op);
  gen_freeregs(w);

  // If there is an optional ELSE clause,
  // generate the jump to skip to the end
  if (n->right)
    register_cgjump(w, Lend);

  // Now the false label
  register_cglabel(w, Lfalse);

  // Optional ELSE clause: generate the
  // false compound statement and the
  // end label
  if (n->right)
  {
    gen_genAST(n->right, w, NOREG, n->op);
    gen_freeregs(w);
    register_cglabel(w, Lend);
  }

  return (NOREG);
}

// Generate the code for a WHILE statement
// and an optional ELSE clause
static int genWHILE(struct ASTnode *n, struct _Writer *w)
{
  int Lstart, Lend;

  // Generate the start and end labels
  // and output the start label
  Lstart = label(w);
  Lend = label(w);
  register_cglabel(w, Lstart);

  // Generate the condition code followed
  // by a jump to the end label.
  // We cheat by sending the Lfalse label as a register.
  gen_genAST(n->left, w, Lend, n->op);
  gen_freeregs(w);

  // Generate the compound statement for the body
  gen_genAST(n->right, w, NOREG, n->op);
  gen_freeregs(w);

  // Finally output the jump back to the condition,
  // and the end label
  register_cgjump(w, Lstart);
  register_cglabel(w, Lend);
  return (NOREG);
}

//reg: normal reg or label reg
int gen_genAST(struct ASTnode *n, struct _Writer *w, int reg, int parentASTop)
{
  int leftreg, rightreg;

  // We now have specific AST node handling at the top
  switch (n->op)
  {
  case A_IF:
    return (genIFAST(n, w));

  case A_WHILE:
    return (genWHILE(n, w));

  case A_GLUE:
    // Do each child statement, and free the
    // registers after each child
    gen_genAST(n->left, w, NOREG, n->op);
    gen_freeregs(w);
    gen_genAST(n->right, w, NOREG, n->op);
    gen_freeregs(w);
    return (NOREG);

  case A_FUNCTION:
    // Generate the function's preamble before the code
    register_cgfuncpreamble(w, sym_getGlob(n->v.id)->name);
    gen_genAST(n->left, w, NOREG, n->op);
    register_cgfuncpostamble(w);
    return (NOREG);
  }

  // General AST node handling below

  // Get the left and right sub-tree values
  if (n->left)
    leftreg = gen_genAST(n->left, w, NOREG, n->op);
  if (n->right)
    rightreg = gen_genAST(n->right, w, leftreg, n->op);

  switch (n->op)
  {
  case A_ADD:
    return (register_cgadd(w, leftreg, rightreg));
  case A_SUBTRACT:
    return (register_cgsub(w, leftreg, rightreg));
  case A_MULTIPLY:
    return (register_cgmul(w, leftreg, rightreg));
  case A_DIVIDE:
    return (register_cgdiv(w, leftreg, rightreg));
  case A_INTLIT:
    return (register_cgload(w, n->v.intvalue));

  case A_IDENT:
    return (register_cgloadglob(w, sym_getGlob(n->v.id)->name));
  case A_LVIDENT:
    return (register_cgstoreglob(w, reg, sym_getGlob(n->v.id)->name));
  case A_ASSIGN:
    // The work has already been done, return the result
    return (rightreg);

  case A_PRINT:
  {
    // Print the left-child's value
    // and return no register
    gen_printint(w, leftreg);
    gen_freeregs(w);
    return (NOREG);
  }

  case A_EQ:
    // return (register_cgequal(w, leftreg, rightreg));
  case A_NE:
    //return (register_cgnotequal(w, leftreg, rightreg));
  case A_LT:
    // return (register_cglessthan(w, leftreg, rightreg));
  case A_GT:
    // return (register_cggreaterthan(w, leftreg, rightreg));
  case A_LE:
    // return (register_cglessequal(w, leftreg, rightreg));
  case A_GE:
    // return (register_cggreaterequal(w, leftreg, rightreg));
    {
      // If the parent AST node is an A_IF or A_WHILE, generate
      // a compare followed by a jump. Otherwise, compare registers
      // and set one to 1 or 0 based on the comparison.
      if (parentASTop == A_IF || parentASTop == A_WHILE)
        return (register_cgcompare_and_jump(w, n->op, leftreg, rightreg, reg));
      else
        return (register_cgcompare_and_set(w, n->op, leftreg, rightreg));
    }

  default:
    fprintf(stderr, "Unknown AST operator %d\n", n->op);
    exit(1);
  }
}

void gen_preamble(struct _Writer *w)
{
  register_cgpreamble(w);
}
void gen_postamble(struct _Writer *w)
{
  register_cgpostamble(w);
}
void gen_freeregs(struct _Writer *w)
{
  register_free_all();
}
void gen_printint(struct _Writer *w, int reg)
{
  register_cgprintint(w, reg);
}

void gen_globsym(struct _Writer *w, int id, const char* name)
{
  register_cgglobsym(w, id, name);
}
