#include "statement.h"
#include "expre.h"
#include "misc.h"
#include "sym.h"

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

static struct ASTnode * print_statement(Content* cd, struct _Writer* w, struct Token* token) {
  struct ASTnode *tree;
  int reg;

  // Match a 'print' as the first token
  misc_match(cd, token, T_PRINT, "print");

  // Parse the following expression
  tree = expre_binexpr(cd, token, 0);
  
  //make an print ast tree
  tree = expre_mkastunary(A_PRINT, tree, 0);

  // Match the following semicolon and return tree
  misc_semi(cd, token);

  return tree;
}

// Parse the declaration of a variable
static void var_declaration(Content* cd, struct _Writer* w, struct Token* token) {

  // Ensure we have an 'int' token followed by an identifier
  // and a semicolon. Text now has the identifier's name.
  // Add it as a known identifier
  misc_match(cd, token, T_INT, "int");
  misc_ident(cd, token);

  sym_addglob(cd->context.textBuf);

  gen_globsym(w, cd->context.textBuf);
  //match final semicolon(';')
  misc_semi(cd, token);
}

static struct ASTnode* assignment_statement(Content* cd, struct _Writer* w, struct Token* token) {
  struct ASTnode *left, *right, *tree;
  int id;

  // Ensure we have an identifier
  misc_ident(cd, token);

  // Check it's been defined then make a leaf node for it
  if ((id = sym_findglob(cd->context.textBuf)) == -1) {
    printf("Undeclared variable", cd->context.textBuf);
    exit(1);
  }
  right = expre_mkastleaf(A_LVIDENT, id);

  // Ensure we have an equals sign
  misc_match(cd, token, T_ASSIGN, "=");

  // Parse the following expression
  left = expre_binexpr(cd, token, 0);

  // Make an assignment AST tree
  tree = expre_mkastnode(A_ASSIGN, left, NULL, right, 0);

  // Match the following semicolon
  misc_semi(cd, token);

  return tree;
}

// Parse an IF statement including
// any optional ELSE clause
// and return its AST
struct ASTnode *if_statement(Content* cd, struct _Writer* w, struct Token* token) {
  struct ASTnode *condAST, *trueAST, *falseAST = NULL;

  // Ensure we have 'if' '('
  misc_match(cd, token, T_IF, "if");
  misc_lparen(cd, token);

  // Parse the following expression
  // and the ')' following. Ensure
  // the tree's operation is a comparison.
  condAST = expre_binexpr(cd, token, 0);

  if (condAST->op < A_EQ || condAST->op > A_GE){
    printf("Bad comparison operator");
    exit(1);
  }
  misc_rparen(cd, token);

  // Get the AST for the compound statement
  trueAST = statement_parse(cd, w, token);

  // If we have an 'else', skip it
  // and get the AST for the compound statement
  if (token->token == T_ELSE) {
    //scan(&Token);
    scanner_scan(cd, token);
    falseAST = statement_parse(cd, w, token);
  }
  // Build and return the AST for this statement
  return (expre_mkastnode(A_IF, condAST, trueAST, falseAST, 0));
}

// Parse a compound statement
// and return its AST
struct ASTnode *statement_parse(Content* cd, struct _Writer* w, struct Token* token) {
  struct ASTnode *left = NULL;
  struct ASTnode *tree;

  // Require a left curly bracket
  misc_lbrace(cd, token);

  while (1) {
      switch (token->token) {
        case T_PRINT:
          tree = print_statement(cd, w, token);
          break;

        case T_INT:
          var_declaration(cd, w, token);
          tree = NULL;            // No AST generated here
          break;

        case T_IDENT:
          tree = assignment_statement(cd, w, token);
          break;

        case T_IF:
          tree = if_statement(cd, w, token);
          break;

      case T_RBRACE:
          // When we hit a right curly bracket,
          // skip past it and return the AST
          misc_rbrace(cd, token);
          return (left);
        default:
          printf("Syntax error, token", token->token);
          exit(1);
      }

      // For each new tree, either save it in left
      // if left is empty, or glue the left and the
      // new tree together
      if (tree) {
        if (left == NULL)
          left = tree;
        else
          left = expre_mkastnode(A_GLUE, left, NULL, tree, 0);
      }
  }
}