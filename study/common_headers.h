#ifndef _COMMON_HEADER_H
#define _COMMON_HEADER_H

#if _COMPILER_MSVC
// Workaround MSVS bug...
#	define _VA_ARGS_PASS(...) _VA_ARGS_PASS_1_ __VA_ARGS__ _VA_ARGS_PASS_2_
#	define _VA_ARGS_PASS_1_ (
#	define _VA_ARGS_PASS_2_ )
#else
#	define _VA_ARGS_PASS(...) (__VA_ARGS__)
#endif

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