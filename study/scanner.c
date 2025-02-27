#include "content.h"
#include "register.h"
#include "token.h"
#include "utils.h"

// Lexical scanning
// Copyright (c) 2019 Warren Toomey, GPL3

// Lexical scanning
// Copyright (c) 2019 Warren Toomey, GPL3

// Return the position of character c
// in string s, or -1 if c not found
static int chrpos(char *s, int c)
{
  char *p;

  p = strchr(s, c);
  return (p ? p - s : -1);
}

static inline void putback(Content *cd, int c)
{
  cd->context->putback = c;
}

// Get the next character from the input file.
static int next(Content *cd)
{
  int c;

  if (cd->context->putback)
  {                           // Use the character put
    c = cd->context->putback; // back if there is one
    cd->context->putback = 0;
    return c;
  }

  c = cd->nextChar(cd->context); // Read from input file
  if ('\n' == c)
    cd->context->line++; // Increment line count
  return c;
}

// Skip past input that we don't need to deal with,
// i.e. whitespace, newlines. Return the first
// character we do need to deal with.
static int skip(Content *cd)
{
  int c;

  c = next(cd);
  while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c)
  {
    c = next(cd);
  }
  return (c);
}

// Scan and return an integer literal
// value from the input file. Store
// the value as a string in Text.
static int scanint(Content *cd, int c)
{
  int k, val = 0;

  // Convert each character into an int value
  while ((k = chrpos("0123456789", c)) >= 0)
  {
    val = val * 10 + k;
    c = next(cd);
  }

  // We hit a non-integer character, put it back.
  //putback(c);
  cd->context->putback = c;
  return val;
}

// Scan an identifier from the input file and
// store it in buf[]. Return the identifier's length
static int scanident(Content *cd, int c, char *buf, int lim)
{
  int i = 0;

  // Allow digits, alpha and underscores
  while (isalpha(c) || isdigit(c) || '_' == c)
  {
    // Error if we hit the identifier length limit,
    // else append to buf[] and get next character
    if (lim - 1 == i)
    {
      printf("identifier too long on line %d\n", cd->context->line);
      exit(1);
    }
    else if (i < lim - 1)
    {
      buf[i++] = c;
    }
    c = next(cd);
  }
  // We hit a non-valid character, put it back.
  // NUL-terminate the buf[] and return the length
  //putback(c);
  cd->context->putback = c;

  buf[i] = '\0';
  return (i);
}
// Return the next character from a character
// or string literal
static int scanch(Content *cd)
{
  int c;

  // Get the next input character and interpret
  // metacharacters that start with a backslash
  c = next(cd);
  if (c == '\\')
  {
    switch (c = next(cd))
    {
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 'f':
      return '\f';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    case '\\':
      return '\\';
    case '"':
      return '"';
    case '\'':
      return '\'';
    default:
      CONTENT_PUBLISH_ERROR(cd, "unknown escape sequence. c = %c", c);
    }
  }
  return (c); // Just an ordinary old character!
}
// Scan in a string literal from the input file,
// and store it in buf[]. Return the length of
// the string.
static int scanstr(Content *cd, char *buf)
{
  int i, c;

  // Loop while we have enough buffer space
  for (i = 0; i < CONTENT_TEXT_BUF_LEN - 1; i++)
  {
    // Get the next char and append to buf
    // Return when we hit the ending double quote
    if ((c = scanch(cd)) == '"')
    {
      buf[i] = 0;
      return (i);
    }
    buf[i] = c;
  }
  // Ran out of buf[] space
  CONTENT_PUBLISH_ERROR(cd, "String literal too long");
  return (0);
}

// Given a word from the input, return the matching
// keyword token number or 0 if it's not a keyword.
// Switch on the first letter so that we don't have
// to waste time strcmp()ing against all the keywords.
static int keyword(char *s)
{
  switch (*s)
  {
  case 'i':
    if (!strcmp(s, "if"))
      return (T_IF);
    if (!strcmp(s, "int"))
      return (T_INT);
    break;

  case 'c':
    if (!strcmp(s, "char"))
      return (T_CHAR);
    break;

  case 'l':
    if (!strcmp(s, "long"))
      return (T_LONG);
    break;

    //-------------------------

  case 'e':
    if (!strcmp(s, "else"))
      return (T_ELSE);
    break;

  case 'w':
    if (!strcmp(s, "while"))
      return (T_WHILE);
    break;

  case 'f':
    if (!strcmp(s, "for"))
      return (T_FOR);
    break;

  case 'v':
    if (!strcmp(s, "void"))
      return (T_VOID);
    break;

  case 'r':
    if (!strcmp(s, "return"))
      return (T_RETURN);
    break;
  }
  return (0);
}

// Reject the token that we just scanned
void scanner_reject_token(Content *cd, struct Token *t)
{
  if (cd->context->rejtoken != NULL)
  {
    fatal(cd, "Can't reject token twice");
  }
  cd->context->rejtoken = t;
}

