#include "moronet8.h"

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

#if MORONET8_LUA_API
#include "api/lua_api.h"
#endif

#if MORONET8_JS_API
#include "api/js_api.h"
#endif

#if MORONET8_FILESYSTEM
#include <fs.h>
#endif

#include <assert.h>

#include <stdarg.h>

typedef struct moronet8_hooks moronet8_hooks;
typedef struct moronet8_socket moronet8_socket;
typedef enum moronet8_session_state moronet8_session_state;
typedef enum moronet8_state moronet8_state;
typedef enum moronet8_api_type moronet8_api_type;
typedef enum moronet8_lang moronet8_lang;
typedef enum moronet8_button moronet8_button;
typedef struct moronet8_session moronet8_session;
typedef struct moronet8_api moronet8_api;
typedef struct moronet8_cart moronet8_cart;
typedef struct moronet8_cart_tileset moronet8_cart_tileset;
typedef struct moronet8_cart_sprite moronet8_cart_sprite;
typedef struct moronet8_cart_header moronet8_cart_header;
typedef struct moronet8_cart_data moronet8_cart_data;
typedef struct moronet8 moronet8;

#ifdef HAVE_MALLOC
#if defined(_MSC_VER)
/* work around MSVC error C2322: '...' address of dllimport '...' is not static */
static void *MORONET8_CDECL internal_malloc(size_t size)
{
    return MORONET8_MALLOC(size);
}

static void MORONET8_CDECL internal_free(void *pointer)
{
    MORONET8_FREE(pointer);
}
#else
#define internal_malloc MORONET8_MALLOC
#define internal_free MORONET8_FREE
#endif
#else
#define internal_malloc NULL
#define internal_free NULL
#endif

#ifdef HAVE_PRINTF
#if defined(_MSC_VER)
/* work around MSVC error C2322: '...' address of dllimport '...' is not static */
static void MORONET8_CDECL internal_printf(const char *fmt, ...)
{
    printf(fmt, ...);
}
#else
#define internal_printf MORONET8_PRINTF
#endif
#else
#define internal_printf NULL
#endif

