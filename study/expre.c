
#include "expre.h"
#include "ast.h"
#include "content.h"
#include "misc.h"
#include "scanner.h"
#include "sym.h"
#include "token.h"
#include "types.h"
#include "utils.h"
#include "writer.h"
#include "gen.h"

//grammer
/// compound_statement:          // empty, i.e. no statement
//      |      statement
//      |      statement statements
//      ;
//
// statement: print_statement
//      |     declaration
//      |     assignment_statement
//      |     function_call
//      |     if_statement
//      |     while_statement
//      |     for_statement
//      |     return_statement
//      ;

// AST tree functions
// Copyright (c) 2019 Warren Toomey, GPL3

// Build and return a generic AST node
struct ASTnode *expre_mkastnode(int op, int type, struct ASTnode *left, struct ASTnode *mid,
                                struct ASTnode *right, int intvalue)
{
  struct ASTnode *n;

  // Malloc a new ASTnode
  n = (struct ASTnode *)malloc(sizeof(struct ASTnode));
  if (n == NULL)
  {
    fprintf(stderr, "Unable to malloc in mkastnode()\n");
    exit(1);
  }
  // Copy in the field values and return it
  n->rvalue = 0;
  n->op = op;
  n->type = type;
  n->left = left;
  n->right = right;
  n->mid = mid;
  n->v.intvalue = intvalue;
  return (n);
}

// Make an AST leaf node
struct ASTnode *expre_mkastleaf(int op, int type, int intvalue)
{
  return (expre_mkastnode(op, type, NULL, NULL, NULL, intvalue));
}

// Make a unary AST node: only one child
//op: ast op
struct ASTnode *expre_mkastunary(int op, int type, struct ASTnode *left, int intvalue)
{
  return (expre_mkastnode(op, type, left, NULL, NULL, intvalue));
}

// Parse a function call with a single expression
// argument and return its AST
struct ASTnode *expre_funccall(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *tree;
  int id;

  // Check that the identifier has been defined,
  // then make a leaf node for it. XXX Add structural type test
  if ((id = sym_findSymbol(cd->context->globalState, cd->context->textBuf)) == -1)
  {
    fatals(cd, "Undeclared function", cd->context->textBuf);
  }
  // Get the '('
  misc_lparen(cd, token);

  // Parse the following expression
  tree = expre_binexpr(cd, w, token, 0);

  // Build the function call AST node. Store the
  // function's return type as this node's type.
  // Also record the function's symbol-id
  tree = expre_mkastunary(A_FUNCCALL, sym_getSymbol(cd->context->globalState, id)->type, tree, id);

  // Get the ')'
  misc_rparen(cd, token);
  return (tree);
}

// Parse the index into an array and
// return an AST tree for it
static struct ASTnode *expre_array_access(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *left, *right;
  int id;

  // Check that the identifier has been defined as an array
  // then make a leaf node for it that points at the base
  if ((id = sym_findSymbol(cd->context->globalState, cd->context->textBuf)) == -1)
  {
    CONTENT_PUBLISH_ERROR(cd, "Undeclared array '%s'", cd->context->textBuf);
  }
  SymTable *st = sym_getSymbol(cd->context->globalState, id);
  if (st->stype != S_ARRAY)
  {
    CONTENT_PUBLISH_ERROR(cd, "Undeclared array '%s'", cd->context->textBuf);
  }

  left = expre_mkastleaf(A_ADDR, st->type, id);

  // Get the '['
  scanner_scan(cd, token);

  // Parse the following expression
  right = expre_binexpr(cd, w, token, 0);

  // Get the ']'
  misc_match(cd, token, T_RBRACKET, "]");

  // Ensure that this is of int type
  if (!types_inttype(right->type))
    CONTENT_PUBLISH_ERROR(cd, "Array index is not of integer type = %d", right->type);

  // Scale the index by the size of the element's type
  right = types_modify_type(right, w, left->type, A_ADD);

  // Return an AST tree where the array's base has the offset
  // added to it, and dereference the element. Still an lvalue
  // at this point.
  left = expre_mkastnode(A_ADD, st->type, left, NULL, right, 0);
  left = expre_mkastunary(A_DEREF, types_value_at(left->type), left, 0);
  return (left);
}

