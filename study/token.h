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
  T_ASSIGN, //lower precedences than '+-*/'
  T_PLUS,
  T_MINUS,
  T_STAR,
  T_SLASH,
  // ==  !=  <  >  <=  >=
  T_EQ,
  T_NE,
  T_LT,
  T_GT,
  T_LE,
  T_GE,
  //type words
  T_VOID,  T_CHAR, T_INT,  T_LONG,
  //int value  ; 
  T_INTLIT,
  T_SEMI, 
  T_IDENT, // identifier

  // { }  ( )
  T_LBRACE,
  T_RBRACE,
  T_LPAREN,
  T_RPAREN,
   
  T_AMPER,  // &
  T_LOGAND, // &&

//other keywords
  T_IF,
  T_ELSE,
  T_WHILE,
  T_FOR,
  T_RETURN,

 
  //T_COMMA , // ,
};

// Token structure
struct Token {
  int token;
  int intvalue;
};
CPP_END

#endif