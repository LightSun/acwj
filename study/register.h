#include "common_headers.h"
#ifndef _REGISTER_H
#define _REGISTER_H
#include "writer.h"
CPP_START

#define REGISTER_CONTEXT_PARAM Writer *w

// Print out the assembly preamble
void register_cgpreamble(REGISTER_CONTEXT_PARAM);
void register_cgpostamble(REGISTER_CONTEXT_PARAM);

int register_cgload(REGISTER_CONTEXT_PARAM, int value);
int register_cgadd(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cgsub(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cgmul(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cgdiv(REGISTER_CONTEXT_PARAM, int r1, int r2);

void register_cgprintint(REGISTER_CONTEXT_PARAM, int r);

void register_free_all();

CPP_END

#endif