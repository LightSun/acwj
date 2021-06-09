#include "ast.h"
#include "content.h"
#include "gen.h"
#include "register.h"
#include "scanner.h"
#include "sym.h"
#include "token.h"
#include "writer.h"

extern struct ASTnode *expre_mkastunary(int op, int type, struct ASTnode *left, int intvalue);

// Types and type handling
// Copyright (c) 2019 Warren Toomey, GPL3

// Return true if a type is an int type
// of any size, false otherwise
int types_inttype(int type)
{
  if (type == P_CHAR || type == P_INT || type == P_LONG)
    return (1);
  return (0);
}

// Return true if a type is of pointer type
int types_ptrtype(int type)
{
  if (type == P_VOIDPTR || type == P_CHARPTR ||
      type == P_INTPTR || type == P_LONGPTR)
    return (1);
  return (0);
}

// Given two primitive types, return true if they are compatible, false otherwise.
// Also return either zero or an A_WIDEN
// operation if one has to be widened to match the other.
// If onlyright is true, only widen left to right.
int types_compatible(struct _Writer *w, int *left, int *right, int onlyright)
{
  int leftsize, rightsize;

  // Same types, they are compatible
  if (*left == *right)
  {
    *left = *right = 0;
    return (1);
  }
  // Get the sizes for each type
  leftsize = gen_primsize(w, *left);
  rightsize = gen_primsize(w, *right);

  // Types with zero size are not
  // not compatible with anything
  if ((leftsize == 0) || (rightsize == 0))
    return (0);

  // Widen types as required
  if (leftsize < rightsize)
  {
    *left = A_WIDEN;
    *right = 0;
    return (1);
  }
  if (rightsize < leftsize)
  {
    if (onlyright)
      return (0);
    *left = 0;
    *right = A_WIDEN;
    return (1);
  }
  // Anything remaining is the same size
  // and thus compatible
  *left = *right = 0;
  return (1);
}

// Given a primitive type, return
// the type which is a pointer to it
int types_pointer_to(int type)
{
  int newtype;
  switch (type)
  {
  case P_VOID:
    newtype = P_VOIDPTR;
    break;
  case P_CHAR:
    newtype = P_CHARPTR;
    break;
  case P_INT:
    newtype = P_INTPTR;
    break;
  case P_LONG:
    newtype = P_LONGPTR;
    break;
  default:
    fprintf(stderr, "Unrecognised in types_pointer_to(...): type = ", type);
    exit(1);
  }
  return (newtype);
}

// Given a primitive pointer type, return
// the type which it points to
int types_value_at(int type)
{
  int newtype;
  switch (type)
  {
  case P_VOIDPTR:
    newtype = P_VOID;
    break;
  case P_CHARPTR:
    newtype = P_CHAR;
    break;
  case P_INTPTR:
    newtype = P_INT;
    break;
  case P_LONGPTR:
    newtype = P_LONG;
    break;
  default:
    fprintf(stderr, "Unrecognised in types_value_at(): type", type);
    exit(1);
  }
  return (newtype);
}

// Parse the current token and
// return a primitive type enum value
int types_parse_type(struct _Content *cd, struct Token *token)
{
  int type;
  switch (token->token)
  {
  case T_VOID:
    type = P_VOID;
    break;
  case T_CHAR:
    type = P_CHAR;
    break;
  case T_INT:
    type = P_INT;
    break;
  case T_LONG:
    type = P_LONG;
    break;
  default:
    CONTENT_PUBLISH_ERROR(cd, "Illegal type, token = %d", token->token);
  }

  //check next '*' exist or not
  while (1)
  {
    scanner_scan(cd, token);
    if (token->token != T_STAR)
      break;
    type = types_pointer_to(type);
  }
  // We leave with the next token already scanned
  return type;
}

struct ASTnode *types_modify_type(struct ASTnode *tree, struct _Writer *w, int rtype, int op)
{
  int ltype;
  int lsize, rsize;

  ltype = tree->type;

  // Compare scalar int types
  if (types_inttype(ltype) && types_inttype(rtype))
  {

    // Both types same, nothing to do
    if (ltype == rtype)
      return (tree);

    // Get the sizes for each type
    lsize = WRITER_G_REG(w)->register_cgprimsize(WRITER_G_REG_CTX(w), ltype);
    rsize = WRITER_G_REG(w)->register_cgprimsize(WRITER_G_REG_CTX(w), rtype);

    // Tree's size is too big
    if (lsize > rsize)
      return (NULL);

    // Widen to the right
    if (rsize > lsize)
      return (expre_mkastunary(A_WIDEN, rtype, tree, 0));
  }

  // For pointers on the left
  if (types_ptrtype(ltype))
  {
    // OK is same type on right and not doing a binary op
    if (op == 0 && ltype == rtype)
      return (tree);
  }

  // We can scale only on A_ADD or A_SUBTRACT operation
  if (op == A_ADD || op == A_SUBTRACT)
  {

    // Left is int type, right is pointer type and the size
    // of the original type is >1: scale the left
    if (types_inttype(ltype) && types_ptrtype(rtype))
    {
      rsize = WRITER_G_REG(w)->register_cgprimsize(WRITER_G_REG_CTX(w), types_value_at(rtype));
      if (rsize > 1)
        return (expre_mkastunary(A_SCALE, rtype, tree, rsize));
    }
  }

  // If we get here, the types are not compatible
  return (NULL);
}