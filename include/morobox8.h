#pragma once

#include "morobox8_config.h"
#include "morobox8_types.h"
#include "morobox8_limits.h"
#include "api/api_type.h"
#include "api/api.h"
#include "cart/cart.h"
#include "network/session_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MOROBOX8_NAME "morobox8"

    typedef morobox8_u8 morobox8_netram_sp;
    typedef morobox8_u8 morobox8_pktram_sp;

#define MOROBOX8_UNUSED(x) (void)(x)

    struct morobox8_socket;
    struct morobox8_session;
    struct morobox8_reader;
    struct morobox8_storage;

    enum morobox8_button
    {
        MOROBOX8_BUTTON_LEFT = 0,
        MOROBOX8_BUTTON_RIGHT,
        MOROBOX8_BUTTON_UP,
        MOROBOX8_BUTTON_DOWN,
        MOROBOX8_BUTTON_A,
        MOROBOX8_BUTTON_START,
    };

    enum morobox8_state
    {
        /* Displaying system menus. */
        MOROBOX8_STATE_BIOS = 0,
        /* Displaying game. */
        MOROBOX8_STATE_CART,
        /* Displaying system overlay over game. */
        MOROBOX8_STATE_OVERLAY
    };

    struct morobox8
    {
        struct
        {
            /* Pointer to vram. */
            void *vram;
            /* Networked ram. */
            morobox8_u8 netram[MOROBOX8_NETRAM_SIZE];
            /* Stack pointer for netram. */
            morobox8_netram_sp netram_sp;
            /* Packet ram. */
            morobox8_u8 pktram[MOROBOX8_PKTRAM_SIZE];
            /* Stack pointer for pktram. */
            morobox8_pktram_sp pktram_sp;
            /* Draw color. */
            morobox8_u8 color;
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
            /* If a code bank must be loaded for bios. */
            morobox8_u8 bios_load_code;
            /* Next code bank to load for bios. */
            morobox8_u8 bios_next_code;
            /* If a code bank must be loaded for cart. */
            morobox8_u8 cart_load_code;
            /* Next code bank to load for cart. */
            morobox8_u8 cart_next_code;
        } ram;
        /* Console state. */
        enum morobox8_state state;
        /* API used for the bios only. */
        struct morobox8_api bios_api;
        /* API used for the cart. */
        struct morobox8_api cart_api;
        /* Bios data. */
        struct morobox8_cart_data bios;
        /* Pointer to bios file. */
        struct morobox8_file *bios_file;
        /* Cart data. */
        struct morobox8_cart_data cart;
        /* Pointer to cart file. */
        struct morobox8_file *cart_file;
        /* Pointer to bios or cart. */
        struct morobox8_cart_data *cart_select;
        struct morobox8_session *session;
        /* Pointer to the external reader. */
        struct morobox8_reader *reader;
        /* Pointer to the internal storage. */
        struct morobox8_storage *storage;
    };

    /* API */
    MOROBOX8_PUBLIC(struct morobox8_api *)
    morobox8_api_init(struct morobox8_api *api, struct morobox8 *vm, enum morobox8_api_lang lang, enum morobox8_api_type type);

    MOROBOX8_PUBLIC(void)
    morobox8_api_delete(struct morobox8_api *api);

    MOROBOX8_PUBLIC(struct morobox8_api *)
    morobox8_api_load_string(struct morobox8_api *api, const char *buf, size_t size);

    MOROBOX8_PUBLIC(struct morobox8_api *)
    morobox8_api_tick(struct morobox8_api *api);

    MOROBOX8_PUBLIC(struct morobox8 *)
    morobox8_create(void);

    MOROBOX8_PUBLIC(void)
    morobox8_init(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_delete(struct morobox8 *vm);

    MOROBOX8_PUBLIC(size_t)
    morobox8_color_format(void);

    MOROBOX8_PUBLIC(size_t)
    morobox8_sizeof(void);

    MOROBOX8_PUBLIC(void *)
    morobox8_get_vram(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_set_vram(struct morobox8 *vm, void *buffer);

    MOROBOX8_PUBLIC(struct morobox8 *)
    morobox8_load_bios(struct morobox8 *vm, struct morobox8_cart_data *cart);

    MOROBOX8_PUBLIC(void)
    morobox8_unload_bios(struct morobox8 *vm);

    MOROBOX8_PUBLIC(struct morobox8 *)
    morobox8_load_cart(struct morobox8 *vm, struct morobox8_cart_data *cart);

    MOROBOX8_PUBLIC(void)
    morobox8_unload_cart(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_tick(struct morobox8 *vm, float dt);

    MOROBOX8_PUBLIC(void)
    morobox8_font(struct morobox8 *vm, const char *name, size_t size);

    MOROBOX8_PUBLIC(void)
    morobox8_print(struct morobox8 *vm, const char *buf, size_t size, morobox8_s32 x, morobox8_s32 y, morobox8_u8 col);

    MOROBOX8_PUBLIC(void)
    morobox8_printc(struct morobox8 *vm, char c, morobox8_s32 x, morobox8_s32 y, morobox8_u8 col);

    MOROBOX8_PUBLIC(int)
    morobox8_btn(struct morobox8 *vm, enum morobox8_button button, morobox8_u8 player);

    MOROBOX8_PUBLIC(int)
    morobox8_btnp(struct morobox8 *vm, enum morobox8_button button, morobox8_u8 player);

    MOROBOX8_PUBLIC(float)
    morobox8_dt(struct morobox8 *vm);

    MOROBOX8_PUBLIC(float)
    morobox8_sin(struct morobox8 *vm, float val);

    MOROBOX8_PUBLIC(float)
    morobox8_cos(struct morobox8 *vm, float val);

    MOROBOX8_PUBLIC(void)
    morobox8_cls(struct morobox8 *vm);

    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_colorget(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_colorset(struct morobox8 *vm, morobox8_u8 col);

    MOROBOX8_PUBLIC(void)
    morobox8_line(struct morobox8 *vm, morobox8_s32 x0, morobox8_s32 y0, morobox8_s32 x1, morobox8_s32 y1, morobox8_u8 col);

    MOROBOX8_PUBLIC(void)
    morobox8_rect(struct morobox8 *vm, morobox8_s32 x0, morobox8_s32 y0, morobox8_s32 x1, morobox8_s32 y1, morobox8_u8 col);

    MOROBOX8_PUBLIC(void)
    morobox8_rectfill(struct morobox8 *vm, morobox8_s32 x0, morobox8_s32 y0, morobox8_s32 x1, morobox8_s32 y1, morobox8_u8 col);

    MOROBOX8_PUBLIC(void)
    morobox8_tileset(struct morobox8 *vm, const char *name, size_t size);

    MOROBOX8_PUBLIC(void)
    morobox8_spr(struct morobox8 *vm, morobox8_u8 id, morobox8_s32 x, morobox8_s32 y, morobox8_u8 w, morobox8_u8 h);

    MOROBOX8_PUBLIC(morobox8_s32)
    morobox8_paltget(struct morobox8 *vm, morobox8_u8 col);

    MOROBOX8_PUBLIC(void)
    morobox8_paltset(struct morobox8 *vm, morobox8_u8 col, morobox8_s32 t);

    MOROBOX8_PUBLIC(void)
    morobox8_code(struct morobox8 *vm, const char *name, size_t size);

    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_peek(struct morobox8 *vm, morobox8_u16 address);

    MOROBOX8_PUBLIC(void)
    morobox8_poke(struct morobox8 *vm, morobox8_u16 address, morobox8_u8 value);

    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_pget(struct morobox8 *vm, morobox8_s32 x, morobox8_s32 y);

    MOROBOX8_PUBLIC(void)
    morobox8_pset(struct morobox8 *vm, morobox8_s32 x, morobox8_s32 y, morobox8_u8 value);

    MOROBOX8_PUBLIC(int)
    morobox8_nethost(struct morobox8 *vm);

    MOROBOX8_PUBLIC(int)
    morobox8_netclient(struct morobox8 *vm);

    MOROBOX8_PUBLIC(morobox8_netram_sp)
    morobox8_netspget(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_netspset(struct morobox8 *vm, morobox8_netram_sp offset);

    MOROBOX8_PUBLIC(void)
    morobox8_netpush(struct morobox8 *vm, morobox8_u8 value);

    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_netpop(struct morobox8 *vm);

    MOROBOX8_PUBLIC(enum morobox8_state)
    morobox8_state_get(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_state_set(struct morobox8 *vm, enum morobox8_state state);

    MOROBOX8_PUBLIC(enum morobox8_session_state)
    morobox8_netsessionstate(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_netsessionstart(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_netsessionjoin(struct morobox8 *vm, const char *host);

    MOROBOX8_PUBLIC(void)
    morobox8_netsessionleave(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_netsessionpoll(struct morobox8 *vm);

    MOROBOX8_PUBLIC(morobox8_pktram_sp)
    morobox8_pktspget(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_pktspset(struct morobox8 *vm, morobox8_pktram_sp offset);

    MOROBOX8_PUBLIC(void)
    morobox8_pktpush(struct morobox8 *vm, morobox8_u8 value);

    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_pktpop(struct morobox8 *vm);

    MOROBOX8_PUBLIC(void)
    morobox8_pktsend(struct morobox8 *vm);

#ifdef __cplusplus
}
#endif
