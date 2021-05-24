#include "common_headers.h"
#include "ast.h"
CPP_START

struct _Content;

// Return an AST tree whose root is a binary operator
struct ASTnode* expre_parseAST(struct _Content* cd);

struct ASTnode* expre_binexpr(struct _Content *cd, struct Token* token, int ptp);

int expre_evaluateAST(struct ASTnode *n);

CPP_END