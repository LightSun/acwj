#ifndef _EXPRE_H
#define _EXPRE_H
#include "common_headers.h"
#include "ast.h"
CPP_START

struct _Content;
struct Token;

struct ASTnode *expre_mkastnode(int op, struct ASTnode *left,
                          struct ASTnode *right, int intvalue);
// Make an AST leaf node
struct ASTnode *expre_mkastleaf(int op, int intvalue);

// Make a unary AST node: only one child
struct ASTnode *expre_mkastunary(int op, struct ASTnode *left, int intvalue);

// Return an AST tree whose root is a binary operator
struct ASTnode* expre_parseAST(struct _Content* cd);

struct ASTnode* expre_binexpr(struct _Content* cd, struct Token* token, int ptp);

int expre_evaluateAST(struct ASTnode *n);

CPP_END
#endif