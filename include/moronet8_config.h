#pragma once

#include <stddef.h>
#include <wchar.h>

/** Major version of ccmoro8. */
#define MORONET8_VERSION_MAJOR 0
/** Minor version of ccmoro8. */
#define MORONET8_VERSION_MINOR 0
/** Patch version of ccmoro8. */
#define MORONET8_VERSION_PATCH 1

/* Define to 1 if you have the <stdio.h> header file. */
#ifndef HAVE_STDIO_H
/* #undef HAVE_STDIO_H */
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef HAVE_STDINT_H
#define HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef HAVE_STRING_H
#define HAVE_STRING_H 1
#endif

/* Define to 1 if you have the <stdarg.h> header file. */
#ifndef HAVE_STDARG_H
#define HAVE_STDARG_H 1
#endif

/* Define to 1 if you have the <iostream.h> header file. */
#ifndef HAVE_IOSTREAM_H
#define HAVE_IOSTREAM_H 1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef HAVE_MEMORY_H
#define HAVE_MEMORY_H 1
#endif

/* Define to 1 if you have the <math.h> header file. */
#ifndef HAVE_MATH_H
#define HAVE_MATH_H 1
#endif

/* Define to 1 if you have the `free' function. */
#ifndef HAVE_FREE
/* #undef HAVE_FREE */
#endif

/* Define to 1 if you have the `malloc' function. */
#ifndef HAVE_MALLOC
/* #undef HAVE_MALLOC */
#endif

#ifndef MORONET8_DOXYGEN
/* Builds Doxygen doc. */
/* #undef MORONET8_DOXYGEN */
#endif

#ifndef MORONET8_MINIMALIST
/**
 * Strips extra features from moro8.
 *
 * This implicitly sets MORONET8_WITH_PARSER=0 and MORONET8_WITH_HANDLERS=0.
 */
/* #undef MORONET8_MINIMALIST */
#endif

#ifndef MORONET8_EXTENDED_OPCODES
/**
 * Includes extended opcodes specific to moro8.
 */
/* #undef MORONET8_EXTENDED_OPCODES */
#endif

#ifndef MORONET8_WITH_PARSER
/**
 * Builds moro8 with support for parser.
 *
 * @note
 * Parser is not available when MORONET8_MINIMALIST is defined.
 *
 */
/* #undef MORONET8_WITH_PARSER */
#endif

#ifndef MORONET8_WITH_HANDLERS
/**
 * Builds moro8 with support for custom opcodes.
 *
 * @note
 * Handlers are not available when MORONET8_MINIMALIST is defined.
 *
 */
/* #undef MORONET8_WITH_HANDLERS */
#endif

#ifndef MORONET8_LUA_API
/* Builds with Lua APi. */
#define MORONET8_LUA_API 1
#endif

#ifndef MORONET8_JS_API
/* Builds with JS APi. */
#define MORONET8_JS_API 1
#endif

#ifndef MORONET8_FILESYSTEM
/* Builds with support for filesystem. */
/* #undef MORONET8_FILESYSTEM */
#endif

#ifndef MORONET8_MALLOC
#ifdef HAVE_MALLOC
/**
 * Defines the malloc function used by moro8 at compile time.
 *
 * @code
 * void* my_malloc(size_t size)
 * {
 *     // do something
 * }
 *
 * #define MORONET8_MALLOC my_malloc
 * @endcode
 */
#define MORONET8_MALLOC malloc
#else
#define MORONET8_MALLOC(size) NULL
#endif
#endif

#ifndef MORONET8_FREE
#ifdef HAVE_FREE
/**
 * Defines the free function used by moro8 at compile time.
 *
 * @code
 * void my_free(void* ptr)
 * {
 *     // do something
 * }
 *
 * #define MORONET8_FREE my_free
 * @endcode
 */
#define MORONET8_FREE free
#else
#define MORONET8_FREE(ptr)
#endif
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#define MORONET8_CDECL __cdecl
#define MORONET8_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(MORONET8_HIDE_SYMBOLS) && !defined(MORONET8_IMPORT_SYMBOLS) && !defined(MORONET8_EXPORT_SYMBOLS)
#define MORONET8_EXPORT_SYMBOLS
#endif

#if defined(MORONET8_HIDE_SYMBOLS)
#define MORONET8_PUBLIC(type) type MORONET8_STDCALL
#elif defined(MORONET8_EXPORT_SYMBOLS)
#define MORONET8_PUBLIC(type) __declspec(dllexport) type MORONET8_STDCALL
#elif defined(MORONET8_IMPORT_SYMBOLS)
#define MORONET8_PUBLIC(type) __declspec(dllimport) type MORONET8_STDCALL
#endif
#else /* !__WINDOWS__ */
#define MORONET8_CDECL
#define MORONET8_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define MORONET8_PUBLIC(type) __attribute__((visibility("default"))) type
#else
#define MORONET8_PUBLIC(type) type
#endif
#endif
