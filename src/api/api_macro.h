#pragma once

#include "morobox8.h"

// clang-format off
#define MOROBOX8_API_CONST_LIST(macro) \
    macro(                                        \
        WIDTH,                          \
        MOROBOX8_SCREEN_WIDTH, \
        MOROBOX8_API_GAME,                                                                         \
        "")                          \
    macro(                                        \
        HEIGHT,                          \
        MOROBOX8_SCREEN_HEIGHT, \
        MOROBOX8_API_GAME,                                                                         \
        "")                          \
        \
    macro(                                        \
        LEFT,                          \
        MOROBOX8_BUTTON_LEFT, \
        MOROBOX8_API_GAME,                                                                         \
        "")                          \
        \
    macro(                                        \
        RIGHT,                          \
        MOROBOX8_BUTTON_RIGHT, \
        MOROBOX8_API_GAME,                                                                         \
        "")                      \
        \
    macro(                                        \
        UP,                          \
        MOROBOX8_BUTTON_UP, \
        MOROBOX8_API_GAME,                                                                         \
        "")                          \
        \
    macro(                                        \
        DOWN,                          \
        MOROBOX8_BUTTON_DOWN, \
        MOROBOX8_API_GAME,                                                                         \
        "")                         \
        \
    macro(                                        \
        A,                          \
        MOROBOX8_BUTTON_A, \
        MOROBOX8_API_GAME,                                                                         \
        "")                         \
        \
    macro(                                        \
        START,                          \
        MOROBOX8_BUTTON_START, \
        MOROBOX8_API_GAME,                                                                         \
        "")                         \
        \
    macro(                                        \
        BIOS,                          \
        MOROBOX8_STATE_BIOS, \
        MOROBOX8_API_BIOS,                                                                         \
        "")                         \
        \
    macro(                                        \
        CART,                          \
        MOROBOX8_STATE_GAME, \
        MOROBOX8_API_BIOS,                                                                         \
        "")                         \
        \
    macro(                                        \
        OVERLAY,                          \
        MOROBOX8_STATE_OVERLAY, \
        MOROBOX8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_CLOSED,                          \
        MOROBOX8_SESSION_CLOSED, \
        MOROBOX8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_CREATING,                          \
        MOROBOX8_SESSION_CREATING, \
        MOROBOX8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_HOSTING,                          \
        MOROBOX8_SESSION_HOSTING, \
        MOROBOX8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_JOINING,                          \
        MOROBOX8_SESSION_JOINING, \
        MOROBOX8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_JOINED,                          \
        MOROBOX8_SESSION_JOINED, \
        MOROBOX8_API_BIOS,                                                                         \
        "")                        \
        \
    macro(                                        \
        SESSION_LEAVING,                          \
        MOROBOX8_SESSION_LEAVING, \
        MOROBOX8_API_BIOS,                                                                         \
        "")
 
#define MOROBOX8_API_FUN_LIST(macro)                                                             \
                                                                                        \
    macro(                                                                                   \
        font,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                       \
        print,                                                                                   \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        btn,                                                                                   \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        btnp,                                                                                   \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        dt,                                                                                   \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        sin,                                                                                   \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                       \
        cos,                                                                                   \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                      \
                                                                                                \
    macro(                                                                                   \
        cls,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                                \
    macro(                                                                                   \
        color,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                                \
    macro(                                                                                   \
        line,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                                \
    macro(                                                                                   \
        rect,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                                \
    macro(                                                                                   \
        rectfill,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                   \
        tileset,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                   \
        spr,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                   \
        palt,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                                   \
        code,                                                                                 \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                                  \
                                                                                        \
    macro(                                                                               \
        peek,                                                                            \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                              \
                                                                                        \
    macro(                                                                           \
        poke,                                                                        \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                          \
                                                                                    \
    macro(                                                                       \
        pset,                                                                    \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                      \
                                                                                \
    macro(                                                                   \
        pget,                                                                \
        MOROBOX8_API_GAME,                                                                         \
        "")                                                                  \
                                                                            \
    macro(                                                           \
        nethost,                                                       \
        MOROBOX8_API_GAME,                                                                         \
        "check is host")                             \
                                                                            \
    macro(                                                           \
        netclient,                                                       \
        MOROBOX8_API_GAME,                                                                         \
        "check is client")                             \
                                                                            \
    macro(                                                           \
        netsp,                                                       \
        MOROBOX8_API_GAME,                                                                         \
        "stack pointer of net stack")                             \
                                                                    \
    macro(                                                       \
        netpush,                                                   \
        MOROBOX8_API_GAME,                                                                         \
        "push word on net stack")                                \
                                                        \
    macro(                                           \
        netpop,                                        \
        MOROBOX8_API_GAME,                                                                         \
        "pop word from net stack")                     \
                                                        \
    macro(                               \
        pktsp,                             \
        MOROBOX8_API_GAME,                                                                         \
        "stack pointer of packet stack")       \
                                                        \
    macro(                               \
        pktpush,                             \
        MOROBOX8_API_GAME,                                                                         \
        "push word on packet stack")       \
                                        \
    macro(                               \
        pktpop,                             \
        MOROBOX8_API_GAME,                                                                         \
        "pop word from packet stack")       \
                                                        \
    macro(                           \
        pktsend,                      \
        MOROBOX8_API_GAME,                                                                         \
        "send packet")       \
                                                        \
    macro(                           \
        state,                      \
        MOROBOX8_API_BIOS,                                                                         \
        "console state")\
                                                              \
    macro(                           \
        reset,                      \
        MOROBOX8_API_BIOS,                                                                         \
        "reset the vm")\
                                                           \
    macro(                           \
        load,                      \
        MOROBOX8_API_BIOS,                                                                         \
        "load cart")\
                                                        \
    macro(                           \
        netsessionstate,                      \
        MOROBOX8_API_BIOS,                                                                         \
        "get session state")\
                                                        \
    macro(                           \
        netsessionstart,                      \
        MOROBOX8_API_BIOS,                                                                         \
        "start a networked session")\
                                                        \
    macro(                           \
        netsessionjoin,                      \
        MOROBOX8_API_BIOS,                                                                         \
        "join a networked session")\
                                                        \
    macro(                           \
        netsessionleave,                      \
        MOROBOX8_API_BIOS,                                                                         \
        "leave the networked session")
// clang-format on
