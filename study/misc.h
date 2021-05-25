#ifndef _MISC_H
#define _MISC_H
#include "token.h"
#include "scanner.h"
#include "content.h"
CPP_START

void misc_match(Content *cd, struct Token *token, int t, char *what);
// Match a semicon and fetch the next token
void misc_semi(Content *cd, struct Token *token);

//match identifier and fetch next token
void misc_ident(Content *cd, struct Token *token);

// (
void misc_lparen(Content *cd, struct Token *token);
// )
void misc_rparen(Content *cd, struct Token *token);

// {
void misc_lbrace(Content *cd, struct Token *token);
// }
void misc_rbrace(Content *cd, struct Token *token);

CPP_END
#endif