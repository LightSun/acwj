
#include "ast.h"
#include "content.h"
#include "token.h"
#include "scanner.h"
#include "sym.h"
#include "utils.h"

//grammer
/**
 compound_statement: '{' '}'          // empty, i.e. no statement
      |      '{' statement '}'
      |      '{' statement statements '}'
      ;

 statement: print_statement
      |     declaration
      |     assignment_statement
      |     if_statement
      ;

 print_statement: 'print' expression ';'  ;

 declaration: 'int' identifier ';'  ;

 assignment_statement: identifier '=' expression ';'   ;

 if_statement: if_head
      |        if_head 'else' compound_statement
      ;

 if_head: 'if' '(' true_false_expression ')' compound_statement  ;

 identifier: T_IDENT ;
 * 
*/

// AST tree functions
// Copyright (c) 2019 Warren Toomey, GPL3

// Build and return a generic AST node
struct ASTnode *expre_mkastnode(int op, int type, struct ASTnode *left, struct ASTnode *mid,
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
  n->type = type;
  n->left = left;
  n->right = right;
  n->mid = mid;
  n->v.intvalue = intvalue;
  return (n);
}


// Make an AST leaf node
struct ASTnode *expre_mkastleaf(int op, int type, int intvalue) {
  return (expre_mkastnode(op, type, NULL, NULL, NULL, intvalue));
}

// Make a unary AST node: only one child
struct ASTnode *expre_mkastunary(int op, int type,struct ASTnode *left, int intvalue) {
  return (expre_mkastnode(op, type, left, NULL, NULL, intvalue));
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
      // Make it a P_CHAR if it's within the P_CHAR range
      if ((token->intvalue) >= 0 && (token->intvalue < 256)){
          n = mkastleaf(A_INTLIT, P_CHAR, token->intvalue);
      }else{
          n = expre_mkastleaf(A_INTLIT, P_INT, token->intvalue);
      }
      break;

    case T_IDENT:
       // Check that this identifier exists
      id = sym_findglob(cd->context.textBuf);
      if (id == -1){
        fatals(cd, "Unknown variable %s.", cd->context.textBuf);
      }

      // Make a leaf AST node for it
      n = expre_mkastleaf(A_IDENT, sym_getGlob(id)->type, id);
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

  // If we hit a semicolon or ')', return just the left node
  tokentype = token->token;
  if (tokentype == T_SEMI || tokentype == T_RPAREN)
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
    left = expre_mkastnode(arithop(cd, tokentype), left, NULL, right, 0);

     // Update the details of the current token.
    // If we hit a semicolon, return just the left node
    tokentype = token->token;
    if (tokentype == T_SEMI || tokentype == T_RPAREN)
      return (left);
  }

  // Return the tree we have when the precedence
  // is the same or lower
  return (left);
}

