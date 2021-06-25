#include "decl.h"
#include "ast.h"
#include "content.h"
#include "expre.h"
#include "gen.h"
#include "misc.h"
#include "sym.h"
#include "token.h"
#include "types.h"
#include "utils.h"
#include "writer.h"

extern struct ASTnode *statement_parse(struct _Content *cd, struct _Writer *w, struct Token *token);

/**
 *  variable_declaration: type identifier ';'
        | type identifier '[' P_INTLIT ']' ';'
        ;
*/
//may be a list of var.
void decl_var(struct _Content *cd, struct _Writer *w, struct Token *token, int type, int islocal, int isparam)
{
  // Text now has the identifier's name.
  // If the next token is a '['
  if (token->token == T_LBRACKET)
  {
    //skip past '['
    scanner_scan(cd, token);
    //check we are in array size.
    if (token->token == T_INTLIT)
    {
      // Add this as a known array and generate its space in assembly.
      // We treat the array as a pointer to its elements' type
      if (islocal)
      {
        //sym_addlocal(cd->context->globalState, cd->context->textBuf, types_pointer_to(type), S_ARRAY, 0, token->intvalue);
        CONTENT_PUBLISH_ERROR(cd, "For now, declaration of local arrays is not implemented");
      }
      else
      {
        sym_addglob(cd->context->globalState, cd->context->textBuf, types_pointer_to(type), S_ARRAY, 0, token->intvalue);
      }
    }
    //ensure we are following ']'
    scanner_scan(cd, token);
    misc_match(cd, token, T_RBRACKET, "]");
  }
  else
  {
    //alloc one var .
    if (islocal)
    {
      if(sym_addlocal(cd->context->globalState, cd->context->textBuf, type, S_VARIABLE, isparam, 1) == -1){
           CONTENT_PUBLISH_ERROR(cd, "Duplicate local variable declaration", cd->context->textBuf);
      }
    }
    else
    {
      sym_addglob(cd->context->globalState, cd->context->textBuf, type, S_VARIABLE, 0, 1);
    }
  }
}

// param_declaration: <null>
//           | variable_declaration
//           | variable_declaration ',' param_declaration
//
// Parse the parameters in parentheses after the function name.
// Add them as symbols to the symbol table and return the number
// of parameters.
static int decl_param(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  int type;
  int paramcnt = 0;

  // Loop until the final right parentheses
  while (token->token != T_RPAREN)
  {
    // Get the type and identifier
    // and add it to the symbol table
    type = types_parse_type(cd, token);
    misc_ident(cd, token);
    decl_var(cd, w, token, type, 1, 1);
    paramcnt++;

    // Must have a ',' or ')' at this point
    switch (token->token)
    {
    case T_COMMA:
      scanner_scan(cd, token);
      break;
    case T_RPAREN:
      break;
    default:
      CONTENT_PUBLISH_ERROR(cd, "Unexpected token(%d) in parameter list", token->token);
    }
  }

  // Return the count of parameters
  return (paramcnt);
}

#define nelems posn
struct ASTnode *decl_function(struct _Content *cd, struct _Writer *w, struct Token *token, int type)
{
  struct ASTnode *tree, *finalstmt;
  int nameslot, endlabel;

  // Text now has the identifier's name.
  // Get a label-id for the end label, add the function
  // to the symbol table, and set the Functionid global
  // to the function's symbol-id
  endlabel = (w->context->label++);

  nameslot = sym_addglob(cd->context->globalState, cd->context->textBuf, type, S_FUNCTION, endlabel, 0);
  cd->context->functionid = nameslot;

  int paramcnt;
  // ( )
  misc_lparen(cd, token);
  paramcnt = decl_param(cd, w, token);
  sym_getSymbol(cd->context->globalState, nameslot)->nelems = paramcnt;
  misc_rparen(cd, token);

  // Get the AST tree for the compound statement
  tree = statement_parse(cd, w, token);

  // If the function type isn't P_VOID ..
  if (type != P_VOID)
  {

    // Error if no statements in the function
    if (tree == NULL)
      WRITER_PUBLISH_ERROR(w, "No statements in function with non-void type");

    // Check that the last AST operation in the
    // compound statement was a return statement
    finalstmt = (tree->op == A_GLUE) ? tree->right : tree;
    if (finalstmt == NULL || finalstmt->op != A_RETURN)
      WRITER_PUBLISH_ERROR(w, "No return for function with non-void type");
  }
  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return (expre_mkastunary(A_FUNCTION, type, tree, nameslot));
}

// Parse one or more global declarations, either
// variables or functions
void decl_global(struct _Content *cd, struct _Writer *w, struct Token *token)
{
  struct ASTnode *tree;
  int type;

  while (1)
  {

    // We have to read past the type and identifier
    // to see either a '(' for a function declaration
    // or a ',' or ';' for a variable declaration.
    // Text is filled in by the ident() call.
    type = types_parse_type(cd, token);
    misc_ident(cd, token);
    if (token->token == T_LPAREN) //(
    {

      // Parse the function declaration and
      // generate the assembly code for it
      tree = decl_function(cd, w, token, type);
      if (cd->context->dumpAST)
      {
        expre_dumpAST(cd, tree, NOLABEL, 0);
        fprintf(stdout, "\n\n");
      }
      gen_genAST(cd, tree, w, NOLABEL, 0);

      // Now free the symbols associated
      // with this function
      sym_freeloclsyms(cd->context->globalState);
    }
    else
    {

      // Parse the global variable declaration
      decl_var(cd, w, token, type, 0, 0);
      misc_semi(cd, token);
    }

    // Stop when we have reached EOF
    if (token->token == T_EOF)
      break;
  }
}