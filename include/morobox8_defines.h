#pragma once

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#define MOROBOX8_CREATE_H(type)    \
    MOROBOX8_PUBLIC(struct type *) \
    type##_create(void);           \
    MOROBOX8_PUBLIC(struct type *) \
    type##_init(struct type *);    \
    MOROBOX8_PUBLIC(void)          \
    type##_delete(struct type *);

#define MOROBOX8_CREATE_C(type)                                               \
    MOROBOX8_PUBLIC(struct type *)                                            \
    type##_create(void)                                                       \
    {                                                                         \
        struct type *o = (struct type *)MOROBOX8_MALLOC(sizeof(struct type)); \
        if (o)                                                                \
        {                                                                     \
            type##_init(o);                                                   \
        }                                                                     \
                                                                              \
        return o;                                                             \
    }

#define MOROBOX8_INIT_C(type)              \
    MOROBOX8_PUBLIC(struct type *)         \
    type##_init(struct type *o)            \
    {                                      \
        memset(o, 0, sizeof(struct type)); \
        return o;                          \
    }

#define MOROBOX8_DELETE_C(type)   \
    MOROBOX8_PUBLIC(void)         \
    type##_delete(struct type *o) \
    {                             \
        MOROBOX8_FREE(o);         \
    }

#define MOROBOX8_CID_C(type) \
    MOROBOX8_CREATE_C(type)  \
    MOROBOX8_INIT_C(type)    \
    MOROBOX8_DELETE_C(type)