static moronet8_hooks moronet8_global_hooks = {
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

MORONET8_PUBLIC(void)
moronet8_init_hooks(moronet8_hooks *hooks)
{
    if (hooks->malloc_fn)
        moronet8_global_hooks.malloc_fn = hooks->malloc_fn;
    if (hooks->free_fn)
        moronet8_global_hooks.free_fn = hooks->free_fn;
    if (hooks->printf_fn)
        moronet8_global_hooks.printf_fn = hooks->printf_fn;
    if (hooks->host_session_fn)
        moronet8_global_hooks.host_session_fn = hooks->host_session_fn;
    if (hooks->join_session_fn)
        moronet8_global_hooks.join_session_fn = hooks->join_session_fn;
    if (hooks->delete_session_fn)
        moronet8_global_hooks.delete_session_fn = hooks->delete_session_fn;
    if (hooks->broadcast_session_fn)
        moronet8_global_hooks.broadcast_session_fn = hooks->broadcast_session_fn;
    if (hooks->receive_session_fn)
        moronet8_global_hooks.receive_session_fn = hooks->receive_session_fn;
    if (hooks->session_state_get_fn)
        moronet8_global_hooks.session_state_get_fn = hooks->session_state_get_fn;
    if (hooks->poll_session_fn)
        moronet8_global_hooks.poll_session_fn = hooks->poll_session_fn;
}

MORONET8_PUBLIC(void)
moronet8_reset_hooks(void)
{
    moronet8_global_hooks.malloc_fn = NULL;
    moronet8_global_hooks.free_fn = NULL;
    moronet8_global_hooks.printf_fn = NULL;
    moronet8_global_hooks.host_session_fn = NULL;
    moronet8_global_hooks.join_session_fn = NULL;
    moronet8_global_hooks.delete_session_fn = NULL;
    moronet8_global_hooks.broadcast_session_fn = NULL;
    moronet8_global_hooks.receive_session_fn = NULL;
    moronet8_global_hooks.session_state_get_fn = NULL;
    moronet8_global_hooks.poll_session_fn = NULL;
}

MORONET8_PUBLIC(moronet8_hooks *)
moronet8_get_hooks(void)
{
    return &moronet8_global_hooks;
}

MORONET8_PUBLIC(void *)
moronet8_malloc(size_t size)
{
    if (!moronet8_global_hooks.malloc_fn)
    {
        return NULL;
    }

    return moronet8_global_hooks.malloc_fn(size);
}

MORONET8_PUBLIC(void)
moronet8_free(void *p)
{
    if (moronet8_global_hooks.free_fn)
    {
        moronet8_global_hooks.free_fn(p);
    }
}

MORONET8_PUBLIC(void)
moronet8_printf(const char *fmt, ...)
{
    if (moronet8_global_hooks.printf_fn)
    {
        va_list args;
        va_start(args, fmt);
        moronet8_global_hooks.printf_fn(fmt, args);
        va_end(args);
    }
}

MORONET8_PUBLIC(moronet8_session *)
moronet8_session_host(const char *host)
{
    if (!moronet8_global_hooks.host_session_fn)
    {
        return NULL;
    }

    return moronet8_global_hooks.host_session_fn(host);
}

MORONET8_PUBLIC(moronet8_session *)
moronet8_session_join(const char *host)
{
    if (!moronet8_global_hooks.join_session_fn)
    {
        return NULL;
    }

    return moronet8_global_hooks.join_session_fn(host);
}

MORONET8_PUBLIC(void)
moronet8_session_delete(moronet8_session *session)
{
    if (!moronet8_global_hooks.delete_session_fn)
    {
        return;
    }

    moronet8_global_hooks.delete_session_fn(session);
}

MORONET8_PUBLIC(void)
moronet8_session_broadcast(moronet8_session *session, const void *buf, size_t size)
{
    if (!moronet8_global_hooks.broadcast_session_fn)
    {
        return;
    }

    moronet8_global_hooks.broadcast_session_fn(session, buf, size);
}

MORONET8_PUBLIC(size_t)
moronet8_session_receive(moronet8_session *session, void *buf, size_t size)
{
    if (!moronet8_global_hooks.receive_session_fn)
    {
        return 0;
    }

    return moronet8_global_hooks.receive_session_fn(session, buf, size);
}

MORONET8_PUBLIC(enum moronet8_session_state)
moronet8_session_state_get(moronet8_session *session)
{
    if (!session || !moronet8_global_hooks.session_state_get_fn)
    {
        return MORONET8_SESSION_CLOSED;
    }

    return moronet8_global_hooks.session_state_get_fn(session);
}

MORONET8_PUBLIC(void)
moronet8_session_poll(moronet8_session *session)
{
    if (!moronet8_global_hooks.poll_session_fn)
    {
        return;
    }

    moronet8_global_hooks.poll_session_fn(session);
}

#define _MORONET8_MALLOC moronet8_global_hooks.malloc_fn
#define _MORONET8_FREE moronet8_global_hooks.free_fn
#define _MORONET8_PRINTF moronet8_global_hooks.printf_fn

#define MORONET8_TRUE 1
#define MORONET8_FALSE 0

MORONET8_PUBLIC(moronet8_api *)
moronet8_api_init(moronet8_api *api, moronet8 *vm, moronet8_lang lang, moronet8_api_type type)
{
    /** Delete old API instance */
    moronet8_api_delete(api);

    api->type = type;

    /** Create new API instance */
    switch (lang)
    {
#if MORONET8_LUA_API
    case MORONET8_LANG_LUA:
        return moronet8_lua_api_init(api, vm, type);
#endif
#if MORONET8_JS_API
    case MORONET8_LANG_JS:
        return moronet8_js_api_init(api, vm, type);
#endif
    default:
        return NULL;
    }
}

MORONET8_PUBLIC(void)
moronet8_api_delete(moronet8_api *api)
{
    if (api->free)
    {
        api->free(api);
    }

    memset(api, 0, sizeof(moronet8_api));
}

MORONET8_PUBLIC(moronet8_api *)
moronet8_api_load_string(moronet8_api *api, const char *buf, size_t size)
{
    return api->load_string ? api->load_string(api, buf, size) : NULL;
}

MORONET8_PUBLIC(moronet8_api *)
moronet8_api_tick(moronet8_api *api)
{
    return api->tick ? api->tick(api) : NULL;
}

MORONET8_PUBLIC(moronet8 *)
moronet8_create(void)
{
    moronet8 *o = (moronet8 *)_MORONET8_MALLOC(sizeof(moronet8));
    if (o)
    {
        moronet8_init(o);
    }

    return o;
}

MORONET8_PUBLIC(void)
moronet8_init(moronet8 *vm)
{
    memset(vm, 0, sizeof(moronet8));
    vm->state = MORONET8_STATE_BIOS;
}

MORONET8_PUBLIC(void)
moronet8_delete(moronet8 *vm)
{
    moronet8_api_delete(&vm->cart_api);
    moronet8_api_delete(&vm->bios_api);
    _MORONET8_FREE(vm);
}

MORONET8_PUBLIC(size_t)
moronet8_color_format(void)
{
    return MORONET8_COLOR_FORMAT;
}

MORONET8_PUBLIC(size_t)
moronet8_sizeof(void)
{
    return sizeof(struct moronet8);
}

MORONET8_PUBLIC(void *)
moronet8_get_vram(struct moronet8 *vm)
{
    return vm->ram.vram;
}

MORONET8_PUBLIC(void)
moronet8_set_vram(struct moronet8 *vm, void *buffer)
{
    vm->ram.vram = buffer;
}

static moronet8 *moronet8_load_any(moronet8 *vm, moronet8_api *api, moronet8_cart_data *dst, moronet8_cart_data *src, moronet8_api_type type)
{
    memcpy(dst, src, sizeof(moronet8_cart_data));
    vm->cart_select = dst;

    if (!moronet8_api_init(api, vm, dst->code.lang, type))
    {
        return NULL;
    }

    if (!moronet8_api_load_string(api, &dst->code.text[0], MORONET8_CART_CODE_SIZE))
    {
        if (dst == &vm->cart)
            moronet8_printf("failed loads");
        return NULL;
    }

    if (dst == &vm->cart)
        moronet8_printf("api init");
    return vm;
}

static void moronet8_unload_any(moronet8_api *api, moronet8_cart_data *cart)
{
    moronet8_api_delete(api);
    memset(cart, 0, sizeof(moronet8_cart_data));
}

MORONET8_PUBLIC(moronet8 *)
moronet8_load_bios(moronet8 *vm, moronet8_cart_data *cart)
{
    return moronet8_load_any(vm, &vm->bios_api, &vm->bios, cart, MORONET8_API_CART | MORONET8_API_BIOS);
}

MORONET8_PUBLIC(moronet8 *)
moronet8_load_cart(moronet8 *vm, moronet8_cart_data *cart)
{
    return moronet8_load_any(vm, &vm->cart_api, &vm->cart, cart, MORONET8_API_CART);
}

MORONET8_PUBLIC(void)
moronet8_unload_bios(moronet8 *vm)
{
    moronet8_unload_any(&vm->bios_api, &vm->bios);
}

MORONET8_PUBLIC(void)
moronet8_unload_cart(moronet8 *vm)
{
    moronet8_unload_any(&vm->cart_api, &vm->cart);
}

MORONET8_PUBLIC(void)
moronet8_tick(moronet8 *vm, float dt)
{
    vm->ram.dt = dt;

    moronet8_session_state session_state = moronet8_session_state_get(vm->session);
    if (session_state == MORONET8_SESSION_JOINED)
    {
        vm->ram.netram_sp = moronet8_session_receive(
            vm->session,
            &vm->ram.netram,
            MORONET8_NETRAM_SIZE);
    }

    /* Only tick cart when in those states */
    if (vm->state == MORONET8_STATE_CART || vm->state == MORONET8_STATE_OVERLAY)
    {
        vm->cart_select = &vm->cart;
        moronet8_api_tick(&vm->cart_api);
    }

    /* Always tick bios */

    if (session_state == MORONET8_SESSION_HOSTING)
    {
        moronet8_session_broadcast(
            vm->session,
            &vm->ram.netram,
            vm->ram.netram_sp);
    }
}

static moronet8_cart_tileset *moronet8_selected_font(struct moronet8 *vm)
{
    assert(vm->cart_select);
    return &vm->cart_select->font;
}

static moronet8_cart_tileset *moronet8_selected_tileset(struct moronet8 *vm)
{
    assert(vm->cart_select);
    return &vm->cart_select->tileset;
}

MORONET8_PUBLIC(moronet8_u8)
moronet8_fontget(moronet8 *vm)
{
    return 0;
}

MORONET8_PUBLIC(void)
moronet8_fontset(moronet8 *vm, moronet8_u8 id)
{
    assert(vm->cart_select);
    // moronet8_cart_select_font(vm->cart_select, id);
}

MORONET8_PUBLIC(void)
moronet8_print(moronet8 *vm, const char *buf, size_t size, moronet8_s32 x, moronet8_s32 y, moronet8_u8 col)
{
    moronet8_printf(buf);
    for (size_t i = 0; i < size; ++i)
    {
        moronet8_printc(vm, buf[i], x + i * MORONET8_SPRITE_WIDTH, y, col);
    }
}

static void moronet8_set_pixel(moronet8 *vm, moronet8_s32 x, moronet8_s32 y, moronet8_u8 value, moronet8_u8 *col)
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

    struct moronet8_cart_color *c = &vm->cart_select->palette[value];

#if MORONET8_COLOR_FORMAT == MORONET8_COLOR_RGB565
    moronet8_u16 c16 = ((c->r & 0xf8) << 8) |
                       ((c->g & 0xfc) << 3) |
                       ((c->b & 0xf8) >> 3);

    ((moronet8_u16 *)vm->ram.vram)[x + y * MORONET8_SCREEN_WIDTH] = __builtin_bswap16(c16);
#elif MORONET8_COLOR_FORMAT == MORONET8_COLOR_RGBA
    moronet8_u32 c32 = ((c->r & 0xf8) << 24) |
                       ((c->g & 0xfc) << 16) |
                       ((c->b & 0xf8) >> 8) |
                       0xFF;

    ((moronet8_u32 *)vm->ram.vram)[x + y * MORONET8_SCREEN_WIDTH] = c32;
#endif
}

