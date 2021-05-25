#include "statement.h"
#include "expre.h"
#include "misc.h"
#include "sym.h"


// statements: statement
//      |      statement statements
//      ;
//
// statement: 'print' expression ';'
//      |     'int'   identifier ';'
//      |     identifier '=' expression ';'
//      ;
//
// identifier: T_IDENT
//      ;

static void print_statement(Content* cd, struct _Writer* w, struct Token* token) {
  struct ASTnode *tree;
  int reg;

  // Match a 'print' as the first token
  misc_match(cd, token, T_PRINT, "print");

  // Parse the following expression and
  // generate the assembly code
  tree = expre_binexpr(cd, token, 0);
  reg = gen_genAST(tree, w, -1);
  gen_printint(w, reg);
  gen_freeregs(w);

  // Match the following semicolon
  misc_semi(cd, token);
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


static void assignment_statement(Content* cd, struct _Writer* w, struct Token* token) {
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
  tree = expre_mkastnode(A_ASSIGN, left, right, 0);

  // Generate the assembly code for the assignment
  gen_genAST(tree, w, -1);
  gen_freeregs(w);

  // Match the following semicolon
  misc_semi(cd, token);
}

// Parse one or more statements
void statement_parse(Content* cd, struct _Writer* w, struct Token* token){
 while (1) {
    switch (token->token) {
    case T_PRINT:
      print_statement(cd , w, token);
      break;
    case T_INT:
      var_declaration(cd , w, token);
      break;
    case T_IDENT:
      assignment_statement(cd , w, token);
      break;
    case T_EOF:
      return;
    default:
      printf("Syntax error, token", token->token);
      exit(1);
    }
  }
}