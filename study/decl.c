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
#include "types.h"

extern struct ASTnode *statement_parse(struct _Content *cd, struct _Writer *w, struct Token *token);

//may be a list of var.
void decl_var(struct _Content *cd, struct _Writer *w, struct Token *token, int type)
{
  int id;

  while (1) {
    // Text now has the identifier's name.
    // Add it as a known identifier
    // and generate its space in assembly
    id = sym_addglob(cd->context->globalState, cd->context->textBuf, type, S_VARIABLE, 0);
     gen_globsym(w, id);

    // If the next token is a semicolon,
    // skip it and return.
    if (token->token == T_SEMI) {
      scanner_scan(cd, token);
      return;
    }
    // If the next token is a comma, skip it,
    // get the identifier and loop back
    if (token->token == T_COMMA) {
      scanner_scan(cd, token);
      misc_ident(cd, token);
      continue;
    }
    WRITER_PUBLISH_ERROR(w, "Missing , or ; after identifier");
  }
}

struct ASTnode *decl_function(struct _Content *cd, struct _Writer *w, struct Token *token, int type)
{
  struct ASTnode *tree, *finalstmt;
  int nameslot, endlabel;

  // Text now has the identifier's name.
  // Get a label-id for the end label, add the function
  // to the symbol table, and set the Functionid global
  // to the function's symbol-id
  endlabel = (w->context->label++);;
  nameslot = sym_addglob(cd->context->globalState, cd->context->textBuf, type, S_FUNCTION, endlabel);
  cd->context->functionid = nameslot;

  // ( )
  misc_lparen(cd, token);
  misc_rparen(cd, token);

  // Get the AST tree for the compound statement
  tree = statement_parse(cd, w, token);

  // If the function type isn't P_VOID ..
  if (type != P_VOID) {

    // Error if no statements in the function
    if (tree == NULL)
      WRITER_PUBLISH_ERROR(w, "No statements in function with non-void type");

    // Check that the last AST operation in the
    // compound statement was a return statement
    finalstmt = (tree->op == A_GLUE) ? tree->right : tree;
    if (finalstmt == NULL || finalstmt->op != A_RETURN)
      WRITER_PUBLISH_ERROR(w,"No return for function with non-void type");
  }
  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return (expre_mkastunary(A_FUNCTION, type, tree, nameslot));
}

// Parse one or more global declarations, either
// variables or functions
void decl_global(struct _Content *cd, struct _Writer *w, struct Token *token) {
  struct ASTnode *tree;
  int type;

  while (1) {

    // We have to read past the type and identifier
    // to see either a '(' for a function declaration
    // or a ',' or ';' for a variable declaration.
    // Text is filled in by the ident() call.
    type = parse_type(cd, token);
    misc_ident(cd, token);
    if (token->token == T_LPAREN) {

      // Parse the function declaration and
      // generate the assembly code for it
      tree = decl_function(cd, w, token, type);
      gen_genAST(cd, tree, w, NOREG, 0);
    } else {

      // Parse the global variable declaration
      decl_var(cd, w, token, type);
    }

    // Stop when we have reached EOF
    if (token->token == T_EOF)
      break;
  }
}