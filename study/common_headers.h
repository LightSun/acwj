#ifndef _COMMON_HEADER_H
#define _COMMON_HEADER_H

#ifdef cplusplus
#define CPP_START extern "C" { 
#else 
#define CPP_START
#endif

#ifdef cplusplus
#define CPP_END }
#else 
#define CPP_END
#endif

CPP_START
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
CPP_END

#endif