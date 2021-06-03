
#ifndef _AST_H
#define _AST_H

#include "common_headers.h"

CPP_START

// Primitive types
enum
{
  //may be complex expre named GLUE
  P_NONE,
  P_VOID,
  P_CHAR,
  P_INT,
  P_LONG,
  // pointers
  P_VOIDPTR,
  P_CHARPTR,
  P_INTPTR,
  P_LONGPTR
};

// AST node types. The first few line up
// with the related tokens
enum
{
  A_ADD = 1,
  A_SUBTRACT,
  A_MULTIPLY,
  A_DIVIDE,
  //==  !=  <  >  <=  >=
  A_EQ,
  A_NE,
  A_LT,
  A_GT,
  A_LE,
  A_GE,
  //int value
  A_INTLIT,
  //identifier
  A_IDENT,
  //used to store value to register var
  A_LVIDENT,
  //assign
  A_ASSIGN,
  // print,   complex statement  ,if
  A_PRINT,
  A_GLUE,
  A_IF,
  //while
  A_WHILE,

  A_FUNCTION,

  A_WIDEN, //sometimes: need widen. like char to int

  A_FUNCCALL, // function call
  A_RETURN,

  A_ADDR,  // &p
  A_DEREF, // *p

  A_SCALE, // its value is scaled by the size of a type.
};

// Abstract Syntax Tree structure
struct ASTnode
{
  int op;               // "Operation" to be performed on this tree. ast op like A_WHILE and etc.
  int type;             // Type of any expression this tree generates. like P_INT and etc.
  struct ASTnode *left; // Left and right child trees
  struct ASTnode *mid;
  struct ASTnode *right;
  union
  {
    int intvalue; // For A_INTLIT, the integer value
    int id;       // For A_IDENT, the symbol slot number()
    int size;     // For A_SCALE, the size to scale by
  } v;            // For A_FUNCCALL, the symbol slot number
};

// Use NOREG when the AST generation
// functions have no register to return
#define NOREG -1

CPP_END

#endif