// Parse a postfix expression and return
// an AST node representing it. The
// identifier is already in Text.
static struct ASTnode *expre_postfix(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *n;
  int id;

  // Scan in the next token to see if we have a postfix expression
  scanner_scan(cd, token);

  // Function call/. ()
  if (token->token == T_LPAREN)
    return (expre_funccall(cd, w, token));

  // An array reference. // [
  if (token->token == T_LBRACKET)
    return (expre_array_access(cd, w, token));

  // A variable. Check that the variable exists.
  id = sym_findSymbol(cd->context->globalState, cd->context->textBuf);
  if (id == -1 || SYM_STYPE(cd->context->globalState, id) != S_VARIABLE)
    WRITER_PUBLISH_ERROR(w, "Unknown variable = %s", cd->context->textBuf);

  switch (token->token)
  {
    // Post-increment: skip over the token
  case T_INC:
    scanner_scan(cd, token);
    n = expre_mkastleaf(A_POSTINC, SYM_TYPE(cd->context->globalState, id), id);
    break;

    // Post-decrement: skip over the token
  case T_DEC:
    scanner_scan(cd, token);
    n = expre_mkastleaf(A_POSTDEC, SYM_TYPE(cd->context->globalState, id), id);
    break;

    // Just a variable reference
  default:
    n = expre_mkastleaf(A_IDENT, SYM_TYPE(cd->context->globalState, id), id);
  }
  return (n);
}

/**
 primary_expression
        : IDENTIFIER
        | CONSTANT
        | STRING_LITERAL
        | '(' expression ')'
        ;

postfix_expression
        : primary_expression
        | postfix_expression '[' expression ']'
        | postfix_expression '(' expression ')'
        | postfix_expression '++'
        | postfix_expression '--'
        ;

prefix_expression
        : postfix_expression
        | '++' prefix_expression
        | '--' prefix_expression
        | prefix_operator prefix_expression
        ;

prefix_operator
        : '&'
        | '*'
        | '-'
        | '~'
        | '!'
        ;

multiplicative_expression
        : prefix_expression
        | multiplicative_expression '*' prefix_expression
        | multiplicative_expression '/' prefix_expression
        | multiplicative_expression '%' prefix_expression
        ;

        etc.
 * */

// Parse a primary factor and return an
// AST node representing it.
static struct ASTnode *primary(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *n;
  int id;

  // For an INTLIT token, make a leaf AST node for it
  // and scan in the next token. Otherwise, a syntax error
  // for any other token type.
  switch (token->token)
  {
  case T_STRLIT:
    id = gen_globstr(w, cd->context->textBuf);
    n = expre_mkastleaf(A_STRLIT, P_CHARPTR, id);
    break;

  case T_INTLIT:
    // For an INTLIT token, make a leaf AST node for it.
    // Make it a P_CHAR if it's within the P_CHAR range
    if ((token->intvalue) >= 0 && (token->intvalue < 256))
    {
      n = expre_mkastleaf(A_INTLIT, P_CHAR, token->intvalue);
    }
    else
    {
      n = expre_mkastleaf(A_INTLIT, P_INT, token->intvalue);
    }
    break;

  case T_IDENT:
    return expre_postfix(cd, w, token);

  case T_LPAREN:
    // Beginning of a parenthesised expression, skip the '('.
    // Scan in the expression and the right parenthesis
    scanner_scan(cd, token);
    n = expre_binexpr(cd, w, token, 0);
    misc_rparen(cd, token);
    return (n);

  default:
    CONTENT_PUBLISH_ERROR(cd, "called primary() >>> syntax error, token = %d", token->token);
  }
  scanner_scan(cd, token);
  return n;
}

// Convert a binary operator token into an AST operation.
// We rely on a 1:1 mapping from token to AST operation
static int arithop(struct _Content *cd, int tokentype)
{
  if (tokentype > T_EOF && tokentype < T_INTLIT)
  {
    return (tokentype);
  }
  CONTENT_PUBLISH_ERROR(cd, "unknown token in arithop()");
  return 0;
}

// Convert a binary operator token into a binary AST operation.
// We rely on a 1:1 mapping from token to AST operation
static int binastop(struct _Content *cd, int tokentype)
{
  if (tokentype > T_EOF && tokentype <= T_SLASH)
    return (tokentype);
  CONTENT_PUBLISH_ERROR(cd, "binastop: Syntax error, token = %d", tokentype);
  return (0); // Keep -Wall happy
}

// Return true if a token is right-associative,
// false otherwise.
static int rightassoc(int tokentype)
{
  if (tokentype == T_ASSIGN)
    return (1);
  return (0);
}

