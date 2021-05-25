
#include "ast.h"
#include "content.h"
#include "token.h"
#include "scanner.h"
#include "sym.h"

// AST tree functions
// Copyright (c) 2019 Warren Toomey, GPL3

// Build and return a generic AST node
struct ASTnode *expre_mkastnode(int op, struct ASTnode *left,
			  struct ASTnode *right, int intvalue) {
  struct ASTnode *n;

  // Malloc a new ASTnode
  n = (struct ASTnode *) malloc(sizeof(struct ASTnode));
  if (n == NULL) {
    fprintf(stderr, "Unable to malloc in mkastnode()\n");
    exit(1);
  }
  // Copy in the field values and return it
  n->op = op;
  n->left = left;
  n->right = right;
  n->v.intvalue = intvalue;
  return (n);
}


// Make an AST leaf node
struct ASTnode *expre_mkastleaf(int op, int intvalue) {
  return (expre_mkastnode(op, NULL, NULL, intvalue));
}

// Make a unary AST node: only one child
struct ASTnode *expre_mkastunary(int op, struct ASTnode *left, int intvalue) {
  return (expre_mkastnode(op, left, NULL, intvalue));
}

// Parsing of expressions
// Copyright (c) 2019 Warren Toomey, GPL3

// Parse a primary factor and return an
// AST node representing it.
static struct ASTnode *primary(struct _Content* cd, struct Token* token) {
  struct ASTnode *n;
  int id;

  // For an INTLIT token, make a leaf AST node for it
  // and scan in the next token. Otherwise, a syntax error
  // for any other token type.
  switch (token->token) {
    case T_INTLIT:
      // For an INTLIT token, make a leaf AST node for it.
      n = expre_mkastleaf(A_INTLIT, token->intvalue);
      break;

    case T_IDENT:
       // Check that this identifier exists
      id = sym_findglob(cd->context.textBuf);
      if (id == -1){
        fprintf(stderr, "Unknown variable %s.", cd->context.textBuf);
        exit(1);
      }

      // Make a leaf AST node for it
      n = expre_mkastleaf(A_IDENT, id);
      break;  

    default:
      fprintf(stderr, "syntax error on line %d\n", cd->context.line);
      exit(1);
  }
  scanner_scan(cd, token);
  return n;
}


// Convert a binary operator token into an AST operation.
// We rely on a 1:1 mapping from token to AST operation
static int arithop(struct _Content* cd, int tokentype) {
  if (tokentype > T_EOF && tokentype < T_INTLIT){
    return(tokentype);
  }
  fprintf(stderr, "unknown token in arithop() on line %d\n", cd->context.line);
  exit(1);
}

// Operator precedence for each token. Must
// match up with the order of tokens in defs.h
static int OpPrec[] = {
  0, 10, 10,                    // T_EOF, T_PLUS, T_MINUS
  20, 20,                       // T_STAR, T_SLASH
  30, 30,                       // T_EQ, T_NE
  40, 40, 40, 40                // T_LT, T_GT, T_LE, T_GE
};

// Check that we have a binary operator and
// return its precedence.
static int op_precedence(struct _Content* cd, int tokentype) {
  int prec = OpPrec[tokentype];
  if (prec == 0) {
    fprintf(stderr, "syntax error on line %d, token %d\n", cd->context.line, tokentype);
    exit(1);
  }
  return (prec);
}
// Return an AST tree whose root is a binary operator.
// ptp:  the previous token's precedence.
struct ASTnode* expre_binexpr(struct _Content* cd, struct Token* token, int ptp) {
  struct ASTnode *left, *right;
  int tokentype;

  // Get the integer literal on the left.
  // Fetch the next token at the same time.
  left = primary(cd, token);

  // If we hit a semicolon, return just the left node
  tokentype = token->token;
  if (tokentype == T_SEMI)
    return (left);

  // While the precedence of this token is
  // more than that of the previous token precedence
  while (op_precedence(cd, tokentype) > ptp) {
    // Fetch in the next integer literal
    scanner_scan(cd, token);

    // Recursively call binexpr() with the
    // precedence of our token to build a sub-tree
    right = expre_binexpr(cd, token, OpPrec[tokentype]);

    // Join that sub-tree with ours. Convert the token
    // into an AST operation at the same time.
    left = expre_mkastnode(arithop(cd, tokentype), left, right, 0);

     // Update the details of the current token.
    // If we hit a semicolon, return just the left node
    tokentype = token->token;
    if (tokentype == T_SEMI)
      return (left);
  }

  // Return the tree we have when the precedence
  // is the same or lower
  return (left);
}

struct ASTnode *expre_parseAST(struct _Content* cd){

    struct Token token;
    //scan first token
    scanner_scan(cd, &token);
    struct ASTnode *ast;
    //ast = binexpr0(cd, &token); //only parse symbol, never deal with operation order
    //ast = additive_expr(cd, &token); // +-*/ with order
    ast = expre_binexpr(cd, &token, 0);
    return ast;
}

// AST tree interpreter
// Copyright (c) 2019 Warren Toomey, GPL3

// List of AST operators
static char *ASTop[] = { "+", "-", "*", "/" };

// Given an AST, interpret the
// operators in it and return
// a final value.
int expre_evaluateAST(struct ASTnode *n) {
  int leftval, rightval;

  // Get the left and right sub-tree values
  if (n->left)
    leftval = expre_evaluateAST(n->left);
  if (n->right)
    rightval = expre_evaluateAST(n->right);

  // Debug: Print what we are about to do
  if (n->op == A_INTLIT)
    printf("int %d\n", n->v.intvalue);
  else
    printf("%d %s %d\n", leftval, ASTop[n->op], rightval);

  switch (n->op) {
    case A_ADD:
      return (leftval + rightval);
    case A_SUBTRACT:
      return (leftval - rightval);
    case A_MULTIPLY:
      return (leftval * rightval);
    case A_DIVIDE:
      return (leftval / rightval);
    case A_INTLIT:
      return (n->v.intvalue);
    default:
      fprintf(stderr, "Unknown AST operator %d\n", n->op);
      exit(1);
  }
}