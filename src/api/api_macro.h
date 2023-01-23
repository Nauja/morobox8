#pragma once

#include "moronet8.h"

// clang-format off
#define MORONET8_API_CONST_LIST(macro) \
    macro(                                        \
        WIDTH,                          \
        MORONET8_SCREEN_WIDTH, \
        MORONET8_API_CART,                                                                         \
        "")                          \
    macro(                                        \
        HEIGHT,                          \
        MORONET8_SCREEN_HEIGHT, \
        MORONET8_API_CART,                                                                         \
        "")                          \
        \
    macro(                                        \
        LEFT,                          \
        MORONET8_BUTTON_LEFT, \
        MORONET8_API_CART,                                                                         \
        "")                          \
        \
    macro(                                        \
        RIGHT,                          \
        MORONET8_BUTTON_RIGHT, \
        MORONET8_API_CART,                                                                         \
        "")                      \
        \
    macro(                                        \
        UP,                          \
        MORONET8_BUTTON_UP, \
        MORONET8_API_CART,                                                                         \
        "")                          \
        \
    macro(                                        \
        DOWN,                          \
        MORONET8_BUTTON_DOWN, \
        MORONET8_API_CART,                                                                         \
        "")                         \
        \
    macro(                                        \
        A,                          \
        MORONET8_BUTTON_A, \
        MORONET8_API_CART,                                                                         \
        "")                         \
        \
    macro(                                        \
        START,                          \
        MORONET8_BUTTON_START, \
        MORONET8_API_CART,                                                                         \
        "")                         \
        \
    macro(                                        \
        BIOS,                          \
        MORONET8_STATE_BIOS, \
        MORONET8_API_BIOS,                                                                         \
        "")                         \
        \
    macro(                                        \
        CART,                          \
        MORONET8_STATE_CART, \
        MORONET8_API_BIOS,                                                                         \
        "")                         \
        \
    macro(                                        \
        OVERLAY,                          \
        MORONET8_STATE_OVERLAY, \
        MORONET8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_CLOSED,                          \
        MORONET8_SESSION_CLOSED, \
        MORONET8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_CREATING,                          \
        MORONET8_SESSION_CREATING, \
        MORONET8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_HOSTING,                          \
        MORONET8_SESSION_HOSTING, \
        MORONET8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_JOINING,                          \
        MORONET8_SESSION_JOINING, \
        MORONET8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_JOINED,                          \
        MORONET8_SESSION_JOINED, \
        MORONET8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_LEAVING,                          \
        MORONET8_SESSION_LEAVING, \
        MORONET8_API_BIOS,                                                                         \
        "")
 
#define MORONET8_API_FUN_LIST(macro)                                                             \
                                                                                        \
    macro(                                                                                   \
        font,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                       \
        print,                                                                                   \
        MORONET8_API_CART,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        btn,                                                                                   \
        MORONET8_API_CART,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        btnp,                                                                                   \
        MORONET8_API_CART,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        dt,                                                                                   \
        MORONET8_API_CART,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        sin,                                                                                   \
        MORONET8_API_CART,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        cos,                                                                                   \
        MORONET8_API_CART,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                   \
        cls,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                                \
    macro(                                                                                   \
        color,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                                \
    macro(                                                                                   \
        line,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                                \
    macro(                                                                                   \
        rect,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                                \
    macro(                                                                                   \
        rectfill,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                   \
        tileset,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                   \
        spr,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                   \
        palt,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                   \
        code,                                                                                 \
        MORONET8_API_CART,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                               \
        peek,                                                                            \
        MORONET8_API_CART,                                                                         \
        "")                                                                              \
                                                                                        \
    macro(                                                                           \
        poke,                                                                        \
        MORONET8_API_CART,                                                                         \
        "")                                                                          \
                                                                                    \
    macro(                                                                       \
        pset,                                                                    \
        MORONET8_API_CART,                                                                         \
        "")                                                                      \
                                                                                \
    macro(                                                                   \
        pget,                                                                \
        MORONET8_API_CART,                                                                         \
        "")                                                                  \
                                                                            \
    macro(                                                           \
        nethost,                                                       \
        MORONET8_API_CART,                                                                         \
        "check is host")                             \
                                                                            \
    macro(                                                           \
        netclient,                                                       \
        MORONET8_API_CART,                                                                         \
        "check is client")                             \
                                                                            \
    macro(                                                           \
        netsp,                                                       \
        MORONET8_API_CART,                                                                         \
        "stack pointer of net stack")                             \
                                                                    \
    macro(                                                       \
        netpush,                                                   \
        MORONET8_API_CART,                                                                         \
        "push word on net stack")                                \
                                                        \
    macro(                                           \
        netpop,                                        \
        MORONET8_API_CART,                                                                         \
        "pop word from net stack")                     \
                                                        \
    macro(                               \
        pktsp,                             \
        MORONET8_API_CART,                                                                         \
        "stack pointer of packet stack")       \
                                                        \
    macro(                               \
        pktpush,                             \
        MORONET8_API_CART,                                                                         \
        "push word on packet stack")       \
                                        \
    macro(                               \
        pktpop,                             \
        MORONET8_API_CART,                                                                         \
        "pop word from packet stack")       \
                                                        \
    macro(                           \
        pktsend,                      \
        MORONET8_API_CART,                                                                         \
        "send packet")       \
                                                        \
    macro(                           \
        state,                      \
        MORONET8_API_BIOS,                                                                         \
        "console state")\
                                                        \
    macro(                           \
        load,                      \
        MORONET8_API_BIOS,                                                                         \
        "load a cart")\
                                                        \
    macro(                           \
        netsessionstate,                      \
        MORONET8_API_BIOS,                                                                         \
        "get session state")\
                                                        \
    macro(                           \
        netsessionstart,                      \
        MORONET8_API_BIOS,                                                                         \
        "start a networked session")\
                                                        \
    macro(                           \
        netsessionjoin,                      \
        MORONET8_API_BIOS,                                                                         \
        "join a networked session")\
                                                        \
    macro(                           \
        netsessionleave,                      \
        MORONET8_API_BIOS,                                                                         \
        "leave the networked session")
// clang-format on
