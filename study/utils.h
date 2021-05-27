#ifndef _UTILS_H
#define _UTILS_H
#include "common_headers.h"

CPP_START

struct _Content;

void fatal(struct _Content* c,char *s);

void fatals(struct _Content* c,char *s1, char *s2);

void fatald(struct _Content* c,char *s, int d);

void fatalc(struct _Content* c,char *s, int ch);

CPP_END

#endif