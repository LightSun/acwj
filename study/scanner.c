#include "content_delegate.h"
#include "token.h"

int _line;
int _putback;

// Put back an unwanted character
static void scanner_putback_set(int ch){
     _putback = ch;
 }
 
/*  int scanner_putback_get(){
     return _putback;
 }
 */

// Lexical scanning
// Copyright (c) 2019 Warren Toomey, GPL3

// Lexical scanning
// Copyright (c) 2019 Warren Toomey, GPL3

// Return the position of character c
// in string s, or -1 if c not found
static int chrpos(char *s, int c) {
  char *p;

  p = strchr(s, c);
  return (p ? p - s : -1);
}

// Get the next character from the input file.
static int next(ContentDelegate* cd) {
  int c;

  if (_putback) {		// Use the character put
    c = _putback;		// back if there is one
    _putback = 0;
    return c;
  }

  c = cd->nextChar(cd->param);		// Read from input file
  if ('\n' == c)
    _line++;			// Increment line count
  return c;
}

// Skip past input that we don't need to deal with, 
// i.e. whitespace, newlines. Return the first
// character we do need to deal with.
static int skip(ContentDelegate* ud) {
  int c;

  c = next(ud);
  while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c) {
    c = next(ud);
  }
  return (c);
}

// Scan and return an integer literal
// value from the input file. Store
// the value as a string in Text.
static int scanint(ContentDelegate* ud, int c) {
  int k, val = 0;

  // Convert each character into an int value
  while ((k = chrpos("0123456789", c)) >= 0) {
    val = val * 10 + k;
    c = next(ud);
  }

  // We hit a non-integer character, put it back.
  //putback(c);
  _putback = c;
  return val;
}


// Scan and return the next token found in the input.
// Return 1 if token valid, 0 if no tokens left.
int scanner_scan(ContentDelegate* ud,struct Token *t) {
    
  int c;

  // Skip whitespace
  c = skip(ud);

  // Determine the token based on
  // the input character
  switch (c) {
  case EOF:
    printf("scanner_scan >> EOF \n");
    t->token = T_EOF;
    return (0);
  case '+':
    printf("scanner_scan >> + \n");
    t->token = T_PLUS;
    break;
  case '-':
    printf("scanner_scan >> - \n");
    t->token = T_MINUS;
    break;
  case '*':
    printf("scanner_scan >> * \n");
    t->token = T_STAR;
    break;
  case '/':
    printf("scanner_scan >> / \n");
    t->token = T_SLASH;
    break;
  default:

    // If it's a digit, scan the
    // literal integer value in
    if (isdigit(c)) {
      t->intvalue = scanint(ud, c);
      t->token = T_INTLIT;
      printf("scanner_scan >> %d \n", t->intvalue);
      break;
    }

    printf("Unrecognised character %c on line %d\n", c, _line);
    //exit(1);
    return 0;
  }

  // We found a token
  return (1);
}


void scanner_init(){
    _line = 1;
    _putback = '\n';
}

int scanner_line_get(){
  return _line;
}