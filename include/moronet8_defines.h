#pragma once

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#define MORONET8_CREATE_H(type)    \
    MORONET8_PUBLIC(struct type *) \
    type##_create(void);           \
    MORONET8_PUBLIC(struct type *) \
    type##_init(struct type *);    \
    MORONET8_PUBLIC(void)          \
    type##_delete(struct type *);

#define MORONET8_CREATE_C(type)                                        \
    MORONET8_PUBLIC(struct type *)                                     \
    type##_create(void)                                                \
    {                                                                  \
        struct type *o = (struct type *)MORONET8_MALLOC(sizeof(type)); \
        if (o)                                                         \
        {                                                              \
            type##_init(o);                                            \
        }                                                              \
                                                                       \
        return o;                                                      \
    }

#define MORONET8_INIT_C(type)              \
    MORONET8_PUBLIC(struct type *)         \
    type##_init(struct type *o)            \
    {                                      \
        memset(o, 0, sizeof(struct type)); \
        return o;                          \
    }

#define MORONET8_DELETE_C(type)   \
    MORONET8_PUBLIC(void)         \
    type##_delete(struct type *o) \
    {                             \
        MORONET8_FREE(o);         \
    }

#define MORONET8_CID_C(type) \
    MORONET8_CREATE_C(type)  \
    MORONET8_INIT_C(type)    \
    MORONET8_DELETE_C(type)
