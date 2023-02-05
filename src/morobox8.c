#include "morobox8.h"
#include "pack.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_MATH_H
#include <math.h>
#endif

#if MOROBOX8_LUA_API
#include "api/lua_api.h"
#endif

#if MOROBOX8_JS_API
#include "api/js_api.h"
#endif

#if MOROBOX8_FILESYSTEM
#include <fs.h>
#endif

#include <assert.h>

#include <stdarg.h>

typedef struct morobox8_packer morobox8_packer;
typedef struct morobox8_hooks morobox8_hooks;
typedef struct morobox8_socket morobox8_socket;
typedef enum morobox8_session_state morobox8_session_state;
typedef enum morobox8_state morobox8_state;
typedef enum morobox8_api_type morobox8_api_type;
typedef enum morobox8_api_lang morobox8_api_lang;
typedef enum morobox8_button morobox8_button;
typedef struct morobox8_session morobox8_session;
typedef struct morobox8_api morobox8_api;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_sprite morobox8_cart_sprite;
typedef struct morobox8_cart_header morobox8_cart_header;
typedef struct morobox8_cart_data morobox8_cart_data;
typedef struct morobox8 morobox8;

#ifdef HAVE_MALLOC
#if defined(_MSC_VER)
/* work around MSVC error C2322: '...' address of dllimport '...' is not static */
static void *MOROBOX8_CDECL internal_malloc(size_t size)
{
    return MOROBOX8_MALLOC(size);
}

static void MOROBOX8_CDECL internal_free(void *pointer)
{
    MOROBOX8_FREE(pointer);
}
#else
#define internal_malloc MOROBOX8_MALLOC
#define internal_free MOROBOX8_FREE
#endif
#else
#define internal_malloc NULL
#define internal_free NULL
#endif

#ifdef HAVE_PRINTF
#if defined(_MSC_VER)
/* work around MSVC error C2322: '...' address of dllimport '...' is not static */
static void MOROBOX8_CDECL internal_printf(const char *fmt, ...)
{
    printf(fmt, ...);
}
#else
#define internal_printf MOROBOX8_PRINTF
#endif
#else
#define internal_printf NULL
#endif

static morobox8_hooks morobox8_global_hooks = {
    internal_malloc,
    internal_free,
    internal_printf,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL};

MOROBOX8_PUBLIC(void)
morobox8_init_hooks(morobox8_hooks *hooks)
{
    if (hooks->malloc_fn)
        morobox8_global_hooks.malloc_fn = hooks->malloc_fn;
    if (hooks->free_fn)
        morobox8_global_hooks.free_fn = hooks->free_fn;
    if (hooks->printf_fn)
        morobox8_global_hooks.printf_fn = hooks->printf_fn;
    if (hooks->host_session_fn)
        morobox8_global_hooks.host_session_fn = hooks->host_session_fn;
    if (hooks->join_session_fn)
        morobox8_global_hooks.join_session_fn = hooks->join_session_fn;
    if (hooks->delete_session_fn)
        morobox8_global_hooks.delete_session_fn = hooks->delete_session_fn;
    if (hooks->broadcast_session_fn)
        morobox8_global_hooks.broadcast_session_fn = hooks->broadcast_session_fn;
    if (hooks->receive_session_fn)
        morobox8_global_hooks.receive_session_fn = hooks->receive_session_fn;
    if (hooks->session_state_get_fn)
        morobox8_global_hooks.session_state_get_fn = hooks->session_state_get_fn;
    if (hooks->poll_session_fn)
        morobox8_global_hooks.poll_session_fn = hooks->poll_session_fn;
}

MOROBOX8_PUBLIC(void)
morobox8_reset_hooks(void)
{
    morobox8_global_hooks.malloc_fn = NULL;
    morobox8_global_hooks.free_fn = NULL;
    morobox8_global_hooks.printf_fn = NULL;
    morobox8_global_hooks.host_session_fn = NULL;
    morobox8_global_hooks.join_session_fn = NULL;
    morobox8_global_hooks.delete_session_fn = NULL;
    morobox8_global_hooks.broadcast_session_fn = NULL;
    morobox8_global_hooks.receive_session_fn = NULL;
    morobox8_global_hooks.session_state_get_fn = NULL;
    morobox8_global_hooks.poll_session_fn = NULL;
}

