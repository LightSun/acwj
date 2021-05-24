#include "statement.h"
#include "expre.h"
#include "misc.h"

// Parse one or more statements
void statement_parse(Content* cd, struct _Writer* w, struct Token* token){
  struct ASTnode *tree;
  int reg;

  while (1) {
    // Match a 'print' as the first token
    misc_match(cd, token, T_PRINT, "print");

    // Parse the following expression and
    // generate the assembly code
    tree = expre_binexpr(cd, token, 0);
    reg = gen_genAST(tree, w);
    gen_printint(w, reg);
    gen_freeregs(w);

    // Match the following semicolon
    // and stop if we are at EOF
    misc_semi(cd, token);
    if (token->token == T_EOF)
      return;
  }
}