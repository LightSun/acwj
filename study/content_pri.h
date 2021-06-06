
#ifndef _CONTENT_PRI_H
#define _CONTENT_PRI_H
#include "common_headers.h"

CPP_START

#define CONTENT_TEXT_BUF_LEN 512                  // max Length of symbols in input
//extern_ char Text[TEXTLEN + 1];                 // Last identifier scanned
struct Token;
struct GlobalState;
struct _Register;

typedef struct
{
   void *param;
   int type;
   //temp
   FILE *infile; //
   int pos;
   //used for scanner
   int line;                               // the line number
   int putback;                            // the last char
   char textBuf[CONTENT_TEXT_BUF_LEN + 1]; // Last identifier scanned

   struct Token *rejtoken; // A pointer to a rejected token
   int functionid;         // Symbol id of the current function

   int dumpAST;  // if need dump ast
   int dumpId;   // dump label id

   struct GlobalState *globalState; // the global state. help do
   struct _Register * _register;
} ContentContext;

#define CONTENT_PARAM ContentContext* ctx

CPP_END

#endif