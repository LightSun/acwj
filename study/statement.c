#include "statement.h"
#include "decl.h"
#include "expre.h"
#include "misc.h"
#include "sym.h"
#include "types.h"
#include "utils.h"

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
 */

static struct ASTnode *print_statement(Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *tree;
  int lefttype, righttype;

  // Match a 'print' as the first token
  misc_match(cd, token, T_PRINT, "print");

  // Parse the following expression
  tree = expre_binexpr(cd, w, token, 0);

  // Ensure the two types are compatible.
  lefttype = P_INT;
  righttype = tree->type;
  if (!types_compatible(w, &lefttype, &righttype, 0))
  {
    fatal(cd, "Incompatible types");
  }

  // Widen the tree if required.
  if (righttype)
    tree = expre_mkastunary(righttype, P_INT, tree, 0);

  //make an print ast tree
  tree = expre_mkastunary(A_PRINT, P_NONE, tree, 0);

  return tree;
}

static struct ASTnode *assignment_statement(Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *left, *right, *tree;
  int lefttype, righttype;
  int id;

  // Ensure we have an identifier
  misc_ident(cd, token);

  // This could be a variable or a function call.
  // If next token is '(', it's a function call
  if (token->token == T_LPAREN)
    return (expre_funccall(cd, w, token));

  // Check it's been defined then make a leaf node for it
  if ((id = sym_findglob(cd->context->globalState, cd->context->textBuf)) == -1)
  {
    fprintf(stderr, "Undeclared variable %s\n", cd->context->textBuf);
    exit(1);
  }
  right = expre_mkastleaf(A_LVIDENT, sym_getGlob(cd->context->globalState, id)->type, id);

  // Ensure we have an equals sign
  misc_match(cd, token, T_ASSIGN, "=");

  // Parse the following expression
  left = expre_binexpr(cd, w, token, 0);

  // Ensure the two types are compatible.
  lefttype = left->type;
  righttype = right->type;
  if (!types_compatible(w, &lefttype, &righttype, 1))
    fatal(cd, "Incompatible types");

  // Widen the left if required.
  if (lefttype)
    left = expre_mkastunary(lefttype, right->type, left, 0);

  // Make an assignment AST tree
  tree = expre_mkastnode(A_ASSIGN, P_INT, left, NULL, right, 0);

  return tree;
}

// Parse an IF statement including
// any optional ELSE clause
// and return its AST
struct ASTnode *if_statement(Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *condAST, *trueAST, *falseAST = NULL;

  // Ensure we have 'if' '('
  misc_match(cd, token, T_IF, "if");
  misc_lparen(cd, token);

  // Parse the following expression
  // and the ')' following. Ensure
  // the tree's operation is a comparison.
  condAST = expre_binexpr(cd, w, token, 0);

  if (condAST->op < A_EQ || condAST->op > A_GE)
  {
    printf("Bad comparison operator");
    exit(1);
  }
  misc_rparen(cd, token);

  // Get the AST for the compound statement
  trueAST = statement_parse(cd, w, token);

  // If we have an 'else', skip it
  // and get the AST for the compound statement
  if (token->token == T_ELSE)
  {
    //scan(&Token);
    scanner_scan(cd, token);
    falseAST = statement_parse(cd, w, token);
  }
  // Build and return the AST for this statement
  return (expre_mkastnode(A_IF, P_NONE, condAST, trueAST, falseAST, 0));
}

static struct ASTnode *single_statement(Content *cd, struct _Writer *w, struct Token *token);

// while_statement: 'while' '(' true_false_expression ')' compound_statement  ;
// Parse a WHILE statement
// and return its AST
struct ASTnode *while_statement(Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *condAST, *bodyAST;

  // Ensure we have 'while' '('
  misc_match(cd, token, T_WHILE, "while");
  misc_lparen(cd, token);

  // Parse the following expression
  // and the ')' following. Ensure
  // the tree's operation is a comparison.
  condAST = expre_binexpr(cd, w, token, 0);
  if (condAST->op < A_EQ || condAST->op > A_GE)
  {
    fprintf(stderr, "Bad comparison operator");
    exit(1);
  }
  misc_rparen(cd, token);

  // Get the AST for the compound statement
  bodyAST = statement_parse(cd, w, token);

  // Build and return the AST for this statement
  return (expre_mkastnode(A_WHILE, P_NONE, condAST, NULL, bodyAST, 0));
}

