#ifndef _TOKEN_H
#define _TOKEN_H

CPP_START

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Tokens
enum { 
    // + - * / int
 T_EOF, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT
};

// Token structure
struct Token {
  int token;
  int intvalue;
};
CPP_END

#endif