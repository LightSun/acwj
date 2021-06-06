#ifndef _EXPRE_H
#define _EXPRE_H
#include "common_headers.h"
#include "ast.h"
CPP_START

struct _Content;
struct _Writer;
struct Token;

struct ASTnode *expre_mkastnode(int op, int type, struct ASTnode *left, struct ASTnode *mid,
                          struct ASTnode *right, int intvalue);
// Make an AST leaf node
struct ASTnode *expre_mkastleaf(int op,  int type, int intvalue);

// Make a unary AST node: only one child
struct ASTnode *expre_mkastunary(int op, int type, struct ASTnode *left, int intvalue);

struct ASTnode *expre_funccall(struct _Content *cd, struct _Writer* w, struct Token *token);

struct ASTnode* expre_binexpr(struct _Content* cd, struct _Writer* w,struct Token* token, int ptp);

// Parse a prefix expression and return 
// a sub-tree representing it.
struct ASTnode *expre_prefix(struct _Content *cd, struct _Writer *w, struct Token *token);

void expre_dumpAST(struct _Content *cd, struct ASTnode *n, int label, int level);

CPP_END
#endif