static void moronet8_draw_sprite(moronet8 *vm, moronet8_cart_tileset *tileset, moronet8_u8 id, moronet8_s32 x, moronet8_s32 y, moronet8_u8 *col)
{
    if (!tileset)
    {
        return;
    }

    moronet8_cart_sprite *sprite = &tileset->sprites[id];
    for (moronet8_u8 j = 0; j < MORONET8_SPRITE_HEIGHT; ++j)
    {
        for (moronet8_u8 i = 0; i < MORONET8_SPRITE_WIDTH; ++i)
        {
            moronet8_set_pixel(vm, x + i, y + j, sprite->pixels[i + j * MORONET8_SPRITE_WIDTH], col);
        }
    }
}

static void moronet8_draw_sprite_wh(moronet8 *vm, moronet8_cart_tileset *tileset, moronet8_u8 id, moronet8_s32 x, moronet8_s32 y, moronet8_u8 w, moronet8_u8 h)
{
    if (!tileset)
    {
        return;
    }

    for (moronet8_u8 j = 0; j < h; j++)
    {
        for (moronet8_u8 i = 0; i < w; i++)
        {
            moronet8_draw_sprite(
                vm,
                tileset,
                id + i + j * MORONET8_TILESET_WIDTH,
                x + i * MORONET8_SPRITE_WIDTH,
                y + j * MORONET8_SPRITE_HEIGHT,
                NULL);
        }
    }
}

