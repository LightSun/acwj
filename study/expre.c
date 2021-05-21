
#include "ast.h"
#include "content_delegate.h"
#include "token.h"
#include "scanner.h"

// AST tree functions
// Copyright (c) 2019 Warren Toomey, GPL3

// Build and return a generic AST node
static struct ASTnode *mkastnode(int op, struct ASTnode *left,
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
  n->intvalue = intvalue;
  return (n);
}


// Make an AST leaf node
static struct ASTnode *mkastleaf(int op, int intvalue) {
  return (mkastnode(op, NULL, NULL, intvalue));
}

// Make a unary AST node: only one child
static struct ASTnode *mkastunary(int op, struct ASTnode *left, int intvalue) {
  return (mkastnode(op, left, NULL, intvalue));
}

// Parsing of expressions
// Copyright (c) 2019 Warren Toomey, GPL3

// Parse a primary factor and return an
// AST node representing it.
static struct ASTnode *primary(struct _ContentDelegate* cd, struct Token* token) {
  struct ASTnode *n;

  // For an INTLIT token, make a leaf AST node for it
  // and scan in the next token. Otherwise, a syntax error
  // for any other token type.
  switch (token->token) {
    case T_INTLIT:
      n = mkastleaf(A_INTLIT, token->intvalue);
      scanner_scan(cd, token);
      return (n);
    default:
      fprintf(stderr, "syntax error on line %d\n", scanner_line_get());
      exit(1);
  }
}


// Convert a token into an AST operation.
static int arithop(int tok) {
  switch (tok) {
    case T_PLUS:
      return (A_ADD);
    case T_MINUS:
      return (A_SUBTRACT);
    case T_STAR:
      return (A_MULTIPLY);
    case T_SLASH:
      return (A_DIVIDE);
    default:
      fprintf(stderr, "unknown token in arithop() on line %d\n", scanner_line_get());
      exit(1);
  }
}

// Return an AST tree whose root is a binary operator
// only parse not deal with operation order
static struct ASTnode *binexpr0(struct _ContentDelegate* cd, struct Token* token){
  struct ASTnode *n, *left, *right;
  int nodetype;

  // Get the integer literal on the left.
  // Fetch the next token at the same time.
  left = primary(cd, token);

  // If no tokens left, return just the left node
  if (token->token == T_EOF)
    return (left);

  // Convert the token into a node type
  nodetype = arithop(token->token);

  // Get the next token in
  scanner_scan(cd, token);

  // Recursively get the right-hand tree
  right = binexpr0(cd , token);

  // Now build a tree with both sub-trees
  n = mkastnode(nodetype, left, right, 0);
  return (n);
}

// Return an AST tree whose root is a '*' or '/' binary operator
struct ASTnode *multiplicative_expr(struct _ContentDelegate* cd, struct Token* token) {
  struct ASTnode *left, *right;
  int tokentype;

  // Get the integer literal on the left.
  // Fetch the next token at the same time.
  left = primary(cd, token);

  // If no tokens left, return just the left node
  tokentype = token->token;
  if (tokentype == T_EOF)
    return (left);

  // While the token is a '*' or '/'
  while ((tokentype == T_STAR) || (tokentype == T_SLASH)) {
    // Fetch in the next integer literal
    //scan(&Token);
    scanner_scan(cd, token);

    right = primary(cd, token);

    // Join that with the left integer literal
    left = mkastnode(arithop(tokentype), left, right, 0);

    // Update the details of the current token.
    // If no tokens left, return just the left node
    tokentype = token->token;
    if (tokentype == T_EOF)
      break;
  }

  // Return whatever tree we have created
  return (left);
}

// Return an AST tree whose root is a '+' or '-' binary operator
struct ASTnode *additive_expr(struct _ContentDelegate* cd, struct Token* token) {
  struct ASTnode *left, *right;
  int tokentype;

  // Get the left sub-tree at a higher precedence than us
  left = multiplicative_expr(cd, token);

  // If no tokens left, return just the left node
  tokentype = token->token;
  if (tokentype == T_EOF)
    return (left);

  // Cache the '+' or '-' token type

  // Loop working on token at our level of precedence
  while (1) {
    // Fetch in the next integer literal
    //scan(&Token);
    scanner_scan(cd, token);

    // Get the right sub-tree at a higher precedence than us
    right = multiplicative_expr(cd, token);

    // Join the two sub-trees with our low-precedence operator
    left = mkastnode(arithop(tokentype), left, right, 0);

    // And get the next token at our precedence
    tokentype = token->token;
    if (tokentype == T_EOF)
      break;
  }

  // Return whatever tree we have created
  return (left);
}

struct ASTnode *expre_parseAST(struct _ContentDelegate* cd){

    struct Token token;
    //scan first token
    scanner_scan(cd, &token);
    //only parse symbol, never deal with operation order
    //struct ASTnode * ast = binexpr0(cd, &token);
    struct ASTnode * ast = additive_expr(cd, &token);
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
    printf("int %d\n", n->intvalue);
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
      return (n->intvalue);
    default:
      fprintf(stderr, "Unknown AST operator %d\n", n->op);
      exit(1);
  }
}