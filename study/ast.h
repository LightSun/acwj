
#ifndef _AST_H
#define _AST_H

#include "common_headers.h"

CPP_START

// AST node types
enum {
  A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT,
  //identifier
  A_IDENT, 
  //used to store value to register var
  A_LVIDENT,
  //assign
   A_ASSIGN
};

// Abstract Syntax Tree structure
struct ASTnode {
  int op;				// "Operation" to be performed on this tree
  struct ASTnode *left;			// Left and right child trees
  struct ASTnode *right;
  union {
    int intvalue;               // For A_INTLIT, the integer value
    int id;                     // For A_IDENT, the symbol slot number()
  } v;
};

CPP_END

#endif