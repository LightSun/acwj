#include "content_delegate.h"
#include "token.h"

CPP_START

int scanner_scan(ContentDelegate *ud, struct Token *t);

void scanner_init();

int scanner_line_get();

CPP_END