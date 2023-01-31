#pragma once

#include <stddef.h>
#include <wchar.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/** Major version of ccmoro8. */
#define MOROBOX8_VERSION_MAJOR 0
/** Minor version of ccmoro8. */
#define MOROBOX8_VERSION_MINOR 0
/** Patch version of ccmoro8. */
#define MOROBOX8_VERSION_PATCH 1

/* Define to 1 if you have the <stdio.h> header file. */
#ifndef HAVE_STDIO_H
#define HAVE_STDIO_H 1
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
/* #undef HAVE_IOSTREAM_H */
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
#define HAVE_FREE 1
#endif

/* Define to 1 if you have the `malloc' function. */
#ifndef HAVE_MALLOC
#define HAVE_MALLOC 1
#endif

#ifndef MOROBOX8_DOXYGEN
/* Builds Doxygen doc. */
/* #undef MOROBOX8_DOXYGEN */
#endif

#ifndef MOROBOX8_MINIMALIST
/**
 * Strips extra features from moro8.
 *
 * This implicitly sets MOROBOX8_WITH_PARSER=0 and MOROBOX8_WITH_HANDLERS=0.
 */
/* #undef MOROBOX8_MINIMALIST */
#endif

#ifndef MOROBOX8_EXTENDED_OPCODES
/**
 * Includes extended opcodes specific to moro8.
 */
/* #undef MOROBOX8_EXTENDED_OPCODES */
#endif

#ifndef MOROBOX8_WITH_PARSER
/**
 * Builds moro8 with support for parser.
 *
 * @note
 * Parser is not available when MOROBOX8_MINIMALIST is defined.
 *
 */
/* #undef MOROBOX8_WITH_PARSER */
#endif

#ifndef MOROBOX8_WITH_HANDLERS
/**
 * Builds moro8 with support for custom opcodes.
 *
 * @note
 * Handlers are not available when MOROBOX8_MINIMALIST is defined.
 *
 */
/* #undef MOROBOX8_WITH_HANDLERS */
#endif

#ifndef MOROBOX8_LUA_API
/* Builds with Lua APi. */
#define MOROBOX8_LUA_API 1
#endif

#ifndef MOROBOX8_JS_API
/* Builds with JS APi. */
#define MOROBOX8_JS_API 1
#endif

#ifndef MOROBOX8_FILESYSTEM
/* Builds with support for filesystem. */
#define MOROBOX8_FILESYSTEM 1
#endif

#ifndef MOROBOX8_WEBSOCKETS
#define MOROBOX8_WEBSOCKETS 1
#endif

#ifndef MOROBOX8_MALLOC
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
 * #define MOROBOX8_MALLOC my_malloc
 * @endcode
 */
#define MOROBOX8_MALLOC malloc
#else
#define MOROBOX8_MALLOC(size) NULL
#endif
#endif

#ifndef MOROBOX8_FREE
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
 * #define MOROBOX8_FREE my_free
 * @endcode
 */
#define MOROBOX8_FREE free
#else
#define MOROBOX8_FREE(ptr)
#endif
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#if defined(__EMSCRIPTEN__)
#define MOROBOX8_CDECL
#define MOROBOX8_STDCALL
#define MOROBOX8_PUBLIC(type) type EMSCRIPTEN_KEEPALIVE
#elif defined(__WINDOWS__)
#define MOROBOX8_CDECL __cdecl
#define MOROBOX8_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(MOROBOX8_HIDE_SYMBOLS) && !defined(MOROBOX8_IMPORT_SYMBOLS) && !defined(MOROBOX8_EXPORT_SYMBOLS)
#define MOROBOX8_EXPORT_SYMBOLS
#endif

#if defined(MOROBOX8_HIDE_SYMBOLS)
#define MOROBOX8_PUBLIC(type) type MOROBOX8_STDCALL
#elif defined(MOROBOX8_EXPORT_SYMBOLS)
#define MOROBOX8_PUBLIC(type) __declspec(dllexport) type MOROBOX8_STDCALL
#elif defined(MOROBOX8_IMPORT_SYMBOLS)
#define MOROBOX8_PUBLIC(type) __declspec(dllimport) type MOROBOX8_STDCALL
#endif
#else /* !__WINDOWS__ */
#define MOROBOX8_CDECL
#define MOROBOX8_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define MOROBOX8_PUBLIC(type) __attribute__((visibility("default"))) type
#else
#define MOROBOX8_PUBLIC(type) type
#endif
#endif