// Operator precedence for each token. Must
// match up with the order of tokens in defs.h
static int OpPrec[] = {
    0, 10, 20, 30,  // T_EOF, T_ASSIGN, T_LOGOR, T_LOGAND
    40, 50, 60,     // T_OR, T_XOR, T_AMPER
    70, 70,         // T_EQ, T_NE
    80, 80, 80, 80, // T_LT, T_GT, T_LE, T_GE
    90, 90,         // T_LSHIFT, T_RSHIFT
    100, 100,       // T_PLUS, T_MINUS
    110, 110        // T_STAR, T_SLASH
};

// Check that we have a binary operator and
// return its precedence.
static int op_precedence(struct _Content *cd, int tokentype)
{
  if (tokentype >= T_VOID)
  {
    CONTENT_PUBLISH_ERROR(cd, "Token with no precedence in op_precedence: token = %d", tokentype);
  }
  int prec = OpPrec[tokentype];
  if (prec == 0)
  {
    CONTENT_PUBLISH_ERROR(cd, "op_precedence: Syntax error, token = %d", tokentype);
  }
  return (prec);
}
// Return an AST tree whose root is a binary operator.
// ptp:  the previous token's precedence.
struct ASTnode *expre_binexpr(struct _Content *cd, struct _Writer *w, struct Token *token, int ptp)
{
  struct ASTnode *left, *right;
  struct ASTnode *ltemp, *rtemp;
  int aSTop;
  int tokentype;

  // Get the integer literal on the left.
  // Fetch the next token at the same time.
  left = expre_prefix(cd, w, token);

  // If we hit a semicolon(;) or ')' or ']', return just the left node
  tokentype = token->token;
  if (tokentype == T_SEMI || tokentype == T_RPAREN || tokentype == T_RBRACKET)
  {
    left->rvalue = 1;
    return (left);
  }

  // While the precedence of this token is more than that of the
  // previous token precedence, or it's right associative and
  // equal to the previous token's precedence
  while (op_precedence(cd, tokentype) > ptp ||
         (rightassoc(tokentype) && op_precedence(cd, tokentype) == ptp))
  {
    // Fetch in the next integer literal
    scanner_scan(cd, token);

    // Recursively call binexpr() with the
    // precedence of our token to build a sub-tree
    right = expre_binexpr(cd, w, token, OpPrec[tokentype]);

    // Determine the operation to be performed on the sub-trees
    aSTop = binastop(cd, tokentype);

    if (aSTop == A_ASSIGN)
    {
      // Assignment
      // Make the right tree into an rvalue
      right->rvalue = 1;

      // Ensure the right's type matches the left
      right = types_modify_type(right, w, left->type, 0);
      if (left == NULL)
        WRITER_PUBLISH_ERROR(w, "Incompatible expression in assignment");

      // Make an assignment AST tree. However, switch
      // left and right around, so that the right expression's
      // code will be generated before the left expression
      ltemp = left;
      left = right;
      right = ltemp;
    }
    else
    {

      // We are not doing an assignment, so both trees should be rvalues
      // Convert both trees into rvalue if they are lvalue trees
      left->rvalue = 1;
      right->rvalue = 1;

      // Ensure the two types are compatible by trying
      // to modify each tree to match the other's type.
      ltemp = types_modify_type(left, w, right->type, aSTop);
      rtemp = types_modify_type(right, w, left->type, aSTop);
      if (ltemp == NULL && rtemp == NULL)
        WRITER_PUBLISH_ERROR(w, "Incompatible types in binary expression");
      if (ltemp != NULL)
        left = ltemp;
      if (rtemp != NULL)
        right = rtemp;
    }

    // Join that sub-tree with ours. Convert the token
    // into an AST operation at the same time.
    left = expre_mkastnode(binastop(cd, tokentype), left->type, left, NULL, right, 0);

    // Update the details of the current token.
    // If we hit a semicolon or ')', return just the left node
    tokentype = token->token;
    if (tokentype == T_SEMI || tokentype == T_RPAREN || tokentype == T_RBRACKET)
    {
      left->rvalue = 1;
      return (left);
    }
  }
  // Return the tree we have when the precedence
  // is the same or lower
  left->rvalue = 1;
  return (left);
}

