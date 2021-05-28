#include "content.h"
#include "token.h"

CPP_START

void scanner_init(Content *ud);

int scanner_scan(Content *ud, struct Token *t);

// Reject the token that we just scanned
void scanner_reject_token(Content *cd, struct Token *t);

CPP_END