MORONET8_PUBLIC(void)
moronet8_printc(struct moronet8 *vm, char c, moronet8_s32 x, moronet8_s32 y, moronet8_u8 col)
{
    moronet8_draw_sprite(vm, moronet8_selected_font(vm), (moronet8_u8)c, x, y, &col);
}

MORONET8_PUBLIC(int)
moronet8_btn(moronet8 *vm, moronet8_button button, moronet8_u8 player)
{
    return ((moronet8_u8 *)&vm->ram.buttons)[player] & (1 << button);
}

MORONET8_PUBLIC(int)
moronet8_btnp(moronet8 *vm, moronet8_button button, moronet8_u8 player)
{
    return moronet8_btn(vm, button, player) != 0;
}

MORONET8_PUBLIC(float)
moronet8_dt(moronet8 *vm)
{
    return vm->ram.dt;
}

MORONET8_PUBLIC(float)
moronet8_sin(struct moronet8 *vm, float val)
{
    return (float)sin(val);
}

MORONET8_PUBLIC(float)
moronet8_cos(struct moronet8 *vm, float val)
{
    return (float)cos(val);
}

MORONET8_PUBLIC(void)
moronet8_cls(moronet8 *vm)
{
    memset(vm->ram.vram, 0, MORONET8_VRAM_SIZE);
}