// prefix_expression: primary
//     | '*'  prefix_expression
//     | '&'  prefix_expression
//     | '-'  prefix_expression
//     | '++' prefix_expression
//     | '--' prefix_expression
//     ;
// Parse a prefix expression and return
// a sub-tree representing it.
struct ASTnode *expre_prefix(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *tree;
  switch (token->token)
  {
  case T_AMPER: //&
    // Get the next token and parse it
    // recursively as a prefix expression
    scanner_scan(cd, token);
    tree = expre_prefix(cd, w, token);

    // Ensure that it's an identifier
    if (tree->op != A_IDENT)
    {
      WRITER_PUBLISH_ERROR(w, "& operator must be followed by an identifier");
    }

    // Now change the operator to A_ADDR and the type to
    // a pointer to the original type
    tree->op = A_ADDR;
    tree->type = types_pointer_to(tree->type);
    break;

  case T_STAR: //*
    // Get the next token and parse it
    // recursively as a prefix expression
    scanner_scan(cd, token);
    tree = expre_prefix(cd, w, token);

    // For now, ensure it's either another deref or an
    // identifier
    if (tree->op != A_IDENT && tree->op != A_DEREF)
      WRITER_PUBLISH_ERROR(w, "* operator must be followed by an identifier or *");

    // Prepend an A_DEREF operation to the tree
    tree = expre_mkastunary(A_DEREF, types_value_at(tree->type), tree, 0);
    break;

  case T_MINUS: //- 
    // Get the next token and parse it
    // recursively as a prefix expression
    scanner_scan(cd, token);
    tree = expre_prefix(cd, w, token);

    // Prepend a A_NEGATE operation to the tree and
    // make the child an rvalue. Because chars are unsigned,
    // also widen this to int so that it's signed
    tree->rvalue = 1;
    tree = types_modify_type(tree, w, P_INT, 0);
    tree = expre_mkastunary(A_NEGATE, tree->type, tree, 0);
    break;

  case T_INVERT: // ~
    // Get the next token and parse it
    // recursively as a prefix expression
    scanner_scan(cd, token);
    tree = expre_prefix(cd, w, token);

    // Prepend a A_INVERT operation to the tree and
    // make the child an rvalue.
    tree->rvalue = 1;
    tree = expre_mkastunary(A_INVERT, tree->type, tree, 0);
    break;

  case T_LOGNOT: // !
    // Get the next token and parse it
    // recursively as a prefix expression
    scanner_scan(cd, token);
    tree = expre_prefix(cd, w, token);

    // Prepend a A_LOGNOT operation to the tree and
    // make the child an rvalue.
    tree->rvalue = 1;
    tree = expre_mkastunary(A_LOGNOT, tree->type, tree, 0);
    break;

  case T_INC: //++
    // Get the next token and parse it
    // recursively as a prefix expression
    scanner_scan(cd, token);
    tree = expre_prefix(cd, w, token);

    // For now, ensure it's an identifier
    if (tree->op != A_IDENT)
      WRITER_PUBLISH_ERROR(w, "++ operator must be followed by an identifier");

    // Prepend an A_PREINC operation to the tree
    tree = expre_mkastunary(A_PREINC, tree->type, tree, 0);
    break;

  case T_DEC: // --
    // Get the next token and parse it
    // recursively as a prefix expression
    scanner_scan(cd, token);
    tree = expre_prefix(cd, w, token);

    // For now, ensure it's an identifier
    if (tree->op != A_IDENT)
      WRITER_PUBLISH_ERROR(w, "-- operator must be followed by an identifier");

    // Prepend an A_PREDEC operation to the tree
    tree = expre_mkastunary(A_PREDEC, tree->type, tree, 0);
    break;

  default:
    tree = primary(cd, w, token);
  }
  return (tree);
}

//----------------------------- dump ------------------------------------------

// Generate and return a new label number
// just for AST dumping purposes
static int gendumplabel(struct _Content *cd)
{
  return (cd->context->dumpId++);
}

