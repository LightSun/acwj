
#ifndef _AST_H
#define _AST_H

#include "common_headers.h"

CPP_START

// AST node types. The first few line up
// with the related tokens
enum {
   A_ADD = 1, A_SUBTRACT, A_MULTIPLY, A_DIVIDE,
   //==  !=  <  >  <=  >=
  A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE,
  //int value
  A_INTLIT,
   //identifier
  A_IDENT, 
   //used to store value to register var
  A_LVIDENT,
   //assign
   A_ASSIGN,
  // print,   ,if
   A_PRINT, A_GLUE, A_IF
};

// Abstract Syntax Tree structure
struct ASTnode {
  int op;				// "Operation" to be performed on this tree
  struct ASTnode *left;			// Left and right child trees
  struct ASTnode *mid;
  struct ASTnode *right;
  union {
    int intvalue;               // For A_INTLIT, the integer value
    int id;                     // For A_IDENT, the symbol slot number()
  } v;
};

#define NOREG -1
CPP_END

#endif