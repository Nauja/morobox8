include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckSymbolExists)

# HEADER FILES
check_include_file(stdio.h HAVE_STDIO_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(stdarg.h HAVE_STDARG_H)
check_include_file(assert.h HAVE_ASSERT_H)
check_include_file(iostream.h HAVE_IOSTREAM_H)
check_include_file(math.h HAVE_MATH_H)
check_include_file(memory.h HAVE_MEMORY_H)

# FUNCTIONS
check_function_exists(free HAVE_FREE)
check_function_exists(malloc HAVE_MALLOC)