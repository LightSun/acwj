#include "decl.h"
#include "ast.h"
#include "content.h"
#include "expre.h"
#include "gen.h"
#include "misc.h"
#include "sym.h"
#include "token.h"
#include "utils.h"
#include "writer.h"

extern struct ASTnode *statement_parse(struct _Content *cd, struct _Writer *w, struct Token *token);

// Parse the current token and
// return a primitive type enum value
static int parse_type(struct _Content *cd, int t)
{
  if (t == T_CHAR)
    return (P_CHAR);
  if (t == T_INT)
    return (P_INT);
  if (t == T_VOID)
    return (P_VOID);
  fatald(cd, "Illegal type, token", t);
}

void decl_var(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  int id, type;

  // Get the type of the variable, then the identifier
  type = parse_type(cd, token->token);

  scanner_scan(cd, token);
  misc_ident(cd, token);
  // textBuf now has the identifier's name.
  // Add it as a known identifier
  // and generate its space in assembly
  id = sym_addglob(cd->context.textBuf, type, S_VARIABLE);

  SymTable* st = sym_getGlob(id);
  gen_globsym(w, st->type, st->name);
  
  //match final semicolon(';')
  misc_semi(cd, token);
}

struct ASTnode *decl_function(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *tree;
  int nameslot;

  // Find the 'void', the identifier, and the '(' ')'.
  // For now, do nothing with them
  misc_match(cd, token, T_VOID, "void");
  misc_ident(cd, token);
  nameslot = sym_addglob(cd->context.textBuf, P_VOID, S_FUNCTION);
  misc_lparen(cd, token);
  misc_rparen(cd, token);

  // Get the AST tree for the compound statement
  tree = statement_parse(cd, w, token);

  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return (expre_mkastunary(A_FUNCTION, P_VOID, tree, nameslot));
}