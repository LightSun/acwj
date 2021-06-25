#ifndef _DECL_H
#define _DECL_H

#include "common_headers.h"
CPP_START

struct _Content;
struct _Writer;
struct Token;
struct ASTnode;

void decl_var(struct _Content *cd, struct _Writer *w, struct Token *token, int type, int isLocal, int isParam);

struct ASTnode* decl_function(struct _Content *cd, struct _Writer *w, struct Token *token, int type);

void decl_global(struct _Content *cd, struct _Writer *w, struct Token *token);

CPP_END
#endif