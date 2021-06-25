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
  // EOF, =, + - * /
  T_EOF,
  //binary operators

  T_ASSIGN,         //lower precedences than '+-*/'
  T_LOGOR,          // ||
  T_LOGAND,         // &&
  T_OR,             // |
  T_XOR,            // ^
  T_AMPER,          // &
   // ==  !=  <  >  <=  >=
  T_EQ,
  T_NE,
  T_LT,
  T_GT,
  T_LE,
  T_GE,
  T_LSHIFT,     // <<
  T_RSHIFT,     // >>

// + - *  /
  T_PLUS,
  T_MINUS,
  T_STAR,
  T_SLASH,
  //other operators
  T_INC,    // ++
  T_DEC,    // --
  T_INVERT, // ~
  T_LOGNOT, // !
 
  //type words
  T_VOID,  
  T_CHAR, 
  T_INT,  
  T_LONG,
  //other keywords
  T_IF,
  T_ELSE,
  T_WHILE,
  T_FOR,
  T_RETURN,

//----------------- structural tokens ===================
  //int value  string value  ; 
  T_INTLIT,
  T_STRLIT,
  T_SEMI, 
  T_IDENT, // identifier

  // { }  ( )  [] ,
  T_LBRACE,
  T_RBRACE,
  T_LPAREN,
  T_RPAREN,
  T_LBRACKET, 
  T_RBRACKET,
  T_COMMA
};

// Token structure
struct Token {
  int token;
  int intvalue;
};
CPP_END

#endif