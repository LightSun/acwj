#include "gen.h"
#include "ast.h"
#include "content.h"
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
static int genIFAST(struct _Content *cd, struct ASTnode *n, struct _Writer *w)
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
  gen_genAST(cd, n->left, w, Lfalse, n->op);
  gen_freeregs(w);

  // Generate the true compound statement
  gen_genAST(cd, n->mid, w, NOREG, n->op);
  gen_freeregs(w);

  // If there is an optional ELSE clause,
  // generate the jump to skip to the end
  if (n->right)
    CONTENT_G_REG(cd)->register_cgjump(WRITER_G_REG_CTX(w), Lend);

  // Now the false label
  CONTENT_G_REG(cd)->register_cglabel(WRITER_G_REG_CTX(w), Lfalse);

  // Optional ELSE clause: generate the
  // false compound statement and the
  // end label
  if (n->right)
  {
    gen_genAST(cd, n->right, w, NOREG, n->op);
    gen_freeregs(w);
    CONTENT_G_REG(cd)->register_cglabel(WRITER_G_REG_CTX(w), Lend);
  }

  return (NOREG);
}

// Generate the code for a WHILE statement
// and an optional ELSE clause
static int genWHILE(struct _Content *cd, struct ASTnode *n, struct _Writer *w)
{
  int Lstart, Lend;

  // Generate the start and end labels
  // and output the start label
  Lstart = label(w);
  Lend = label(w);
  CONTENT_G_REG(cd)->register_cglabel(WRITER_G_REG_CTX(w), Lstart);

  // Generate the condition code followed
  // by a jump to the end label.
  // We cheat by sending the Lfalse label as a register.
  gen_genAST(cd, n->left, w, Lend, n->op);
  gen_freeregs(w);

  // Generate the compound statement for the body
  gen_genAST(cd, n->right, w, NOREG, n->op);
  gen_freeregs(w);

  // Finally output the jump back to the condition,
  // and the end label
  CONTENT_G_REG(cd)->register_cgjump(WRITER_G_REG_CTX(w), Lstart);
  CONTENT_G_REG(cd)->register_cglabel(WRITER_G_REG_CTX(w), Lend);
  return (NOREG);
}

//reg: normal reg or label reg
int gen_genAST(struct _Content *cd, struct ASTnode *n, struct _Writer *w, int reg, int parentASTop)
{
  int leftreg, rightreg;

  // We now have specific AST node handling at the top
  switch (n->op)
  {
  case A_IF:
    return (genIFAST(cd, n, w));

  case A_WHILE:
    return (genWHILE(cd, n, w));

  case A_GLUE:
    // Do each child statement, and free the
    // registers after each child
    gen_genAST(cd, n->left, w, NOREG, n->op);
    gen_freeregs(w);
    gen_genAST(cd, n->right, w, NOREG, n->op);
    gen_freeregs(w);
    return (NOREG);

  case A_FUNCTION:
    // Generate the function's preamble before the code
    CONTENT_G_REG(cd)->register_cgfuncpreamble(WRITER_G_REG_CTX(w), n->v.id);
    gen_genAST(cd, n->left, w, NOREG, n->op);
    CONTENT_G_REG(cd)->register_cgfuncpostamble(WRITER_G_REG_CTX(w), n->v.id);
    return (NOREG);
  }

  // General AST node handling below

  // Get the left and right sub-tree values
  if (n->left)
    leftreg = gen_genAST(cd, n->left, w, NOREG, n->op);
  if (n->right)
    rightreg = gen_genAST(cd, n->right, w, leftreg, n->op);

  switch (n->op)
  {
  case A_ADD:
    return (CONTENT_G_REG(cd)->register_cgadd(WRITER_G_REG_CTX(w), leftreg, rightreg));
  case A_SUBTRACT:
    return (CONTENT_G_REG(cd)->register_cgsub(WRITER_G_REG_CTX(w), leftreg, rightreg));
  case A_MULTIPLY:
    return (CONTENT_G_REG(cd)->register_cgmul(WRITER_G_REG_CTX(w), leftreg, rightreg));
  case A_DIVIDE:
    return (CONTENT_G_REG(cd)->register_cgdiv(WRITER_G_REG_CTX(w), leftreg, rightreg));
  case A_INTLIT:
    return (CONTENT_G_REG(cd)->register_cgloadint(WRITER_G_REG_CTX(w), n->v.intvalue));

  case A_IDENT:
  {
    return (CONTENT_G_REG(cd)->register_cgloadglob(WRITER_G_REG_CTX(w), n->v.id));
  }

  case A_LVIDENT:
  {
    return (CONTENT_G_REG(cd)->register_cgstoreglob(WRITER_G_REG_CTX(w), reg, n->v.id));
  }

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
        return (CONTENT_G_REG(cd)->register_cgcompare_and_jump(WRITER_G_REG_CTX(w), n->op, leftreg, rightreg, reg));
      else
        return (CONTENT_G_REG(cd)->register_cgcompare_and_set(WRITER_G_REG_CTX(w), n->op, leftreg, rightreg));
    }

  case A_WIDEN:
    // Widen the child's type to the parent's type
    return (CONTENT_G_REG(cd)->register_cgwiden(WRITER_G_REG_CTX(w), leftreg, n->left->type, n->type));

  case A_RETURN:
  {
    CONTENT_G_REG(cd)->register_cgreturn(WRITER_G_REG_CTX(w), leftreg, cd->context->functionid);
    return (NOREG);
  }

  case A_FUNCCALL:
    return (CONTENT_G_REG(cd)->register_cgcall(WRITER_G_REG_CTX(w), leftreg, n->v.id));

  case A_ADDR:
    return (CONTENT_G_REG(cd)->register_cgaddress(WRITER_G_REG_CTX(w), n->v.id));

  case A_DEREF:
    return (CONTENT_G_REG(cd)->register_cgderef(WRITER_G_REG_CTX(w), leftreg, n->left->type));

  default:
    fprintf(stderr, "Unknown AST operator %d\n", n->op);
    exit(1);
  }
  return (NOREG);
}

void gen_preamble(struct _Writer *w)
{
  WRITER_G_REG(w)->register_cgpreamble(WRITER_G_REG_CTX(w));
}
void gen_postamble(struct _Writer *w, int id)
{
  WRITER_G_REG(w)->register_cgpostamble(WRITER_G_REG_CTX(w), id);
}
void gen_freeregs(struct _Writer *w)
{
  WRITER_G_REG(w)->register_free_all(WRITER_G_REG_CTX(w));
}
void gen_printint(struct _Writer *w, int reg)
{
  WRITER_G_REG(w)->register_cgprintint(WRITER_G_REG_CTX(w), reg);
}

void gen_globsym(struct _Writer *w, int sym_id)
{
  WRITER_G_REG(w)->register_cgglobsym(WRITER_G_REG_CTX(w), sym_id);
}

int gen_primsize(struct _Writer *w, int type)
{
  return (WRITER_G_REG(w)->register_cgprimsize(WRITER_G_REG_CTX(w), type));
}