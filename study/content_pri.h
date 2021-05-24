

#include "common_headers.h"

#define CONTENT_TEXT_BUF_LEN         512          // max Length of symbols in input
//extern_ char Text[TEXTLEN + 1];                 // Last identifier scanned

typedef struct{
    void *param;
    int type;
 //temp   
    FILE *infile; //
    int pos;
 //used for scanner
    int line;    // the line number
    int putback; // the last char
    char textBuf[CONTENT_TEXT_BUF_LEN + 1];       // Last identifier scanned
} ContentContext;