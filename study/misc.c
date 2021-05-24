#include "content.h"
#include "misc.h"

// Ensure that the current token is t,
// and fetch the next token. Otherwise
// throw an error 
void misc_match(Content* cd,struct Token* token,int t, char *what) {
  if (token->token == t) {
    scanner_scan(cd, token);
  } else {
    printf("%s expected on line %d\n", what, cd->context.line);
    exit(1);
  }
}

// Match a semicon and fetch the next token
void misc_semi(Content* cd,struct Token* token) {
  misc_match(cd, token, T_SEMI, ";");
}