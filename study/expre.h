#include "common_headers.h"
#include "ast.h"
CPP_START

struct _ContentDelegate;

// Return an AST tree whose root is a binary operator
struct ASTnode* expre_parseAST(struct _ContentDelegate* cd);
int expre_evaluateAST(struct ASTnode *n);

CPP_END