MOROBOX8_PUBLIC(morobox8_hooks *)
morobox8_get_hooks(void)
{
    return &morobox8_global_hooks;
}

MOROBOX8_PUBLIC(void *)
morobox8_malloc(size_t size)
{
    if (!morobox8_global_hooks.malloc_fn)
    {
        return NULL;
    }

    return morobox8_global_hooks.malloc_fn(size);
}

MOROBOX8_PUBLIC(void)
morobox8_free(void *p)
{
    if (morobox8_global_hooks.free_fn)
    {
        morobox8_global_hooks.free_fn(p);
    }
}

MOROBOX8_PUBLIC(void)
morobox8_printf(const char *fmt, ...)
{
    if (morobox8_global_hooks.printf_fn)
    {
        va_list args;
        va_start(args, fmt);
        morobox8_global_hooks.printf_fn(fmt, args);
        va_end(args);
    }
}

MOROBOX8_PUBLIC(morobox8_session *)
morobox8_session_host(const char *host)
{
    if (!morobox8_global_hooks.host_session_fn)
    {
        return NULL;
    }

    return morobox8_global_hooks.host_session_fn(host);
}

MOROBOX8_PUBLIC(morobox8_session *)
morobox8_session_join(const char *host)
{
    if (!morobox8_global_hooks.join_session_fn)
    {
        return NULL;
    }

    return morobox8_global_hooks.join_session_fn(host);
}

MOROBOX8_PUBLIC(void)
morobox8_session_delete(morobox8_session *session)
{
    if (!morobox8_global_hooks.delete_session_fn)
    {
        return;
    }

    morobox8_global_hooks.delete_session_fn(session);
}

MOROBOX8_PUBLIC(void)
morobox8_session_broadcast(morobox8_session *session, const void *buf, size_t size)
{
    if (!morobox8_global_hooks.broadcast_session_fn)
    {
        return;
    }

    morobox8_global_hooks.broadcast_session_fn(session, buf, size);
}

MOROBOX8_PUBLIC(size_t)
morobox8_session_receive(morobox8_session *session, void *buf, size_t size)
{
    if (!morobox8_global_hooks.receive_session_fn)
    {
        return 0;
    }

    return morobox8_global_hooks.receive_session_fn(session, buf, size);
}

MOROBOX8_PUBLIC(enum morobox8_session_state)
morobox8_session_state_get(morobox8_session *session)
{
    if (!session || !morobox8_global_hooks.session_state_get_fn)
    {
        return MOROBOX8_SESSION_CLOSED;
    }

    return morobox8_global_hooks.session_state_get_fn(session);
}

MOROBOX8_PUBLIC(void)
morobox8_session_poll(morobox8_session *session)
{
    if (!morobox8_global_hooks.poll_session_fn)
    {
        return;
    }

    morobox8_global_hooks.poll_session_fn(session);
}

#define _MOROBOX8_MALLOC morobox8_global_hooks.malloc_fn
#define _MOROBOX8_FREE morobox8_global_hooks.free_fn
#define _MOROBOX8_PRINTF morobox8_global_hooks.printf_fn

#define MOROBOX8_TRUE 1
#define MOROBOX8_FALSE 0

MOROBOX8_PUBLIC(morobox8_api *)
morobox8_api_init(morobox8_api *api, morobox8 *vm, morobox8_api_lang lang, morobox8_api_type type)
{
    /** Delete old API instance */
    morobox8_api_delete(api);

    api->type = type;

    /** Create new API instance */
    switch (lang)
    {
#if MOROBOX8_LUA_API
    case MOROBOX8_API_LANG_LUA:
        return morobox8_lua_api_init(api, vm, type);
#endif
#if MOROBOX8_JS_API
    case MOROBOX8_API_LANG_JS:
        return morobox8_js_api_init(api, vm, type);
#endif
    default:
        return NULL;
    }
}

