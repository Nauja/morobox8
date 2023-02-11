#pragma once

#include "morobox8_config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#define MOROBOX8_TRUE 1
#define MOROBOX8_FALSE 0
#define MOROBOX8_COMMA ,
#define MOROBOX8_VERBOSE 1
#define MOROBOX8_EXTRA_VERBOSE 2

#ifndef min
#define min(a, b) (a < b ? a : b)
#endif
#ifndef max
#define max(a, b) (a > b ? a : b)
#endif

#define MOROBOX8_RETFALSE_DEFAULT(val) return MOROBOX8_FALSE;
#define MOROBOX8_RETZERO_DEFAULT(val) return 0;
#define MOROBOX8_RETNULL_DEFAULT(val) return NULL;
#define MOROBOX8_RETVOID_DEFAULT(val) return;
#define MOROBOX8_RETFALSE_VALUE(val) return val;
#define MOROBOX8_RETZERO_VALUE(val) return val;
#define MOROBOX8_RETNULL_VALUE(val) return val;
#define MOROBOX8_RETVOID_VALUE(val) return;
#define MOROBOX8_PERCENT_INT(val, total) ((int)(((val) / (float)(total)) * 100))

#define MOROBOX8_CREATE_H(type)    \
    MOROBOX8_PUBLIC(struct type *) \
    type##_create(void);           \
    MOROBOX8_PUBLIC(void)          \
    type##_init(struct type *);    \
    MOROBOX8_PUBLIC(void)          \
    type##_delete(struct type *);

#define MOROBOX8_CREATE_C(type)                                               \
    MOROBOX8_PUBLIC(struct type *)                                            \
    type##_create(void)                                                       \
    {                                                                         \
        struct type *o = (struct type *)morobox8_malloc(sizeof(struct type)); \
        assert(o);                                                            \
        if (o)                                                                \
        {                                                                     \
            type##_init(o);                                                   \
        }                                                                     \
                                                                              \
        return o;                                                             \
    }

#define MOROBOX8_INIT_C(type)              \
    MOROBOX8_PUBLIC(void)                  \
    type##_init(struct type *o)            \
    {                                      \
        assert(o);                         \
        memset(o, 0, sizeof(struct type)); \
    }

#define MOROBOX8_DELETE_C(type)   \
    MOROBOX8_PUBLIC(void)         \
    type##_delete(struct type *o) \
    {                             \
        morobox8_free(o);         \
    }

#define MOROBOX8_CID_C(type) \
    MOROBOX8_INIT_C(type)    \
    MOROBOX8_CREATE_C(type)  \
    MOROBOX8_DELETE_C(type)
