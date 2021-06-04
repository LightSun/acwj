
#include "expre.h"
#include "content.h"
#include "misc.h"
#include "scanner.h"
#include "sym.h"
#include "token.h"
#include "types.h"
#include "utils.h"
#include "writer.h"

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
  if ((id = sym_findglob(cd->context->globalState, cd->context->textBuf)) == -1)
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
  tree = expre_mkastunary(A_FUNCCALL, sym_getGlob(cd->context->globalState, id)->type, tree, id);

  // Get the ')'
  misc_rparen(cd, token);
  return (tree);
}

// Parsing of expressions
// Copyright (c) 2019 Warren Toomey, GPL3

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
    /** can be: 
   x= fred + jim;
   x= fred(5) + jim;
     * */
    //this should be a var-del or func-call
    scanner_scan(cd, token);
    // It's a '(', so a function call
    if (token->token == T_LPAREN)
    {
      return (expre_funccall(cd, w, token));
    }
    // Not a function call, so reject the new token
    scanner_reject_token(cd, token);

    // Check that this identifier exists
    id = sym_findglob(cd->context->globalState, cd->context->textBuf);
    if (id == -1)
    {
      fatals(cd, "Unknown variable %s.", cd->context->textBuf);
    }

    // Make a leaf AST node for it
    n = expre_mkastleaf(A_IDENT, sym_getGlob(cd->context->globalState, id)->type, id);
    break;

  default:
    fprintf(stderr, "syntax error on line %d\n", cd->context->line);
    exit(1);
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
  if (tokentype > T_EOF && tokentype < T_INTLIT)
    return (tokentype);
  CONTENT_PUBLISH_ERROR(cd, "Syntax error, token = %d", tokentype);
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
    0, 10,         // T_EOF,  T_ASSIGN
    20, 20,        // T_PLUS, T_MINUS
    30, 30,        // T_STAR, T_SLASH
    40, 40,        // T_EQ, T_NE
    50, 50, 50, 50 // T_LT, T_GT, T_LE, T_GE
};

// Check that we have a binary operator and
// return its precedence.
static int op_precedence(struct _Content *cd, int tokentype)
{
  int prec = OpPrec[tokentype];
  if (prec == 0)
  {
    CONTENT_PUBLISH_ERROR(cd, "syntax error on line %d, token %d\n", cd->context->line, tokentype);
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
  left = prefix(cd, w, token);

  // If we hit a semicolon or ')', return just the left node
  tokentype = token->token;
  if (tokentype == T_SEMI || tokentype == T_RPAREN)
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
        WRITER_PUBLISH_ERROR(w,"Incompatible types in binary expression");
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
    if (tokentype == T_SEMI || tokentype == T_RPAREN)
    {
      left->rvalue = 1;
      return (left);
    }
  
  }
   // Return the tree we have when the precedence
  // is the same or lower
  left->rvalue = 1; 
  return(left);
}

// Parse a prefix expression and return
// a sub-tree representing it.
struct ASTnode *prefix(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *tree;
  switch (token->token)
  {
  case T_AMPER: //&
    // Get the next token and parse it
    // recursively as a prefix expression
    scanner_scan(cd, token);
    tree = prefix(cd, w, token);

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
    tree = prefix(cd, w, token);

    // For now, ensure it's either another deref or an
    // identifier
    if (tree->op != A_IDENT && tree->op != A_DEREF)
      WRITER_PUBLISH_ERROR(w, "* operator must be followed by an identifier or *");

    // Prepend an A_DEREF operation to the tree
    tree = expre_mkastunary(A_DEREF, types_value_at(tree->type), tree, 0);
    break;
  default:
    tree = primary(cd, w, token);
  }
  return (tree);
}