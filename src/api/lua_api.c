#include "morobox8.h"

#if MOROBOX8_LUA_API

#include "api/lua_api.h"
#include "api/api_macro.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

#define MOROBOX8_TICK_FN "tick"

typedef enum morobox8_state morobox8_state;
typedef enum morobox8_api_type morobox8_api_type;
typedef struct morobox8_api morobox8_api;
typedef struct morobox8 morobox8;

static morobox8 *morobox8_lua_get_vm(lua_State *lua)
{
    return (morobox8 *)lua_touserdata(lua, lua_upvalueindex(1));
}

#define MOROBOX8_LUA_PUSH_NUMBER(lua, value) lua_pushnumber(lua, (lua_Number)value)
#define MOROBOX8_LUA_PUSH_BOOL(lua, value) lua_pushboolean(lua, (int)value)
#define MOROBOX8_LUA_GET_NUMBER(lua, index) ((int)lua_tonumber(lua, index))
#define MOROBOX8_LUA_GET_BOOL(lua, index) lua_toboolean(lua, index)
#define MOROBOX8_LUA_GET_U8(lua, index) ((morobox8_u8)lua_tonumber(lua, index))
#define MOROBOX8_LUA_GET_S8(lua, index) ((morobox8_s8)lua_tonumber(lua, index))
#define MOROBOX8_LUA_GET_U16(lua, index) ((morobox8_u16)lua_tonumber(lua, index))
#define MOROBOX8_LUA_GET_S16(lua, index) ((morobox8_s16)lua_tonumber(lua, index))
#define MOROBOX8_LUA_GET_U32(lua, index) ((morobox8_u32)lua_tonumber(lua, index))
#define MOROBOX8_LUA_GET_S32(lua, index) ((morobox8_s32)lua_tonumber(lua, index))
#define MOROBOX8_LUA_GET_U64(lua, index) ((morobox8_u64)lua_tonumber(lua, index))
#define MOROBOX8_LUA_GET_S64(lua, index) ((morobox8_s64)lua_tonumber(lua, index))
#define MOROBOX8_LUA_GET_FLOAT(lua, index) ((float)lua_tonumber(lua, index))

static inline void morobox8_lua_error(lua_State *lua, const char *fmt)
{
    morobox8_printf(fmt);
}

static inline const char *morobox8_lua_get_string(lua_State *lua, int index, size_t *size)
{
    return luaL_tolstring(lua, index, size);
}

static inline void morobox8_lua_push_word(lua_State *lua, morobox8_u8 value)
{
    lua_pushnumber(lua, (lua_Number)value);
}

static void morobox8_lua_api_register_const(lua_State *lua, morobox8 *vm, lua_Number value, const char *name)
{
    lua_pushlightuserdata(lua, vm);
    lua_pushnumber(lua, value);
    lua_setglobal(lua, name);
}

static void morobox8_lua_api_register_fun(lua_State *lua, morobox8 *vm, lua_CFunction func, const char *name)
{
    lua_pushlightuserdata(lua, vm);
    lua_pushcclosure(lua, func, 1);
    lua_setglobal(lua, name);
}

static int morobox8_lua_api_msghandler(lua_State *lua)
{
    const char *msg = lua_tostring(lua, 1);
    if (msg == NULL) /* is error object not a string? */
    {
        if (luaL_callmeta(lua, 1, "__tostring") && /* does it have a metamethod */
            lua_type(lua, -1) == LUA_TSTRING)      /* that produces a string? */
            return 1;                              /* that is the message */
        else
            msg = lua_pushfstring(lua, "(error object is a %s value)", luaL_typename(lua, 1));
    }
    luaL_traceback(lua, lua, msg, 1); /* append a standard traceback */
    return 1;                         /* return the traceback */
}

static int morobox8_lua_api_do_call(lua_State *lua, int narg, int nres)
{
    int status = 0;
    int base = lua_gettop(lua) - narg;                   /* function index */
    lua_pushcfunction(lua, morobox8_lua_api_msghandler); /* push message handler */
    lua_insert(lua, base);                               /* put it under function and args */
    status = lua_pcall(lua, narg, nres, base);
    lua_remove(lua, base); /* remove message handler from the stack */
    return status;
}

static int morobox8_lua_api_font(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            MOROBOX8_LUA_PUSH_NUMBER(
                lua,
                morobox8_fontget(vm));
            return 1;
        }

        morobox8_fontset(vm, MOROBOX8_LUA_GET_U8(lua, 1));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, font([id])\n");

    return 0;
}