// Scan and return the next token found in the input.
// Return 1 if token valid, 0 if no tokens left.
// scan keyword string to 'cd->context->textBuf'
int scanner_scan(Content *cd, struct Token *t)
{

  // If we have any rejected token, return it
  if (cd->context->rejtoken != NULL)
  {
    t = cd->context->rejtoken;
    cd->context->rejtoken = NULL;
    return (1);
  }

  int c, tokentype;

  // Skip whitespace
  c = skip(cd);

  // Determine the token based on
  // the input character
  switch (c)
  {
  case EOF:
    //printf("scanner_scan >> EOF \n");
    t->token = T_EOF;
    return (0);

  case '+':
    //printf("scanner_scan >> + \n");
    if ((c = next(cd)) == '+')
    {
      t->token = T_INC;
    }
    else
    {
      putback(cd, c);
      t->token = T_PLUS;
    }
    break;

  case '-':
    //printf("scanner_scan >> - \n");
    if ((c = next(cd)) == '-')
    {
      t->token = T_DEC;
    }
    else
    {
      putback(cd, c);
      t->token = T_MINUS;
    }
    break;
  case '*':
    //printf("scanner_scan >> * \n");
    t->token = T_STAR;
    break;
  case '/':
    //printf("scanner_scan >> / \n");
    t->token = T_SLASH;
    break;

  case ';':
    //printf("scanner_scan >> ; \n");
    t->token = T_SEMI;
    break;

  case '{':
    //printf("scanner_scan >> { \n");
    t->token = T_LBRACE;
    break;
  case '}':
    //printf("scanner_scan >> } \n");
    t->token = T_RBRACE;
    break;

  case '(':
    //printf("scanner_scan >> ( \n");
    t->token = T_LPAREN;
    break;
  case ')':
    //printf("scanner_scan >> ) \n");
    t->token = T_RPAREN;
    break;

  case '[':
    t->token = T_LBRACKET;
    break;

  case ']':
    t->token = T_RBRACKET;
    break;
  case ',':
    t->token = T_COMMA;
    break;

  case '~':
    t->token = T_INVERT;
    break;
  case '^':
    t->token = T_XOR;
    break;

  case '=':
    if ((c = next(cd)) == '=')
    {
      //printf("scanner_scan >> == \n");
      t->token = T_EQ;
    }
    else
    {
      // just =
      putback(cd, c);
      t->token = T_ASSIGN;
    }
    break;

  case '!':
    if ((c = next(cd)) == '=')
    {
      t->token = T_NE;
    }
    else
    {
      putback(cd, c);
      t->token = T_LOGNOT;
    }
    break;

  case '<':
    if ((c = next(cd)) == '=')
    {
      t->token = T_LE;
    }
    else if (c == '<')
    {
      t->token = T_LSHIFT;
    }
    else
    {
      putback(cd, c);
      t->token = T_LT;
    }
    break;

  case '>':
    if ((c = next(cd)) == '=')
    {
      t->token = T_GE;
    }
    else if (c == '>')
    {
      t->token = T_RSHIFT;
    }
    else
    {
      putback(cd, c);
      t->token = T_GT;
    }
    break;

  case '&':
    if ((c = next(cd)) == '&')
    {
      t->token = T_LOGAND;
    }
    else
    {
      putback(cd, c);
      t->token = T_AMPER;
    }
    break;

  case '|':
    if ((c = next(cd)) == '|')
    {
      t->token = T_LOGOR;
    }
    else
    {
      putback(cd, c);
      t->token = T_OR;
    }
    break;

  case '\'':
    //it is a quote, scan in the literal character value and the end quote.
    t->intvalue = scanch(cd);
    t->token = T_INTLIT;
    if (next(cd) != '\'')
    {
      CONTENT_PUBLISH_ERROR(cd, "Expected '\\'' at end of char literal");
    }
    break;

  case '"':
    //scan in a literal string.
    scanstr(cd, cd->context->textBuf);
    t->token = T_STRLIT;
    break;

  default:

    // If it's a digit, scan the
    // literal integer value in
    if (isdigit(c))
    {
      t->intvalue = scanint(cd, c);
      t->token = T_INTLIT;
      // printf("scanner_scan >> %d \n", t->intvalue);
      break;
    }
    else if (isalpha(c) || '_' == c)
    {
      //read keyword
      scanident(cd, c, cd->context->textBuf, CONTENT_TEXT_BUF_LEN);
      // If it's a recognised keyword, return that token
      if (tokentype = keyword(cd->context->textBuf))
      {
        t->token = tokentype;
        break;
      }
      //Not a recognised keyword, so it must be an identifier
      t->token = T_IDENT;
      break;
    }
    CONTENT_PUBLISH_ERROR(cd, "Unrecognised character %c", c);
    //return 0;
  }

  // We found a token
  return (1);
}

void scanner_init(Content *ud)
{
  ud->context->line = 1;
  ud->context->putback = '\n';
}