// Parse a FOR statement
// and return its AST
static struct ASTnode *for_statement(Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *condAST, *bodyAST;
  struct ASTnode *preopAST, *postopAST;
  struct ASTnode *tree;

  // Ensure we have 'for' '('
  misc_match(cd, token, T_FOR, "for");
  misc_lparen(cd, token);

  // Get the pre_op statement and the ';'
  preopAST = single_statement(cd, w, token);
  misc_semi(cd, token);

  // Get the condition and the ';'
  condAST = expre_binexpr(cd, w, token, 0);
  if (condAST->op < A_EQ || condAST->op > A_GE)
  {
    fprintf(stderr, "Bad comparison operator");
    exit(1);
  }
  misc_semi(cd, token);

  // Get the post_op statement and the ')'
  postopAST = single_statement(cd, w, token);
  misc_rparen(cd, token);

  // Get the compound statement which is the body
  bodyAST = statement_parse(cd, w, token);

  // For now, all four sub-trees have to be non-NULL.
  // Later on, we'll change the semantics for when some are missing

  // Glue the compound statement and the postop tree
  tree = expre_mkastnode(A_GLUE, P_NONE, bodyAST, NULL, postopAST, 0);

  // Make a WHILE loop with the condition and this new body
  tree = expre_mkastnode(A_WHILE, P_NONE, condAST, NULL, tree, 0);

  // And glue the preop tree to the A_WHILE tree
  return (expre_mkastnode(A_GLUE, P_NONE, preopAST, NULL, tree, 0));
  //https://github.com/LightSun/acwj/tree/master/10_For_Loops
}

// Parse a return statement and return its AST
static struct ASTnode *return_statement(Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *tree;
  int returntype, functype;

  // Can't return a value if function returns P_VOID
  if (sym_getGlob(cd->context->globalState, cd->context->functionid)->type == P_VOID)
    fatal(cd, "Can't return from a void function");

  // Ensure we have 'return' '('
  misc_match(cd, token, T_RETURN, "return");
  misc_lparen(cd, token);

  // Parse the following expression
  tree = expre_binexpr(cd, w, token, 0);

  // Ensure this is compatible with the function's type
  returntype = tree->type;
  functype = sym_getGlob(cd->context->globalState, cd->context->functionid)->type;
  if (!types_compatible(w, &returntype, &functype, 1))
    fatal(cd, "Incompatible types");

  // Widen the left if required.
  if (returntype)
    tree = expre_mkastunary(returntype, functype, tree, 0);

  // Add on the A_RETURN node
  tree = expre_mkastunary(A_RETURN, P_NONE, tree, 0);

  // Get the ')'
  misc_rparen(cd, token);
  return (tree);
}

// Parse a single statement
// and return its AST
static struct ASTnode *single_statement(Content *cd, struct _Writer *w, struct Token *token)
{
  int type;

  switch (token->token)
  {
  case T_PRINT:
    return (print_statement(cd, w, token));

  case T_CHAR:
  case T_INT:
  case T_LONG:
    // The beginning of a variable declaration.
    // Parse the type and get the identifier.
    // Then parse the rest of the declaration.
    // XXX: These are globals at present.
    type = parse_type(cd, token);
    misc_ident(cd, token);
    decl_var(cd, w, token, type);
    return (NULL); // No AST generated here

  case T_IDENT:
    return (assignment_statement(cd, w, token));

  case T_IF:
    return (if_statement(cd, w, token));

  case T_WHILE:
    return (while_statement(cd, w, token));

  case T_FOR:
    return (for_statement(cd, w, token));

  case T_RETURN:
    return (return_statement(cd, w, token));

  default:
    fprintf(stderr, "Syntax error, token = %d", token->token);
    exit(1);
  }
}

// Parse a compound statement with "{ + }"
// and return its AST
struct ASTnode *statement_parse(Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *left = NULL;
  struct ASTnode *tree;

  // Require a left curly bracket
  misc_lbrace(cd, token);

  while (1)
  {
    // Parse a single statement
    tree = single_statement(cd, w, token);

    // Some statements must be followed by a semicolon
    if (tree != NULL && (tree->op == A_PRINT || tree->op == A_ASSIGN || tree->op == A_RETURN || tree->op == A_FUNCCALL))
    {
      misc_semi(cd, token);
    }

    // For each new tree, either save it in left
    // if left is empty, or glue the left and the
    // new tree together
    if (tree != NULL)
    {
      if (left == NULL)
        left = tree;
      else
        left = expre_mkastnode(A_GLUE, P_NONE, left, NULL, tree, 0);
    }
    // When we hit a right curly bracket,
    // skip past it and return the AST
    if (token->token == T_RBRACE)
    {
      misc_rbrace(cd, token);
      return (left);
    }
  }
}