static int morobox8_lua_api_print(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 5)
    {
        size_t size;
        const char *s = morobox8_lua_get_string(lua, 1, &size);

        morobox8_print(
            vm,
            s,
            size,
            top < 2 ? 0 : MOROBOX8_LUA_GET_S32(lua, 2),
            top < 3 ? 0 : MOROBOX8_LUA_GET_S32(lua, 3),
            top < 4 ? morobox8_colorget(vm) : MOROBOX8_LUA_GET_U8(lua, 4));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, print(msg, [x, y, col])\n");

    return 0;
}

static int morobox8_lua_api_btn(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 3)
    {
        MOROBOX8_LUA_PUSH_NUMBER(
            lua,
            morobox8_btn(
                vm,
                top < 1 ? MOROBOX8_BUTTON_LEFT : MOROBOX8_LUA_GET_U8(lua, 1),
                top < 2 ? 0 : MOROBOX8_LUA_GET_U8(lua, 2)));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, btn([id], [player])\n");

    return 0;
}

static int morobox8_lua_api_btnp(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 3)
    {
        MOROBOX8_LUA_PUSH_BOOL(
            lua,
            morobox8_btnp(
                vm,
                top < 1 ? MOROBOX8_BUTTON_LEFT : MOROBOX8_LUA_GET_U8(lua, 1),
                top < 2 ? 0 : MOROBOX8_LUA_GET_U8(lua, 2)));
        return 1;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, btn([id], [player])\n");

    return 0;
}

static int morobox8_lua_api_dt(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 0)
    {
        MOROBOX8_LUA_PUSH_NUMBER(lua, morobox8_dt(vm));
        return 1;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, dt()\n");

    return 0;
}

static int morobox8_lua_api_sin(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 1)
    {
        MOROBOX8_LUA_PUSH_NUMBER(lua, morobox8_sin(vm, MOROBOX8_LUA_GET_FLOAT(lua, 1)));
        return 1;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, sin(val)\n");

    return 0;
}

static int morobox8_lua_api_cos(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 1)
    {
        MOROBOX8_LUA_PUSH_NUMBER(lua, morobox8_cos(vm, MOROBOX8_LUA_GET_FLOAT(lua, 1)));
        return 1;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, cos(val)\n");

    return 0;
}

static int morobox8_lua_api_cls(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 0)
    {
        morobox8_cls(vm);
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, cls()\n");

    return 0;
}

static int morobox8_lua_api_color(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            morobox8_lua_push_word(
                lua,
                morobox8_colorget(vm));
            return 1;
        }

        morobox8_colorset(vm, MOROBOX8_LUA_GET_U8(lua, 1));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, color([col])\n");

    return 0;
}

static int morobox8_lua_api_line(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 6)
    {
        morobox8_line(
            vm,
            MOROBOX8_LUA_GET_S32(lua, 1),
            MOROBOX8_LUA_GET_S32(lua, 2),
            MOROBOX8_LUA_GET_S32(lua, 3),
            MOROBOX8_LUA_GET_S32(lua, 4),
            top < 5 ? morobox8_colorget(vm) : MOROBOX8_LUA_GET_U8(lua, 5));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, line(x0, y0, x1, y1, [col])\n");

    return 0;
}

static int morobox8_lua_api_rect(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 6)
    {
        morobox8_rect(
            vm,
            MOROBOX8_LUA_GET_S32(lua, 1),
            MOROBOX8_LUA_GET_S32(lua, 2),
            MOROBOX8_LUA_GET_S32(lua, 3),
            MOROBOX8_LUA_GET_S32(lua, 4),
            top < 5 ? morobox8_colorget(vm) : MOROBOX8_LUA_GET_U8(lua, 5));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, rect(x0, y0, x1, y1, [col])\n");

    return 0;
}

static int morobox8_lua_api_rectfill(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 6)
    {
        morobox8_rectfill(
            vm,
            MOROBOX8_LUA_GET_S32(lua, 1),
            MOROBOX8_LUA_GET_S32(lua, 2),
            MOROBOX8_LUA_GET_S32(lua, 3),
            MOROBOX8_LUA_GET_S32(lua, 4),
            top < 5 ? morobox8_colorget(vm) : MOROBOX8_LUA_GET_U8(lua, 5));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, rectfill(x0, y0, x1, y1, [col])\n");

    return 0;
}