MOROBOX8_PUBLIC(void)
morobox8_api_delete(morobox8_api *api)
{
    if (api->free)
    {
        api->free(api);
    }

    memset(api, 0, sizeof(morobox8_api));
}

MOROBOX8_PUBLIC(morobox8_api *)
morobox8_api_load_string(morobox8_api *api, const char *buf, size_t size)
{
    return api->load_string ? api->load_string(api, buf, size) : NULL;
}

MOROBOX8_PUBLIC(morobox8_api *)
morobox8_api_tick(morobox8_api *api)
{
    return api->tick ? api->tick(api) : NULL;
}

MOROBOX8_PUBLIC(morobox8 *)
morobox8_create(void)
{
    morobox8 *o = (morobox8 *)_MOROBOX8_MALLOC(sizeof(morobox8));
    if (o)
    {
        morobox8_init(o);
    }

    return o;
}

MOROBOX8_PUBLIC(void)
morobox8_init(morobox8 *vm)
{
    memset(vm, 0, sizeof(morobox8));
    vm->state = MOROBOX8_STATE_BIOS;
}

MOROBOX8_PUBLIC(void)
morobox8_delete(morobox8 *vm)
{
    morobox8_api_delete(&vm->cart_api);
    morobox8_api_delete(&vm->bios_api);
    _MOROBOX8_FREE(vm);
}

MOROBOX8_PUBLIC(size_t)
morobox8_color_format(void)
{
    return MOROBOX8_COLOR_FORMAT;
}

MOROBOX8_PUBLIC(size_t)
morobox8_sizeof(void)
{
    return sizeof(struct morobox8);
}

MOROBOX8_PUBLIC(void *)
morobox8_get_vram(struct morobox8 *vm)
{
    return vm->ram.vram;
}

MOROBOX8_PUBLIC(void)
morobox8_set_vram(struct morobox8 *vm, void *buffer)
{
    vm->ram.vram = buffer;
}

static morobox8 *morobox8_load_any(morobox8 *vm, morobox8_api *api, morobox8_cart_data *dst, morobox8_cart_data *src, morobox8_api_type type)
{
    memcpy(dst, src, sizeof(morobox8_cart_data));
    vm->cart_select = dst;

    if (!morobox8_api_init(api, vm, dst->code.lang, type))
    {
        return NULL;
    }

    if (!morobox8_api_load_string(api, &dst->code.text[0], MOROBOX8_CART_CODE_SIZE))
    {
        if (dst == &vm->cart)
            morobox8_printf("failed loads");
        return NULL;
    }

    if (dst == &vm->cart)
        morobox8_printf("api init");
    return vm;
}

static void morobox8_unload_any(morobox8_api *api, morobox8_cart_data *cart)
{
    morobox8_api_delete(api);
    memset(cart, 0, sizeof(morobox8_cart_data));
}

MOROBOX8_PUBLIC(morobox8 *)
morobox8_load_bios(morobox8 *vm, morobox8_cart_data *cart)
{
    return morobox8_load_any(vm, &vm->bios_api, &vm->bios, cart, MOROBOX8_API_CART | MOROBOX8_API_BIOS);
}

MOROBOX8_PUBLIC(morobox8 *)
morobox8_load_cart(morobox8 *vm, morobox8_cart_data *cart)
{
    return morobox8_load_any(vm, &vm->cart_api, &vm->cart, cart, MOROBOX8_API_CART);
}

MOROBOX8_PUBLIC(void)
morobox8_unload_bios(morobox8 *vm)
{
    morobox8_unload_any(&vm->bios_api, &vm->bios);
}

MOROBOX8_PUBLIC(void)
morobox8_unload_cart(morobox8 *vm)
{
    morobox8_unload_any(&vm->cart_api, &vm->cart);
}

