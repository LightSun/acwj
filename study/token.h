#ifndef _TOKEN_H
#define _TOKEN_H

CPP_START

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Tokens
enum
{
  // + - * / int ; print
  T_EOF,
  T_PLUS,
  T_MINUS,
  T_STAR,
  T_SLASH,
  
  T_INTLIT,
  T_SEMI,

  T_EQUALS, // =
  T_IDENT,  // identifier
  // ============= keywords
  T_PRINT,   // print

  T_INT      // int identifier
};

// Token structure
struct Token {
  int token;
  int intvalue;
};
CPP_END

#endif