MORONET8_PUBLIC(moronet8_u8)
moronet8_colorget(moronet8 *vm)
{
    return vm->ram.color;
}

MORONET8_PUBLIC(void)
moronet8_colorset(moronet8 *vm, moronet8_u8 col)
{
    vm->ram.color = col;
}

#define MORONET8_SWAP(a, b, type) \
    do                            \
    {                             \
        type temp = a;            \
        a = b;                    \
        b = temp;                 \
    } while (0)

static float moronet8_init_line(moronet8_s32 *x0, moronet8_s32 *x1, moronet8_s32 *y0, moronet8_s32 *y1)
{
    if (*y0 > *y1)
    {
        MORONET8_SWAP(*x0, *x1, moronet8_s32);
        MORONET8_SWAP(*y0, *y1, moronet8_s32);
    }

    float t = (*x1 - *x0) / (float)(*y1 - *y0);

    if (*y0 < 0)
        *x0 -= *y0 * t, *y0 = 0;
    if (*y1 > MORONET8_SCREEN_WIDTH)
        *x1 += (MORONET8_SCREEN_WIDTH - *y0) * t, *y1 = MORONET8_SCREEN_WIDTH;

    return t;
}

MORONET8_PUBLIC(void)
moronet8_line(moronet8 *vm, moronet8_s32 x0, moronet8_s32 y0, moronet8_s32 x1, moronet8_s32 y1, moronet8_u8 col)
{
    if (abs(x0 - x1) < abs(y0 - y1))
        for (float t = moronet8_init_line(&x0, &x1, &y0, &y1); y0 < y1; y0++, x0 += t)
            moronet8_pset(vm, x0, y0, col);
    else
        for (float t = moronet8_init_line(&y0, &y1, &x0, &x1); x0 < x1; x0++, y0 += t)
            moronet8_pset(vm, x0, y0, col);

    moronet8_pset(vm, x1, y1, col);
}

MORONET8_PUBLIC(void)
moronet8_rect(moronet8 *vm, moronet8_s32 x0, moronet8_s32 y0, moronet8_s32 x1, moronet8_s32 y1, moronet8_u8 col)
{
    moronet8_line(vm, x0, y0, x1, y0, col);
    moronet8_line(vm, x0, y1, x1, y1, col);
    moronet8_line(vm, x0, y0, x0, y1, col);
    moronet8_line(vm, x1, y0, x1, y1, col);
}

MORONET8_PUBLIC(void)
moronet8_rectfill(moronet8 *vm, moronet8_s32 x0, moronet8_s32 y0, moronet8_s32 x1, moronet8_s32 y1, moronet8_u8 col)
{
    for (moronet8_s32 i = y0; i <= y1; ++i)
    {
        moronet8_line(vm, x0, i, x1, i, col);
    }
}