MOROBOX8_PUBLIC(void)
morobox8_tick(morobox8 *vm, float dt)
{
    vm->ram.dt = dt;

    morobox8_session_state session_state = morobox8_session_state_get(vm->session);
    if (session_state == MOROBOX8_SESSION_JOINED)
    {
        vm->ram.netram_sp = morobox8_session_receive(
            vm->session,
            &vm->ram.netram,
            MOROBOX8_NETRAM_SIZE);
    }

    /* Only tick cart when in those states */
    if (vm->state == MOROBOX8_STATE_CART || vm->state == MOROBOX8_STATE_OVERLAY)
    {
        vm->cart_select = &vm->cart;
        morobox8_api_tick(&vm->cart_api);
    }

    /* Always tick bios */

    if (session_state == MOROBOX8_SESSION_HOSTING)
    {
        morobox8_session_broadcast(
            vm->session,
            &vm->ram.netram,
            vm->ram.netram_sp);
    }
}

static morobox8_cart_tileset *morobox8_selected_font(struct morobox8 *vm)
{
    assert(vm->cart_select);
    return &vm->cart_select->font;
}

static morobox8_cart_tileset *morobox8_selected_tileset(struct morobox8 *vm)
{
    assert(vm->cart_select);
    return &vm->cart_select->tileset;
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_fontget(morobox8 *vm)
{
    return 0;
}

MOROBOX8_PUBLIC(void)
morobox8_fontset(morobox8 *vm, morobox8_u8 id)
{
    assert(vm->cart_select);
    // morobox8_cart_select_font(vm->cart_select, id);
}

MOROBOX8_PUBLIC(void)
morobox8_print(morobox8 *vm, const char *buf, size_t size, morobox8_s32 x, morobox8_s32 y, morobox8_u8 col)
{
    morobox8_printf(buf);
    for (size_t i = 0; i < size; ++i)
    {
        morobox8_printc(vm, buf[i], x + i * MOROBOX8_SPRITE_WIDTH, y, col);
    }
}

static void morobox8_set_pixel(morobox8 *vm, morobox8_s32 x, morobox8_s32 y, morobox8_u8 value, morobox8_u8 *col)
{
    assert(vm->cart_select);
    if (vm->cart_select->palette[value].t)
    {
        return;
    }

    if (col)
    {
        value = *col;
        if (vm->cart_select->palette[value].t)
        {
            return;
        }
    }

    struct morobox8_cart_color *c = &vm->cart_select->palette[value];

#if MOROBOX8_COLOR_FORMAT == MOROBOX8_COLOR_RGB565
    morobox8_u16 c16 = ((c->r & 0xf8) << 8) |
                       ((c->g & 0xfc) << 3) |
                       ((c->b & 0xf8) >> 3);

    ((morobox8_u16 *)vm->ram.vram)[x + y * MOROBOX8_SCREEN_WIDTH] = __builtin_bswap16(c16);
#elif MOROBOX8_COLOR_FORMAT == MOROBOX8_COLOR_RGBA
    morobox8_u32 c32 = ((c->r & 0xf8) << 24) |
                       ((c->g & 0xfc) << 16) |
                       ((c->b & 0xf8) >> 8) |
                       0xFF;

    ((morobox8_u32 *)vm->ram.vram)[x + y * MOROBOX8_SCREEN_WIDTH] = c32;
#endif
}

static void morobox8_draw_sprite(morobox8 *vm, morobox8_cart_tileset *tileset, morobox8_u8 id, morobox8_s32 x, morobox8_s32 y, morobox8_u8 *col)
{
    if (!tileset)
    {
        return;
    }

    morobox8_cart_sprite *sprite = &tileset->sprites[id];
    for (morobox8_u8 j = 0; j < MOROBOX8_SPRITE_HEIGHT; ++j)
    {
        for (morobox8_u8 i = 0; i < MOROBOX8_SPRITE_WIDTH; ++i)
        {
            morobox8_set_pixel(vm, x + i, y + j, sprite->pixels[i + j * MOROBOX8_SPRITE_WIDTH], col);
        }
    }
}

static void morobox8_draw_sprite_wh(morobox8 *vm, morobox8_cart_tileset *tileset, morobox8_u8 id, morobox8_s32 x, morobox8_s32 y, morobox8_u8 w, morobox8_u8 h)
{
    if (!tileset)
    {
        return;
    }

    for (morobox8_u8 j = 0; j < h; j++)
    {
        for (morobox8_u8 i = 0; i < w; i++)
        {
            morobox8_draw_sprite(
                vm,
                tileset,
                id + i + j * MOROBOX8_TILESET_WIDTH,
                x + i * MOROBOX8_SPRITE_WIDTH,
                y + j * MOROBOX8_SPRITE_HEIGHT,
                NULL);
        }
    }
}

MOROBOX8_PUBLIC(void)
morobox8_printc(struct morobox8 *vm, char c, morobox8_s32 x, morobox8_s32 y, morobox8_u8 col)
{
    morobox8_draw_sprite(vm, morobox8_selected_font(vm), (morobox8_u8)c, x, y, &col);
}

MOROBOX8_PUBLIC(int)
morobox8_btn(morobox8 *vm, morobox8_button button, morobox8_u8 player)
{
    return ((morobox8_u8 *)&vm->ram.buttons)[player] & (1 << button);
}

MOROBOX8_PUBLIC(int)
morobox8_btnp(morobox8 *vm, morobox8_button button, morobox8_u8 player)
{
    return morobox8_btn(vm, button, player) != 0;
}

MOROBOX8_PUBLIC(float)
morobox8_dt(morobox8 *vm)
{
    return vm->ram.dt;
}

MOROBOX8_PUBLIC(float)
morobox8_sin(struct morobox8 *vm, float val)
{
    return (float)sin(val);
}

MOROBOX8_PUBLIC(float)
morobox8_cos(struct morobox8 *vm, float val)
{
    return (float)cos(val);
}

MOROBOX8_PUBLIC(void)
morobox8_cls(morobox8 *vm)
{
    memset(vm->ram.vram, 0, MOROBOX8_VRAM_SIZE);
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_colorget(morobox8 *vm)
{
    return vm->ram.color;
}

MOROBOX8_PUBLIC(void)
morobox8_colorset(morobox8 *vm, morobox8_u8 col)
{
    vm->ram.color = col;
}

#define MOROBOX8_SWAP(a, b, type) \
    do                            \
    {                             \
        type temp = a;            \
        a = b;                    \
        b = temp;                 \
    } while (0)

static float morobox8_init_line(morobox8_s32 *x0, morobox8_s32 *x1, morobox8_s32 *y0, morobox8_s32 *y1)
{
    if (*y0 > *y1)
    {
        MOROBOX8_SWAP(*x0, *x1, morobox8_s32);
        MOROBOX8_SWAP(*y0, *y1, morobox8_s32);
    }

    float t = (*x1 - *x0) / (float)(*y1 - *y0);

    if (*y0 < 0)
        *x0 -= *y0 * t, *y0 = 0;
    if (*y1 > MOROBOX8_SCREEN_WIDTH)
        *x1 += (MOROBOX8_SCREEN_WIDTH - *y0) * t, *y1 = MOROBOX8_SCREEN_WIDTH;

    return t;
}

MOROBOX8_PUBLIC(void)
morobox8_line(morobox8 *vm, morobox8_s32 x0, morobox8_s32 y0, morobox8_s32 x1, morobox8_s32 y1, morobox8_u8 col)
{
    if (abs(x0 - x1) < abs(y0 - y1))
        for (float t = morobox8_init_line(&x0, &x1, &y0, &y1); y0 < y1; y0++, x0 += t)
            morobox8_pset(vm, x0, y0, col);
    else
        for (float t = morobox8_init_line(&y0, &y1, &x0, &x1); x0 < x1; x0++, y0 += t)
            morobox8_pset(vm, x0, y0, col);

    morobox8_pset(vm, x1, y1, col);
}

MOROBOX8_PUBLIC(void)
morobox8_rect(morobox8 *vm, morobox8_s32 x0, morobox8_s32 y0, morobox8_s32 x1, morobox8_s32 y1, morobox8_u8 col)
{
    morobox8_line(vm, x0, y0, x1, y0, col);
    morobox8_line(vm, x0, y1, x1, y1, col);
    morobox8_line(vm, x0, y0, x0, y1, col);
    morobox8_line(vm, x1, y0, x1, y1, col);
}

MOROBOX8_PUBLIC(void)
morobox8_rectfill(morobox8 *vm, morobox8_s32 x0, morobox8_s32 y0, morobox8_s32 x1, morobox8_s32 y1, morobox8_u8 col)
{
    for (morobox8_s32 i = y0; i <= y1; ++i)
    {
        morobox8_line(vm, x0, i, x1, i, col);
    }
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_tilesetget(morobox8 *vm)
{
    return 0;
}

MOROBOX8_PUBLIC(void)
morobox8_tilesetset(morobox8 *vm, morobox8_u8 id)
{
    assert(vm->cart_select);
    // morobox8_cart_select_tileset(vm->cart_select, id);
}

MOROBOX8_PUBLIC(void)
morobox8_spr(morobox8 *vm, morobox8_u8 id, morobox8_s32 x, morobox8_s32 y, morobox8_u8 w, morobox8_u8 h)
{
    morobox8_draw_sprite_wh(vm, morobox8_selected_tileset(vm), id, x, y, w, h);
}

MOROBOX8_PUBLIC(morobox8_s32)
morobox8_paltget(morobox8 *vm, morobox8_u8 col)
{
    assert(vm->cart_select);
    return vm->cart_select->palette[col].t;
}

MOROBOX8_PUBLIC(void)
morobox8_paltset(morobox8 *vm, morobox8_u8 col, morobox8_s32 t)
{
    assert(vm->cart_select);
    vm->cart_select->palette[col].t = ((morobox8_u8)t != 0);
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_codeget(morobox8 *vm)
{
    return 0;
}

MOROBOX8_PUBLIC(void)
morobox8_codeset(morobox8 *vm, morobox8_u8 id)
{
    assert(vm->cart_select);
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_peek(morobox8 *vm, morobox8_u16 address)
{
    return ((morobox8_u8 *)&vm->ram.vram)[address];
}

MOROBOX8_PUBLIC(void)
morobox8_poke(morobox8 *vm, morobox8_u16 address, morobox8_u8 value)
{
    ((morobox8_u8 *)&vm->ram.vram)[address] = value;
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_pget(struct morobox8 *vm, morobox8_s32 x, morobox8_s32 y)
{
    return morobox8_peek(vm, x + ((morobox8_u16)y) * MOROBOX8_SCREEN_WIDTH);
}

MOROBOX8_PUBLIC(void)
morobox8_pset(morobox8 *vm, morobox8_s32 x, morobox8_s32 y, morobox8_u8 value)
{
    assert(vm->cart_select);
    if (vm->cart_select->palette[value].t)
    {
        return;
    }

    morobox8_poke(vm, x + ((morobox8_u16)y) * MOROBOX8_SCREEN_WIDTH, value);
}

MOROBOX8_PUBLIC(morobox8_netram_sp)
morobox8_netspget(morobox8 *vm)
{
    return vm->ram.netram_sp;
}

MOROBOX8_PUBLIC(void)
morobox8_netspset(morobox8 *vm, morobox8_netram_sp offset)
{
    vm->ram.netram_sp = offset;
}

#define MOROBOX8_STACK_PUSH(stack, sp, size, value) \
    stack[sp] = value;                              \
    if (sp < size - 1)                              \
    {                                               \
        sp++;                                       \
    }

#define MOROBOX8_STACK_POP(stack, sp, size) \
    if (sp > 0)                             \
    {                                       \
        sp--;                               \
    }                                       \
    return stack[sp]

MOROBOX8_PUBLIC(int)
morobox8_nethost(morobox8 *vm)
{
    return morobox8_session_state_get(vm->session) == MOROBOX8_SESSION_HOSTING;
}

MOROBOX8_PUBLIC(int)
morobox8_netclient(morobox8 *vm)
{
    return morobox8_session_state_get(vm->session) == MOROBOX8_SESSION_JOINED;
}

MOROBOX8_PUBLIC(void)
morobox8_netpush(morobox8 *vm, morobox8_u8 value)
{
    MOROBOX8_STACK_PUSH(vm->ram.netram, vm->ram.netram_sp, MOROBOX8_NETRAM_SIZE, value);
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_netpop(morobox8 *vm)
{
    morobox8_u8 value = vm->ram.netram[vm->ram.netram_sp];
    if (vm->ram.netram_sp < MOROBOX8_NETRAM_SIZE - 1)
    {
        vm->ram.netram_sp++;
    }

    return value;
}

MOROBOX8_PUBLIC(morobox8_state)
morobox8_state_get(morobox8 *vm)
{
    return vm->state;
}

MOROBOX8_PUBLIC(void)
morobox8_state_set(morobox8 *vm, morobox8_state state)
{
    switch (state)
    {
    case MOROBOX8_STATE_BIOS:
        morobox8_printf("state BIOS\n");
        break;
    case MOROBOX8_STATE_CART:
        morobox8_printf("state CART\n");
        break;
    case MOROBOX8_STATE_OVERLAY:
        morobox8_printf("state OVERLAY\n");
        break;
    default:
        break;
    }
    vm->state = state;
}

MOROBOX8_PUBLIC(void)
morobox8_load(morobox8 *vm, const char *cart)
{
#ifdef MOROBOX8_FILESYSTEM
    morobox8_cart c;
    morobox8_pack(cart, &c);

    morobox8_load_cart(vm, &c.data);
#else
    morobox8_printf("load not supported on this platform\n");
#endif
}

MOROBOX8_PUBLIC(morobox8_session_state)
morobox8_netsessionstate(morobox8 *vm)
{
    return vm->session ? morobox8_session_state_get(vm->session) : MOROBOX8_SESSION_CLOSED;
}

MOROBOX8_PUBLIC(void)
morobox8_netsessionstart(morobox8 *vm)
{
    morobox8_netsessionleave(vm);

    morobox8_printf("start net session\n");
    vm->session = morobox8_session_host("127.0.0.1");
}

MOROBOX8_PUBLIC(void)
morobox8_netsessionjoin(morobox8 *vm, const char *host)
{
    morobox8_netsessionleave(vm);

    morobox8_printf("join net session %s\n", host);
    vm->session = morobox8_session_join(host);
}

MOROBOX8_PUBLIC(void)
morobox8_netsessionleave(morobox8 *vm)
{
    morobox8_printf("leave net session\n");
    if (!vm->session)
    {
        return;
    }

    morobox8_session_delete(vm->session);
    vm->session = NULL;
}

MOROBOX8_PUBLIC(void)
morobox8_netsessionpoll(morobox8 *vm)
{
    if (!vm->session)
    {
        return;
    }

    morobox8_session_poll(vm->session);
}

MOROBOX8_PUBLIC(morobox8_pktram_sp)
morobox8_pktspget(morobox8 *vm)
{
    return vm->ram.pktram_sp;
}

MOROBOX8_PUBLIC(void)
morobox8_pktspset(morobox8 *vm, morobox8_pktram_sp offset)
{
    vm->ram.pktram_sp = offset;
}

MOROBOX8_PUBLIC(void)
morobox8_pktpush(morobox8 *vm, morobox8_u8 value)
{
    MOROBOX8_STACK_PUSH(vm->ram.pktram, vm->ram.pktram_sp, MOROBOX8_PKTRAM_SIZE, value);
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_pktpop(morobox8 *vm)
{
    MOROBOX8_STACK_POP(vm->ram.pktram, vm->ram.pktram_sp, MOROBOX8_PKTRAM_SIZE);
}

MOROBOX8_PUBLIC(void)
morobox8_pktsend(morobox8 *vm)
{
}

#undef MOROBOX8_STACK_PUSH
#undef MOROBOX8_STACK_POP
