#include "decl.h"
#include "ast.h"
#include "content.h"
#include "token.h"
#include "writer.h"
#include "expre.h"
#include "misc.h"
#include "sym.h"
#include "gen.h"

extern struct ASTnode *statement_parse(struct _Content *cd, struct _Writer *w, struct Token *token);


void decl_var(struct _Content *cd, struct _Writer *w, struct Token *token){
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

struct ASTnode* decl_function(struct _Content *cd, struct _Writer *w, struct Token *token){
    struct ASTnode *tree;
  int nameslot;

  // Find the 'void', the identifier, and the '(' ')'.
  // For now, do nothing with them
  misc_match(cd, token, T_VOID, "void");
  misc_ident(cd, token);
  nameslot = sym_addglob(cd->context.textBuf);
  misc_lparen(cd, token);
  misc_rparen(cd, token);

  // Get the AST tree for the compound statement
  tree = statement_parse(cd, w, token);

  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return(expre_mkastunary(A_FUNCTION, tree, nameslot));
}