MORONET8_PUBLIC(moronet8_u8)
moronet8_tilesetget(moronet8 *vm)
{
    return 0;
}

MORONET8_PUBLIC(void)
moronet8_tilesetset(moronet8 *vm, moronet8_u8 id)
{
    assert(vm->cart_select);
    // moronet8_cart_select_tileset(vm->cart_select, id);
}

MORONET8_PUBLIC(void)
moronet8_spr(moronet8 *vm, moronet8_u8 id, moronet8_s32 x, moronet8_s32 y, moronet8_u8 w, moronet8_u8 h)
{
    moronet8_draw_sprite_wh(vm, moronet8_selected_tileset(vm), id, x, y, w, h);
}

MORONET8_PUBLIC(moronet8_s32)
moronet8_paltget(moronet8 *vm, moronet8_u8 col)
{
    assert(vm->cart_select);
    return vm->cart_select->palette[col].t;
}

MORONET8_PUBLIC(void)
moronet8_paltset(moronet8 *vm, moronet8_u8 col, moronet8_s32 t)
{
    assert(vm->cart_select);
    vm->cart_select->palette[col].t = ((moronet8_u8)t != 0);
}

MORONET8_PUBLIC(moronet8_u8)
moronet8_codeget(moronet8 *vm)
{
    return 0;
}

MORONET8_PUBLIC(void)
moronet8_codeset(moronet8 *vm, moronet8_u8 id)
{
    assert(vm->cart_select);
    // moronet8_cart_select_code(vm->cart_select, id);
}

MORONET8_PUBLIC(moronet8_u8)
moronet8_peek(moronet8 *vm, moronet8_u16 address)
{
    return ((moronet8_u8 *)&vm->ram.vram)[address];
}

MORONET8_PUBLIC(void)
moronet8_poke(moronet8 *vm, moronet8_u16 address, moronet8_u8 value)
{
    ((moronet8_u8 *)&vm->ram.vram)[address] = value;
}

MORONET8_PUBLIC(moronet8_u8)
moronet8_pget(struct moronet8 *vm, moronet8_s32 x, moronet8_s32 y)
{
    return moronet8_peek(vm, x + ((moronet8_u16)y) * MORONET8_SCREEN_WIDTH);
}

MORONET8_PUBLIC(void)
moronet8_pset(moronet8 *vm, moronet8_s32 x, moronet8_s32 y, moronet8_u8 value)
{
    assert(vm->cart_select);
    if (vm->cart_select->palette[value].t)
    {
        return;
    }

    moronet8_poke(vm, x + ((moronet8_u16)y) * MORONET8_SCREEN_WIDTH, value);
}

MORONET8_PUBLIC(moronet8_netram_sp)
moronet8_netspget(moronet8 *vm)
{
    return vm->ram.netram_sp;
}

MORONET8_PUBLIC(void)
moronet8_netspset(moronet8 *vm, moronet8_netram_sp offset)
{
    vm->ram.netram_sp = offset;
}

#define MORONET8_STACK_PUSH(stack, sp, size, value) \
    stack[sp] = value;                              \
    if (sp < size - 1)                              \
    {                                               \
        sp++;                                       \
    }

#define MORONET8_STACK_POP(stack, sp, size) \
    if (sp > 0)                             \
    {                                       \
        sp--;                               \
    }                                       \
    return stack[sp]

MORONET8_PUBLIC(int)
moronet8_nethost(moronet8 *vm)
{
    return moronet8_session_state_get(vm->session) == MORONET8_SESSION_HOSTING;
}

MORONET8_PUBLIC(int)
moronet8_netclient(moronet8 *vm)
{
    return moronet8_session_state_get(vm->session) == MORONET8_SESSION_JOINED;
}

MORONET8_PUBLIC(void)
moronet8_netpush(moronet8 *vm, moronet8_u8 value)
{
    MORONET8_STACK_PUSH(vm->ram.netram, vm->ram.netram_sp, MORONET8_NETRAM_SIZE, value);
}

