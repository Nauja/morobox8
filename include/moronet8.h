#pragma once

#include "moronet8_config.h"
#include "moronet8_types.h"
#include "moronet8_limits.h"
#include "cart.h"
#include "api/api_type.h"
#include "network/session_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MORONET8_NAME "moronet8"

    typedef moronet8_u8 moronet8_netram_sp;
    typedef moronet8_u8 moronet8_pktram_sp;

#define MORONET8_UNUSED(x) (void)(x)

    struct moronet8_socket;
    struct moronet8_session;

    /* Struct for custom hooks configuration. */
    struct moronet8_hooks
    {
        /* Custom malloc function. */
        void *(*malloc_fn)(size_t size);
        /* Custom free function. */
        void (*free_fn)(void *ptr);
        /* Custom printf function. */
        void (*printf_fn)(const char *fmt, va_list args);
        struct moronet8_session *(*host_session_fn)(const char *host);
        struct moronet8_session *(*join_session_fn)(const char *host);
        void (*delete_session_fn)(struct moronet8_session *session);
        void (*broadcast_session_fn)(struct moronet8_session *session, const void *buf, size_t size);
        size_t (*receive_session_fn)(struct moronet8_session *session, void *buf, size_t size);
        enum moronet8_session_state (*session_state_get_fn)(struct moronet8_session *session);
        void (*poll_session_fn)(struct moronet8_session *session);
    };

    struct moronet8_api
    {
        /* Type of API. */
        enum moronet8_api_type type;
        /* State of the API. */
        void *state;
        /* Delete the API. */
        void (*free)(struct moronet8_api *api);
        /* Load code from buffer. */
        struct moronet8_api *(*load_string)(struct moronet8_api *api, const char *buf, size_t size);
        /* Tick the code. */
        struct moronet8_api *(*tick)(struct moronet8_api *api);
        /* Notify cart is loading. */
        void (*on_cart_loading)(struct moronet8_api *api);
        /* Notify cart is loaded. */
        void (*on_cart_loaded)(struct moronet8_api *api);
    };

    enum moronet8_button
    {
        MORONET8_BUTTON_LEFT = 0,
        MORONET8_BUTTON_RIGHT,
        MORONET8_BUTTON_UP,
        MORONET8_BUTTON_DOWN,
        MORONET8_BUTTON_A,
        MORONET8_BUTTON_START,
    };

    enum moronet8_state
    {
        /* Displaying system menus. */
        MORONET8_STATE_BIOS = 0,
        /* Displaying game. */
        MORONET8_STATE_CART,
        /* Displaying system overlay over game. */
        MORONET8_STATE_OVERLAY
    };

    struct moronet8
    {
        struct
        {
            /* Pointer to vram. */
            void *vram;
            /* Networked ram. */
            moronet8_u8 netram[MORONET8_NETRAM_SIZE];
            /* Stack pointer for netram. */
            moronet8_netram_sp netram_sp;
            /* Packet ram. */
            moronet8_u8 pktram[MORONET8_PKTRAM_SIZE];
            /* Stack pointer for pktram. */
            moronet8_pktram_sp pktram_sp;
            /* Draw color. */
            moronet8_u8 color;
            /* Delta time. */
            float dt;
            struct
            {
                int left : 1;
                int right : 1;
                int up : 1;
                int down : 1;
                int a : 1;
                int start : 1;
                int reserved : 2;
            } buttons[2];
        } ram;
        /* Console state. */
        enum moronet8_state state;
        /* Screen buffer. */
        moronet8_u32 *screen;
        /* API used for the bios only. */
        struct moronet8_api bios_api;
        /* API used for the cart. */
        struct moronet8_api cart_api;
        /* Bios data. */
        struct moronet8_cart_data bios;
        /* Cart data. */
        struct moronet8_cart_data cart;
        /* Pointer to bios or cart. */
        struct moronet8_cart_data *cart_select;
        struct moronet8_session *session;
    };

    /* Helpers for hooks */
    MORONET8_PUBLIC(void)
    moronet8_init_hooks(struct moronet8_hooks *hooks);

    MORONET8_PUBLIC(void)
    moronet8_reset_hooks(void);

    MORONET8_PUBLIC(struct moronet8_hooks *)
    moronet8_get_hooks(void);

    MORONET8_PUBLIC(void *)
    moronet8_malloc(size_t size);

    MORONET8_PUBLIC(void)
    moronet8_free(void *p);

    MORONET8_PUBLIC(void)
    moronet8_printf(const char *fmt, ...);

    MORONET8_PUBLIC(struct moronet8_session *)
    moronet8_session_host(const char *host);

    MORONET8_PUBLIC(struct moronet8_session *)
    moronet8_session_join(const char *host);

    MORONET8_PUBLIC(void)
    moronet8_session_delete(struct moronet8_session *session);

    MORONET8_PUBLIC(void)
    moronet8_session_broadcast(struct moronet8_session *session, const void *data, size_t size);

    MORONET8_PUBLIC(enum moronet8_session_state)
    moronet8_session_state_get(struct moronet8_session *session);

    MORONET8_PUBLIC(void)
    moronet8_session_poll(struct moronet8_session *session);

    /* API */
    MORONET8_PUBLIC(struct moronet8_api *)
    moronet8_api_init(struct moronet8_api *api, struct moronet8 *vm, enum moronet8_lang lang, enum moronet8_api_type type);

    MORONET8_PUBLIC(void)
    moronet8_api_delete(struct moronet8_api *api);

    MORONET8_PUBLIC(struct moronet8_api *)
    moronet8_api_load_string(struct moronet8_api *api, const char *buf, size_t size);

    MORONET8_PUBLIC(struct moronet8_api *)
    moronet8_api_tick(struct moronet8_api *api);

    MORONET8_PUBLIC(struct moronet8 *)
    moronet8_create(void);

    MORONET8_PUBLIC(void)
    moronet8_init(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_delete(struct moronet8 *vm);

    MORONET8_PUBLIC(size_t)
    moronet8_sizeof(void);

    MORONET8_PUBLIC(struct moronet8 *)
    moronet8_load_bios(struct moronet8 *vm, struct moronet8_cart_data *cart);

    MORONET8_PUBLIC(void)
    moronet8_unload_bios(struct moronet8 *vm);

    MORONET8_PUBLIC(struct moronet8 *)
    moronet8_load_cart(struct moronet8 *vm, struct moronet8_cart_data *cart);

    MORONET8_PUBLIC(void)
    moronet8_unload_cart(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_tick(struct moronet8 *vm, float dt);

    MORONET8_PUBLIC(moronet8_u8)
    moronet8_fontget(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_fontset(struct moronet8 *vm, moronet8_u8 id);

    MORONET8_PUBLIC(void)
    moronet8_print(struct moronet8 *vm, const char *buf, size_t size, moronet8_s32 x, moronet8_s32 y, moronet8_u8 col);

    MORONET8_PUBLIC(void)
    moronet8_printc(struct moronet8 *vm, char c, moronet8_s32 x, moronet8_s32 y, moronet8_u8 col);

    MORONET8_PUBLIC(int)
    moronet8_btn(struct moronet8 *vm, enum moronet8_button button, moronet8_u8 player);

    MORONET8_PUBLIC(int)
    moronet8_btnp(struct moronet8 *vm, enum moronet8_button button, moronet8_u8 player);

    MORONET8_PUBLIC(float)
    moronet8_dt(struct moronet8 *vm);

    MORONET8_PUBLIC(float)
    moronet8_sin(struct moronet8 *vm, float val);

    MORONET8_PUBLIC(float)
    moronet8_cos(struct moronet8 *vm, float val);

    MORONET8_PUBLIC(void)
    moronet8_cls(struct moronet8 *vm);

    MORONET8_PUBLIC(moronet8_u8)
    moronet8_colorget(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_colorset(struct moronet8 *vm, moronet8_u8 col);

    MORONET8_PUBLIC(void)
    moronet8_line(struct moronet8 *vm, moronet8_s32 x0, moronet8_s32 y0, moronet8_s32 x1, moronet8_s32 y1, moronet8_u8 col);

    MORONET8_PUBLIC(void)
    moronet8_rect(struct moronet8 *vm, moronet8_s32 x0, moronet8_s32 y0, moronet8_s32 x1, moronet8_s32 y1, moronet8_u8 col);

    MORONET8_PUBLIC(void)
    moronet8_rectfill(struct moronet8 *vm, moronet8_s32 x0, moronet8_s32 y0, moronet8_s32 x1, moronet8_s32 y1, moronet8_u8 col);

    MORONET8_PUBLIC(moronet8_u8)
    moronet8_tilesetget(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_tilesetset(struct moronet8 *vm, moronet8_u8 id);

    MORONET8_PUBLIC(void)
    moronet8_spr(struct moronet8 *vm, moronet8_u8 id, moronet8_s32 x, moronet8_s32 y, moronet8_u8 w, moronet8_u8 h);

    MORONET8_PUBLIC(moronet8_s32)
    moronet8_paltget(struct moronet8 *vm, moronet8_u8 col);

    MORONET8_PUBLIC(void)
    moronet8_paltset(struct moronet8 *vm, moronet8_u8 col, moronet8_s32 t);

    MORONET8_PUBLIC(moronet8_u8)
    moronet8_codeget(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_codeset(struct moronet8 *vm, moronet8_u8 id);

    MORONET8_PUBLIC(moronet8_u8)
    moronet8_peek(struct moronet8 *vm, moronet8_u16 address);

    MORONET8_PUBLIC(void)
    moronet8_poke(struct moronet8 *vm, moronet8_u16 address, moronet8_u8 value);

    MORONET8_PUBLIC(moronet8_u8)
    moronet8_pget(struct moronet8 *vm, moronet8_s32 x, moronet8_s32 y);

    MORONET8_PUBLIC(void)
    moronet8_pset(struct moronet8 *vm, moronet8_s32 x, moronet8_s32 y, moronet8_u8 value);

    MORONET8_PUBLIC(int)
    moronet8_nethost(struct moronet8 *vm);

    MORONET8_PUBLIC(int)
    moronet8_netclient(struct moronet8 *vm);

    MORONET8_PUBLIC(moronet8_netram_sp)
    moronet8_netspget(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_netspset(struct moronet8 *vm, moronet8_netram_sp offset);

    MORONET8_PUBLIC(void)
    moronet8_netpush(struct moronet8 *vm, moronet8_u8 value);

    MORONET8_PUBLIC(moronet8_u8)
    moronet8_netpop(struct moronet8 *vm);

    MORONET8_PUBLIC(enum moronet8_state)
    moronet8_state_get(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_state_set(struct moronet8 *vm, enum moronet8_state state);

    MORONET8_PUBLIC(void)
    moronet8_load(struct moronet8 *vm, const char *cart);

    MORONET8_PUBLIC(enum moronet8_session_state)
    moronet8_netsessionstate(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_netsessionstart(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_netsessionjoin(struct moronet8 *vm, const char *host);

    MORONET8_PUBLIC(void)
    moronet8_netsessionleave(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_netsessionpoll(struct moronet8 *vm);

    MORONET8_PUBLIC(moronet8_pktram_sp)
    moronet8_pktspget(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_pktspset(struct moronet8 *vm, moronet8_pktram_sp offset);

    MORONET8_PUBLIC(void)
    moronet8_pktpush(struct moronet8 *vm, moronet8_u8 value);

    MORONET8_PUBLIC(moronet8_u8)
    moronet8_pktpop(struct moronet8 *vm);

    MORONET8_PUBLIC(void)
    moronet8_pktsend(struct moronet8 *vm);

#ifdef __cplusplus
}
#endif