static int morobox8_lua_api_tileset(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            MOROBOX8_LUA_PUSH_NUMBER(
                lua,
                morobox8_tilesetget(vm));
            return 1;
        }

        morobox8_tilesetset(vm, MOROBOX8_LUA_GET_U8(lua, 1));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, tileset([id])\n");

    return 0;
}

static int morobox8_lua_api_spr(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 6)
    {
        morobox8_spr(
            vm,
            MOROBOX8_LUA_GET_U8(lua, 1),
            MOROBOX8_LUA_GET_S32(lua, 2),
            MOROBOX8_LUA_GET_S32(lua, 3),
            top < 4 ? 1 : MOROBOX8_LUA_GET_U8(lua, 4),
            top < 5 ? 1 : MOROBOX8_LUA_GET_U8(lua, 5));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, spr(id, x, y, [w, h])\n");

    return 0;
}

static int morobox8_lua_api_palt(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 3)
    {
        if (top == 1)
        {
            MOROBOX8_LUA_PUSH_BOOL(
                lua,
                morobox8_paltget(
                    vm,
                    MOROBOX8_LUA_GET_U8(lua, 1)));
            return 1;
        }

        morobox8_paltset(
            vm,
            MOROBOX8_LUA_GET_U8(lua, 1),
            MOROBOX8_LUA_GET_BOOL(lua, 2));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, palt(col, [t])\n");

    return 0;
}

static int morobox8_lua_api_code(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            MOROBOX8_LUA_PUSH_NUMBER(
                lua,
                morobox8_codeget(vm));
            return 1;
        }

        morobox8_codeset(vm, MOROBOX8_LUA_GET_U8(lua, 1));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, code([id])\n");

    return 0;
}

static int morobox8_lua_api_peek(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 1)
    {
        morobox8_lua_push_word(
            lua,
            morobox8_peek(vm, MOROBOX8_LUA_GET_NUMBER(lua, 1)));
        return 1;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, peek(addr)\n");

    return 0;
}

static int morobox8_lua_api_poke(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 2)
    {
        morobox8_poke(vm, MOROBOX8_LUA_GET_NUMBER(lua, 1), MOROBOX8_LUA_GET_U8(lua, 2));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, peek(addr, val)\n");

    return 0;
}

static int morobox8_lua_api_pget(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 2)
    {
        morobox8_lua_push_word(
            lua,
            morobox8_pget(
                vm,
                MOROBOX8_LUA_GET_NUMBER(lua, 1),
                MOROBOX8_LUA_GET_NUMBER(lua, 2)));
        return 1;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, pget(x, y)\n");

    return 0;
}

static int morobox8_lua_api_pset(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 3)
    {
        morobox8_pset(
            vm,
            MOROBOX8_LUA_GET_S32(lua, 1),
            MOROBOX8_LUA_GET_S32(lua, 2),
            MOROBOX8_LUA_GET_U8(lua, 3));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, pset(x, y, val)\n");

    return 0;
}