MORONET8_PUBLIC(moronet8_u8)
moronet8_netpop(moronet8 *vm)
{
    moronet8_u8 value = vm->ram.netram[vm->ram.netram_sp];
    if (vm->ram.netram_sp < MORONET8_NETRAM_SIZE - 1)
    {
        vm->ram.netram_sp++;
    }

    return value;
}

MORONET8_PUBLIC(moronet8_state)
moronet8_state_get(moronet8 *vm)
{
    return vm->state;
}

MORONET8_PUBLIC(void)
moronet8_state_set(moronet8 *vm, moronet8_state state)
{
    switch (state)
    {
    case MORONET8_STATE_BIOS:
        moronet8_printf("state BIOS\n");
        break;
    case MORONET8_STATE_CART:
        moronet8_printf("state CART\n");
        break;
    case MORONET8_STATE_OVERLAY:
        moronet8_printf("state OVERLAY\n");
        break;
    default:
        break;
    }
    vm->state = state;
}

MORONET8_PUBLIC(void)
moronet8_load(moronet8 *vm, const char *cart)
{
#ifdef MORONET8_FILESYSTEM
    if (vm->bios_api.on_cart_loading)
    {
        vm->bios_api.on_cart_loading(&vm->bios_api);
    }

    moronet8_cart c;
    moronet8_cart_load_dir(&c, cart);

    moronet8_load_cart(vm, &c.data);

    if (vm->bios_api.on_cart_loaded)
    {
        vm->bios_api.on_cart_loaded(&vm->bios_api);
    }
#else
    moronet8_printf("load not supported on this platform\n");
#endif
}

MORONET8_PUBLIC(moronet8_session_state)
moronet8_netsessionstate(moronet8 *vm)
{
    return vm->session ? moronet8_session_state_get(vm->session) : MORONET8_SESSION_CLOSED;
}

MORONET8_PUBLIC(void)
moronet8_netsessionstart(moronet8 *vm)
{
    moronet8_netsessionleave(vm);

    moronet8_printf("start net session\n");
    vm->session = moronet8_session_host("127.0.0.1");
}

MORONET8_PUBLIC(void)
moronet8_netsessionjoin(moronet8 *vm, const char *host)
{
    moronet8_netsessionleave(vm);

    moronet8_printf("join net session %s\n", host);
    vm->session = moronet8_session_join(host);
}

MORONET8_PUBLIC(void)
moronet8_netsessionleave(moronet8 *vm)
{
    moronet8_printf("leave net session\n");
    if (!vm->session)
    {
        return;
    }

    moronet8_session_delete(vm->session);
    vm->session = NULL;
}

MORONET8_PUBLIC(void)
moronet8_netsessionpoll(moronet8 *vm)
{
    if (!vm->session)
    {
        return;
    }

    moronet8_session_poll(vm->session);
}

MORONET8_PUBLIC(moronet8_pktram_sp)
moronet8_pktspget(moronet8 *vm)
{
    return vm->ram.pktram_sp;
}

MORONET8_PUBLIC(void)
moronet8_pktspset(moronet8 *vm, moronet8_pktram_sp offset)
{
    vm->ram.pktram_sp = offset;
}

MORONET8_PUBLIC(void)
moronet8_pktpush(moronet8 *vm, moronet8_u8 value)
{
    MORONET8_STACK_PUSH(vm->ram.pktram, vm->ram.pktram_sp, MORONET8_PKTRAM_SIZE, value);
}

MORONET8_PUBLIC(moronet8_u8)
moronet8_pktpop(moronet8 *vm)
{
    MORONET8_STACK_POP(vm->ram.pktram, vm->ram.pktram_sp, MORONET8_PKTRAM_SIZE);
}

MORONET8_PUBLIC(void)
moronet8_pktsend(moronet8 *vm)
{
}

#undef MORONET8_STACK_PUSH
#undef MORONET8_STACK_POP
