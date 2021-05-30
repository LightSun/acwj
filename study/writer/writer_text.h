
#ifndef _WRITER_TEXT_H
#define _WRITER_TEXT_H
#include "../common_headers.h"
#include "../writer_pri.h"
CPP_START

int writer_text_start(WriterContext *context,int initSize);

int writer_text_writeChars(WriterContext *context,const char *content);

//void writer_text_writeFmt(WriterContext *context,const char *format,...);

void writer_text_end(WriterContext *context);
CPP_END
#endif