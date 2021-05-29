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
  if (t == T_LONG)
    return (P_LONG);
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
  id = sym_addglob(cd->context.globalState, cd->context.textBuf, type, S_VARIABLE, 0);

  SymTable* st = sym_getGlob(cd->context.globalState, id);
  gen_globsym(w, st->type, st->name);
  
  //match final semicolon(';')
  misc_semi(cd, token);
}

struct ASTnode *decl_function(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *tree, *finalstmt;
  int nameslot, type, endlabel;

  // Get the type of the variable, then the identifier
  type = parse_type(cd, token->token);
  scanner_scan(cd, token);
  misc_ident(cd, token);

  // Get a label-id for the end label, add the function
  // to the symbol table, and set the Functionid global
  // to the function's symbol-id
  endlabel = (w->context->label++); //genlabel()
  nameslot = sym_addglob(cd->context.globalState, cd->context.textBuf, type, S_FUNCTION, endlabel);
  cd->context.functionid = nameslot;

// ( )
  misc_lparen(cd, token);
  misc_rparen(cd, token);

  // Get the AST tree for the compound statement
  tree = statement_parse(cd, w, token);

  
  // If the function type isn't P_VOID, check that
  // the last AST operation in the compound statement
  // was a return statement
  if (type != P_VOID) {
    finalstmt = (tree->op == A_GLUE) ? tree->right : tree;
    if (finalstmt == NULL || finalstmt->op != A_RETURN){
      fatal(cd, "No return for function with non-void type");
    }
  }

  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return (expre_mkastunary(A_FUNCTION, type, tree, nameslot));
}