// Given an AST tree, print it out and follow the
// traversal of the tree that genAST() follows
void expre_dumpAST(struct _Content *cd, struct ASTnode *n, int label, int level)
{
  int Lfalse, Lstart, Lend;

  switch (n->op)
  {
  case A_IF:
    Lfalse = gendumplabel(cd);
    for (int i = 0; i < level; i++)
      fprintf(stdout, " ");
    fprintf(stdout, "A_IF");
    if (n->right)
    {
      Lend = gendumplabel(cd);
      fprintf(stdout, ", end L%d", Lend);
    }
    fprintf(stdout, "\n");
    expre_dumpAST(cd, n->left, Lfalse, level + 2);
    expre_dumpAST(cd, n->mid, NOLABEL, level + 2);
    if (n->right)
      expre_dumpAST(cd, n->right, NOLABEL, level + 2);
    return;

  case A_WHILE:
    Lstart = gendumplabel(cd);
    for (int i = 0; i < level; i++)
      fprintf(stdout, " ");
    fprintf(stdout, "A_WHILE, start L%d\n", Lstart);

    Lend = gendumplabel(cd);
    expre_dumpAST(cd, n->left, Lend, level + 2);
    expre_dumpAST(cd, n->right, NOLABEL, level + 2);
    return;
  }

  // Reset level to -2 for A_GLUE
  if (n->op == A_GLUE)
    level = -2;

  // General AST node handling
  if (n->left)
    expre_dumpAST(cd, n->left, NOLABEL, level + 2);
  if (n->right)
    expre_dumpAST(cd, n->right, NOLABEL, level + 2);

  for (int i = 0; i < level; i++)
  {
    fprintf(stdout, " ");
  }

  switch (n->op)
  {
  case A_GLUE:
    fprintf(stdout, "\n\n");
    return;

  case A_FUNCTION:
    fprintf(stdout, "A_FUNCTION %s\n", SYM_NAME(cd->context->globalState, n->v.id));
    return;

  case A_ADD:
    fprintf(stdout, "A_ADD\n");
    return;
  case A_SUBTRACT:
    fprintf(stdout, "A_SUBTRACT\n");
    return;
  case A_MULTIPLY:
    fprintf(stdout, "A_MULTIPLY\n");
    return;

  case A_DIVIDE:
    fprintf(stdout, "A_DIVIDE\n");
    return;

  case A_EQ:
    fprintf(stdout, "A_EQ\n");
    return;
  case A_NE:
    fprintf(stdout, "A_NE\n");
    return;
  case A_LT:
    fprintf(stdout, "A_LE\n");
    return;
  case A_GT:
    fprintf(stdout, "A_GT\n");
    return;
  case A_LE:
    fprintf(stdout, "A_LE\n");
    return;
  case A_GE:
    fprintf(stdout, "A_GE\n");
    return;

  case A_INTLIT:
    fprintf(stdout, "A_INTLIT %d\n", n->v.intvalue);
    return;
  case A_STRLIT:
    fprintf(stdout, "A_STRLIT rval label L%d\n", n->v.id);
    return;

  case A_IDENT:
    if (n->rvalue)
      fprintf(stdout, "A_IDENT rval %s\n", SYM_NAME(cd->context->globalState, n->v.id));
    else
      fprintf(stdout, "A_IDENT %s\n", SYM_NAME(cd->context->globalState, n->v.id));
    return;

  case A_ASSIGN:
    fprintf(stdout, "A_ASSIGN\n");
    return;
  case A_WIDEN:
    fprintf(stdout, "A_WIDEN\n");
    return;
  case A_RETURN:
    fprintf(stdout, "A_RETURN\n");
    return;
  case A_FUNCCALL:
    fprintf(stdout, "A_FUNCCALL %s\n", SYM_NAME(cd->context->globalState, n->v.id));
    return;

  case A_ADDR:
    fprintf(stdout, "A_ADDR %s\n", SYM_NAME(cd->context->globalState, n->v.id));
    return;
  case A_DEREF:
    if (n->rvalue)
      fprintf(stdout, "A_DEREF rval\n");
    else
      fprintf(stdout, "A_DEREF\n");
    return;
    
  case A_SCALE:
    fprintf(stdout, "A_SCALE %d\n", n->v.size);
    return;

  case A_PREINC:
    fprintf(stdout, "A_PREINC %s\n", SYM_NAME(cd->context->globalState, n->v.id));
    return;
  case A_PREDEC:
    fprintf(stdout, "A_PREDEC %s\n", SYM_NAME(cd->context->globalState, n->v.id));
    return;
  case A_POSTINC:
    fprintf(stdout, "A_POSTINC\n");
    return;
  case A_POSTDEC:
    fprintf(stdout, "A_POSTDEC\n");
    return;
  case A_NEGATE:
    fprintf(stdout, "A_NEGATE\n");
    return;

  default:
    CONTENT_PUBLISH_ERROR(cd, "Unknown expre_dumpAST operator = %d", n->op);
  }
}