#define MOROBOX8_LUA_API_STACK_SP(name, type)                                 \
    int top = lua_gettop(lua);                                                \
    morobox8 *vm = morobox8_lua_get_vm(lua);                                  \
                                                                              \
    if (top < 2)                                                              \
    {                                                                         \
        if (top == 0)                                                         \
        {                                                                     \
            MOROBOX8_LUA_PUSH_NUMBER(                                         \
                lua,                                                          \
                (int)morobox8_##name##get(vm));                               \
            return 1;                                                         \
        }                                                                     \
                                                                              \
        morobox8_##name##set(                                                 \
            vm,                                                               \
            (type)MOROBOX8_LUA_GET_NUMBER(lua, 1));                           \
        return 0;                                                             \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        morobox8_lua_error(lua, "invalid parameters, " #name "([offset])\n"); \
    }                                                                         \
    return 0;

#define MOROBOX8_LUA_API_STACK_PUSH(name)                                \
    int top = lua_gettop(lua);                                           \
    morobox8 *vm = morobox8_lua_get_vm(lua);                             \
                                                                         \
    if (top == 1)                                                        \
    {                                                                    \
        morobox8_##name(vm, MOROBOX8_LUA_GET_U8(lua, 1));                \
        return 0;                                                        \
    }                                                                    \
    else                                                                 \
    {                                                                    \
        morobox8_lua_error(lua, "invalid parameters, " #name "(val)\n"); \
    }                                                                    \
    return 0;

#define MOROBOX8_LUA_API_STACK_POP(name)                              \
    int top = lua_gettop(lua);                                        \
    morobox8 *vm = morobox8_lua_get_vm(lua);                          \
                                                                      \
    if (top == 0)                                                     \
    {                                                                 \
        morobox8_lua_push_word(                                       \
            lua,                                                      \
            morobox8_##name(vm));                                     \
        return 1;                                                     \
    }                                                                 \
    else                                                              \
    {                                                                 \
        morobox8_lua_error(lua, "invalid parameters, " #name "()\n"); \
    }                                                                 \
    return 0;

static int morobox8_lua_api_nethost(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 0)
    {
        MOROBOX8_LUA_PUSH_BOOL(
            lua,
            morobox8_nethost(vm));
        return 1;
    }
    else
    {
        morobox8_lua_error(lua, "invalid parameters, nethost()\n");
    }

    return 0;
}

static int morobox8_lua_api_netclient(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 0)
    {
        MOROBOX8_LUA_PUSH_BOOL(
            lua,
            morobox8_netclient(vm));
        return 1;
    }
    else
    {
        morobox8_lua_error(lua, "invalid parameters, netclient()\n");
    }

    return 0;
}

static int morobox8_lua_api_netsp(lua_State *lua)
{
    MOROBOX8_LUA_API_STACK_SP(netsp, morobox8_netram_sp);
}

static int morobox8_lua_api_netpush(lua_State *lua)
{
    MOROBOX8_LUA_API_STACK_PUSH(netpush);
}

static int morobox8_lua_api_netpop(lua_State *lua)
{
    MOROBOX8_LUA_API_STACK_POP(netpop);
}

static int morobox8_lua_api_pktsp(lua_State *lua)
{
    MOROBOX8_LUA_API_STACK_SP(pktsp, morobox8_pktram_sp);
}

static int morobox8_lua_api_pktpush(lua_State *lua)
{
    MOROBOX8_LUA_API_STACK_PUSH(pktpush);
}

static int morobox8_lua_api_pktpop(lua_State *lua)
{
    MOROBOX8_LUA_API_STACK_POP(pktpop);
}

static int morobox8_lua_api_pktsend(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 0)
    {
        morobox8_pktsend(vm);
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, pktsend()\n");

    return 0;
}

static int morobox8_lua_api_state(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            MOROBOX8_LUA_PUSH_NUMBER(lua, (int)morobox8_state_get(vm));
            return 1;
        }

        morobox8_state_set(vm, (morobox8_state)MOROBOX8_LUA_GET_NUMBER(lua, 1));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, state([val])\n");

    return 0;
}

static int morobox8_lua_api_load(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 1)
    {
        size_t size;
        morobox8_load(vm, morobox8_lua_get_string(lua, 1, &size));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, load(cart)\n");

    return 0;
}

static int morobox8_lua_api_netsessionstate(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 0)
    {
        MOROBOX8_LUA_PUSH_NUMBER(lua, morobox8_netsessionstate(vm));
        return 1;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, netsessionstate()\n");

    return 0;
}

static int morobox8_lua_api_netsessionstart(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 0)
    {
        morobox8_netsessionstart(vm);
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, netsessionstart()\n");

    return 0;
}

static int morobox8_lua_api_netsessionjoin(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 1)
    {
        size_t size;
        morobox8_netsessionjoin(vm, morobox8_lua_get_string(lua, 1, &size));
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, netsessionjoin(host)\n");

    return 0;
}

static int morobox8_lua_api_netsessionleave(lua_State *lua)
{
    int top = lua_gettop(lua);
    morobox8 *vm = morobox8_lua_get_vm(lua);

    if (top == 0)
    {
        morobox8_netsessionleave(vm);
        return 0;
    }
    else
        morobox8_lua_error(lua, "invalid parameters, netsessionleave()\n");

    return 0;
}

#undef MOROBOX8_LUA_API_STACK_SP
#undef MOROBOX8_LUA_API_STACK_PUSH
#undef MOROBOX8_LUA_API_STACK_POP

static void morobox8_lua_api_init_lua(lua_State *lua, morobox8 *vm, morobox8_api_type type)
{
#define MOROBOX8_REGISTER_API(name, val, api_type, ...) \
    if ((api_type & type) != 0)                         \
        morobox8_lua_api_register_const(lua, vm, val, #name);
    MOROBOX8_API_CONST_LIST(MOROBOX8_REGISTER_API)
#undef MOROBOX8_REGISTER_API
#define MOROBOX8_REGISTER_API(name, api_type, ...) \
    if ((api_type & type) != 0)                    \
        morobox8_lua_api_register_fun(lua, vm, morobox8_lua_api_##name, #name);
    MOROBOX8_API_FUN_LIST(MOROBOX8_REGISTER_API)
#undef MOROBOX8_REGISTER_API
}

static morobox8_api *morobox8_lua_api_load_string(morobox8_api *api, const char *buf, size_t size)
{
    MOROBOX8_UNUSED(size);

    lua_State *lua = (lua_State *)api->state;
    if (!lua)
    {
        morobox8_printf("error in load_string: no lua state");
        return NULL;
    }

    lua_settop(lua, 0);

    if (luaL_loadstring(lua, buf) != LUA_OK)
    {
        morobox8_printf("error in load_string: ");
        morobox8_printf(lua_tostring(lua, -1));
        return NULL;
    }

    if (lua_pcall(lua, 0, LUA_MULTRET, 0) != LUA_OK)
    {
        morobox8_printf("error in load_string: ");
        morobox8_printf(lua_tostring(lua, -1));
        return NULL;
    }

    return api;
}

static morobox8_api *morobox8_lua_api_tick(morobox8_api *api)
{
    morobox8_printf("tick cart");
    lua_State *lua = (lua_State *)api->state;
    if (!lua)
    {
        morobox8_printf("no state");
        return NULL;
    }

    lua_getglobal(lua, MOROBOX8_TICK_FN);
    if (!lua_isfunction(lua, -1))
    {
        morobox8_lua_error(lua, "no tick");
        lua_pop(lua, 1);
        return NULL;
    }

    if (morobox8_lua_api_do_call(lua, 0, 0) != LUA_OK)
    {
        morobox8_lua_error(lua, lua_tostring(lua, -1));
        return NULL;
    }

    morobox8_lua_error(lua, "ticked");
    return api;
}

void morobox8_lua_api_on_unload(morobox8_api *api)
{
    lua_State *lua = (lua_State *)api->state;
    if (!lua)
    {
        return;
    }

    lua_getglobal(lua, "on_unload");
    if (!lua_isfunction(lua, -1))
    {
        lua_pop(lua, 1);
        return;
    }

    if (morobox8_lua_api_do_call(lua, 0, 0) != LUA_OK)
    {
        morobox8_printf("error in on_unload: %s", lua_tostring(lua, -1));
        return;
    }
}

void morobox8_lua_api_on_load(morobox8_api *api)
{
    lua_State *lua = (lua_State *)api->state;
    if (!lua)
    {
        return;
    }

    lua_getglobal(lua, "on_load");
    if (!lua_isfunction(lua, -1))
    {
        lua_pop(lua, 1);
        return;
    }

    if (morobox8_lua_api_do_call(lua, 0, 0) != LUA_OK)
    {
        morobox8_printf("error in on_load: %s", lua_tostring(lua, -1));
    }
}

static void morobox8_lua_api_free(morobox8_api *api)
{
    if (api->state)
    {
        lua_close((lua_State *)api->state);
    }
}

MOROBOX8_PUBLIC(morobox8_api *)
morobox8_lua_api_init(morobox8_api *api, morobox8 *vm, morobox8_api_type type)
{
    lua_State *lua = luaL_newstate();
    if (!lua)
    {
        morobox8_printf("error in api_init: no lua state");
        return NULL;
    }

    morobox8_lua_api_init_lua(lua, vm, type);

    api->state = (void *)lua;
    api->free = &morobox8_lua_api_free;
    api->load_string = &morobox8_lua_api_load_string;
    api->tick = &morobox8_lua_api_tick;
    api->on_unload = &morobox8_lua_api_on_unload;
    api->on_load = &morobox8_lua_api_on_load;

    return api;
}

const struct morobox8_api_config morobox8_lua_api_config =
    {
        .lang = MOROBOX8_API_LANG_LUA,
        .main = "main.lua",
        .main_size = 8,
        .ext = ".lua",
        .ext_size = 4};

#endif
