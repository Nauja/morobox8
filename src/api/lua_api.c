#include "moronet8.h"

#if MORONET8_LUA_API

#include "api/lua_api.h"
#include "api/api_macro.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

#define MORONET8_TICK_FN "tick"

typedef enum moronet8_state moronet8_state;
typedef enum moronet8_api_type moronet8_api_type;
typedef struct moronet8_api moronet8_api;
typedef struct moronet8 moronet8;

static moronet8 *moronet8_lua_get_vm(lua_State *lua)
{
    return (moronet8 *)lua_touserdata(lua, lua_upvalueindex(1));
}

#define MORONET8_LUA_PUSH_NUMBER(lua, value) lua_pushnumber(lua, (lua_Number)value)
#define MORONET8_LUA_PUSH_BOOL(lua, value) lua_pushboolean(lua, (int)value)
#define MORONET8_LUA_GET_NUMBER(lua, index) ((int)lua_tonumber(lua, index))
#define MORONET8_LUA_GET_BOOL(lua, index) lua_toboolean(lua, index)
#define MORONET8_LUA_GET_U8(lua, index) ((moronet8_u8)lua_tonumber(lua, index))
#define MORONET8_LUA_GET_S8(lua, index) ((moronet8_s8)lua_tonumber(lua, index))
#define MORONET8_LUA_GET_U16(lua, index) ((moronet8_u16)lua_tonumber(lua, index))
#define MORONET8_LUA_GET_S16(lua, index) ((moronet8_s16)lua_tonumber(lua, index))
#define MORONET8_LUA_GET_U32(lua, index) ((moronet8_u32)lua_tonumber(lua, index))
#define MORONET8_LUA_GET_S32(lua, index) ((moronet8_s32)lua_tonumber(lua, index))
#define MORONET8_LUA_GET_U64(lua, index) ((moronet8_u64)lua_tonumber(lua, index))
#define MORONET8_LUA_GET_S64(lua, index) ((moronet8_s64)lua_tonumber(lua, index))
#define MORONET8_LUA_GET_FLOAT(lua, index) ((float)lua_tonumber(lua, index))

static inline const char *moronet8_lua_get_string(lua_State *lua, int index, size_t *size)
{
    return luaL_tolstring(lua, index, size);
}

static inline void moronet8_lua_push_word(lua_State *lua, moronet8_u8 value)
{
    lua_pushnumber(lua, (lua_Number)value);
}

static void moronet8_lua_api_register_const(lua_State *lua, moronet8 *vm, lua_Number value, const char *name)
{
    lua_pushlightuserdata(lua, vm);
    lua_pushnumber(lua, value);
    lua_setglobal(lua, name);
}

static void moronet8_lua_api_register_fun(lua_State *lua, moronet8 *vm, lua_CFunction func, const char *name)
{
    lua_pushlightuserdata(lua, vm);
    lua_pushcclosure(lua, func, 1);
    lua_setglobal(lua, name);
}

static int moronet8_lua_api_msghandler(lua_State *lua)
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

static int moronet8_lua_api_do_call(lua_State *lua, int narg, int nres)
{
    int status = 0;
    int base = lua_gettop(lua) - narg;                   /* function index */
    lua_pushcfunction(lua, moronet8_lua_api_msghandler); /* push message handler */
    lua_insert(lua, base);                               /* put it under function and args */
    status = lua_pcall(lua, narg, nres, base);
    lua_remove(lua, base); /* remove message handler from the stack */
    return status;
}

static int moronet8_lua_api_font(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            MORONET8_LUA_PUSH_NUMBER(
                lua,
                moronet8_fontget(vm));
            return 1;
        }

        moronet8_fontset(vm, MORONET8_LUA_GET_U8(lua, 1));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, font([id])\n");

    return 0;
}

static int moronet8_lua_api_print(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 5)
    {
        size_t size;
        const char *s = moronet8_lua_get_string(lua, 1, &size);

        moronet8_print(
            vm,
            s,
            size,
            top < 2 ? 0 : MORONET8_LUA_GET_S32(lua, 2),
            top < 3 ? 0 : MORONET8_LUA_GET_S32(lua, 3),
            top < 4 ? moronet8_colorget(vm) : MORONET8_LUA_GET_U8(lua, 4));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, print(msg, [x, y, col])\n");

    return 0;
}

static int moronet8_lua_api_btn(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 3)
    {
        MORONET8_LUA_PUSH_NUMBER(
            lua,
            moronet8_btn(
                vm,
                top < 1 ? MORONET8_BUTTON_LEFT : MORONET8_LUA_GET_U8(lua, 1),
                top < 2 ? 0 : MORONET8_LUA_GET_U8(lua, 2)));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, btn([id], [player])\n");

    return 0;
}

static int moronet8_lua_api_btnp(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 3)
    {
        MORONET8_LUA_PUSH_BOOL(
            lua,
            moronet8_btnp(
                vm,
                top < 1 ? MORONET8_BUTTON_LEFT : MORONET8_LUA_GET_U8(lua, 1),
                top < 2 ? 0 : MORONET8_LUA_GET_U8(lua, 2)));
        return 1;
    }
    else
        luaL_error(lua, "invalid parameters, btn([id], [player])\n");

    return 0;
}

static int moronet8_lua_api_dt(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 0)
    {
        MORONET8_LUA_PUSH_NUMBER(lua, moronet8_dt(vm));
        return 1;
    }
    else
        luaL_error(lua, "invalid parameters, dt()\n");

    return 0;
}

static int moronet8_lua_api_sin(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 1)
    {
        MORONET8_LUA_PUSH_NUMBER(lua, moronet8_sin(vm, MORONET8_LUA_GET_FLOAT(lua, 1)));
        return 1;
    }
    else
        luaL_error(lua, "invalid parameters, sin(val)\n");

    return 0;
}

static int moronet8_lua_api_cos(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 1)
    {
        MORONET8_LUA_PUSH_NUMBER(lua, moronet8_cos(vm, MORONET8_LUA_GET_FLOAT(lua, 1)));
        return 1;
    }
    else
        luaL_error(lua, "invalid parameters, cos(val)\n");

    return 0;
}

static int moronet8_lua_api_cls(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 0)
    {
        moronet8_cls(vm);
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, cls()\n");

    return 0;
}

static int moronet8_lua_api_color(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            moronet8_lua_push_word(
                lua,
                moronet8_colorget(vm));
            return 1;
        }

        moronet8_colorset(vm, MORONET8_LUA_GET_U8(lua, 1));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, color([col])\n");

    return 0;
}

static int moronet8_lua_api_line(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 6)
    {
        moronet8_line(
            vm,
            MORONET8_LUA_GET_S32(lua, 1),
            MORONET8_LUA_GET_S32(lua, 2),
            MORONET8_LUA_GET_S32(lua, 3),
            MORONET8_LUA_GET_S32(lua, 4),
            top < 5 ? moronet8_colorget(vm) : MORONET8_LUA_GET_U8(lua, 5));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, line(x0, y0, x1, y1, [col])\n");

    return 0;
}

static int moronet8_lua_api_rect(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 6)
    {
        moronet8_rect(
            vm,
            MORONET8_LUA_GET_S32(lua, 1),
            MORONET8_LUA_GET_S32(lua, 2),
            MORONET8_LUA_GET_S32(lua, 3),
            MORONET8_LUA_GET_S32(lua, 4),
            top < 5 ? moronet8_colorget(vm) : MORONET8_LUA_GET_U8(lua, 5));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, rect(x0, y0, x1, y1, [col])\n");

    return 0;
}

static int moronet8_lua_api_rectfill(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 6)
    {
        moronet8_rectfill(
            vm,
            MORONET8_LUA_GET_S32(lua, 1),
            MORONET8_LUA_GET_S32(lua, 2),
            MORONET8_LUA_GET_S32(lua, 3),
            MORONET8_LUA_GET_S32(lua, 4),
            top < 5 ? moronet8_colorget(vm) : MORONET8_LUA_GET_U8(lua, 5));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, rectfill(x0, y0, x1, y1, [col])\n");

    return 0;
}

static int moronet8_lua_api_tileset(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            MORONET8_LUA_PUSH_NUMBER(
                lua,
                moronet8_tilesetget(vm));
            return 1;
        }

        moronet8_tilesetset(vm, MORONET8_LUA_GET_U8(lua, 1));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, tileset([id])\n");

    return 0;
}

static int moronet8_lua_api_spr(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 6)
    {
        moronet8_spr(
            vm,
            MORONET8_LUA_GET_U8(lua, 1),
            MORONET8_LUA_GET_S32(lua, 2),
            MORONET8_LUA_GET_S32(lua, 3),
            top < 4 ? 1 : MORONET8_LUA_GET_U8(lua, 4),
            top < 5 ? 1 : MORONET8_LUA_GET_U8(lua, 5));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, spr(id, x, y, [w, h])\n");

    return 0;
}

static int moronet8_lua_api_palt(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 3)
    {
        if (top == 1)
        {
            MORONET8_LUA_PUSH_BOOL(
                lua,
                moronet8_paltget(
                    vm,
                    MORONET8_LUA_GET_U8(lua, 1)));
            return 1;
        }

        moronet8_paltset(
            vm,
            MORONET8_LUA_GET_U8(lua, 1),
            MORONET8_LUA_GET_BOOL(lua, 2));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, palt(col, [t])\n");

    return 0;
}

static int moronet8_lua_api_code(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            MORONET8_LUA_PUSH_NUMBER(
                lua,
                moronet8_codeget(vm));
            return 1;
        }

        moronet8_codeset(vm, MORONET8_LUA_GET_U8(lua, 1));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, code([id])\n");

    return 0;
}

static int moronet8_lua_api_peek(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 1)
    {
        moronet8_lua_push_word(
            lua,
            moronet8_peek(vm, MORONET8_LUA_GET_NUMBER(lua, 1)));
        return 1;
    }
    else
        luaL_error(lua, "invalid parameters, peek(addr)\n");

    return 0;
}

static int moronet8_lua_api_poke(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 2)
    {
        moronet8_poke(vm, MORONET8_LUA_GET_NUMBER(lua, 1), MORONET8_LUA_GET_U8(lua, 2));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, peek(addr, val)\n");

    return 0;
}

static int moronet8_lua_api_pget(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 2)
    {
        moronet8_lua_push_word(
            lua,
            moronet8_pget(
                vm,
                MORONET8_LUA_GET_NUMBER(lua, 1),
                MORONET8_LUA_GET_NUMBER(lua, 2)));
        return 1;
    }
    else
        luaL_error(lua, "invalid parameters, pget(x, y)\n");

    return 0;
}

static int moronet8_lua_api_pset(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 3)
    {
        moronet8_pset(
            vm,
            MORONET8_LUA_GET_S32(lua, 1),
            MORONET8_LUA_GET_S32(lua, 2),
            MORONET8_LUA_GET_U8(lua, 3));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, pset(x, y, val)\n");

    return 0;
}

#define MORONET8_LUA_API_STACK_SP(name, type)                         \
    int top = lua_gettop(lua);                                        \
    moronet8 *vm = moronet8_lua_get_vm(lua);                          \
                                                                      \
    if (top < 2)                                                      \
    {                                                                 \
        if (top == 0)                                                 \
        {                                                             \
            MORONET8_LUA_PUSH_NUMBER(                                 \
                lua,                                                  \
                (int)moronet8_##name##get(vm));                       \
            return 1;                                                 \
        }                                                             \
                                                                      \
        moronet8_##name##set(                                         \
            vm,                                                       \
            (type)MORONET8_LUA_GET_NUMBER(lua, 1));                   \
        return 0;                                                     \
    }                                                                 \
    else                                                              \
    {                                                                 \
        luaL_error(lua, "invalid parameters, " #name "([offset])\n"); \
    }                                                                 \
    return 0;

#define MORONET8_LUA_API_STACK_PUSH(name)                        \
    int top = lua_gettop(lua);                                   \
    moronet8 *vm = moronet8_lua_get_vm(lua);                     \
                                                                 \
    if (top == 1)                                                \
    {                                                            \
        moronet8_##name(vm, MORONET8_LUA_GET_U8(lua, 1));        \
        return 0;                                                \
    }                                                            \
    else                                                         \
    {                                                            \
        luaL_error(lua, "invalid parameters, " #name "(val)\n"); \
    }                                                            \
    return 0;

#define MORONET8_LUA_API_STACK_POP(name)                      \
    int top = lua_gettop(lua);                                \
    moronet8 *vm = moronet8_lua_get_vm(lua);                  \
                                                              \
    if (top == 0)                                             \
    {                                                         \
        moronet8_lua_push_word(                               \
            lua,                                              \
            moronet8_##name(vm));                             \
        return 1;                                             \
    }                                                         \
    else                                                      \
    {                                                         \
        luaL_error(lua, "invalid parameters, " #name "()\n"); \
    }                                                         \
    return 0;

static int moronet8_lua_api_nethost(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 0)
    {
        MORONET8_LUA_PUSH_BOOL(
            lua,
            moronet8_nethost(vm));
        return 1;
    }
    else
    {
        luaL_error(lua, "invalid parameters, nethost()\n");
    }

    return 0;
}

static int moronet8_lua_api_netclient(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 0)
    {
        MORONET8_LUA_PUSH_BOOL(
            lua,
            moronet8_netclient(vm));
        return 1;
    }
    else
    {
        luaL_error(lua, "invalid parameters, netclient()\n");
    }

    return 0;
}

static int moronet8_lua_api_netsp(lua_State *lua)
{
    MORONET8_LUA_API_STACK_SP(netsp, moronet8_netram_sp);
}

static int moronet8_lua_api_netpush(lua_State *lua)
{
    MORONET8_LUA_API_STACK_PUSH(netpush);
}

static int moronet8_lua_api_netpop(lua_State *lua)
{
    MORONET8_LUA_API_STACK_POP(netpop);
}

static int moronet8_lua_api_pktsp(lua_State *lua)
{
    MORONET8_LUA_API_STACK_SP(pktsp, moronet8_pktram_sp);
}

static int moronet8_lua_api_pktpush(lua_State *lua)
{
    MORONET8_LUA_API_STACK_PUSH(pktpush);
}

static int moronet8_lua_api_pktpop(lua_State *lua)
{
    MORONET8_LUA_API_STACK_POP(pktpop);
}

static int moronet8_lua_api_pktsend(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 0)
    {
        moronet8_pktsend(vm);
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, pktsend()\n");

    return 0;
}

static int moronet8_lua_api_state(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top < 2)
    {
        if (top == 0)
        {
            MORONET8_LUA_PUSH_NUMBER(lua, (int)moronet8_state_get(vm));
            return 1;
        }

        moronet8_state_set(vm, (moronet8_state)MORONET8_LUA_GET_NUMBER(lua, 1));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, state([val])\n");

    return 0;
}

static int moronet8_lua_api_load(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 1)
    {
        size_t size;
        moronet8_load(vm, moronet8_lua_get_string(lua, 1, &size));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, load(cart)\n");

    return 0;
}

static int moronet8_lua_api_netsessionstate(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 0)
    {
        MORONET8_LUA_PUSH_NUMBER(lua, moronet8_netsessionstate(vm));
        return 1;
    }
    else
        luaL_error(lua, "invalid parameters, netsessionstate()\n");

    return 0;
}

static int moronet8_lua_api_netsessionstart(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 0)
    {
        moronet8_netsessionstart(vm);
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, netsessionstart()\n");

    return 0;
}

static int moronet8_lua_api_netsessionjoin(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 1)
    {
        size_t size;
        moronet8_netsessionjoin(vm, moronet8_lua_get_string(lua, 1, &size));
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, netsessionjoin(host)\n");

    return 0;
}

static int moronet8_lua_api_netsessionleave(lua_State *lua)
{
    int top = lua_gettop(lua);
    moronet8 *vm = moronet8_lua_get_vm(lua);

    if (top == 0)
    {
        moronet8_netsessionleave(vm);
        return 0;
    }
    else
        luaL_error(lua, "invalid parameters, netsessionleave()\n");

    return 0;
}

#undef MORONET8_LUA_API_STACK_SP
#undef MORONET8_LUA_API_STACK_PUSH
#undef MORONET8_LUA_API_STACK_POP

static void moronet8_lua_api_init_lua(lua_State *lua, moronet8 *vm, moronet8_api_type type)
{
#define MORONET8_REGISTER_API(name, val, api_type, ...) \
    if ((api_type & type) != 0)                         \
        moronet8_lua_api_register_const(lua, vm, val, #name);
    MORONET8_API_CONST_LIST(MORONET8_REGISTER_API)
#undef MORONET8_REGISTER_API
#define MORONET8_REGISTER_API(name, api_type, ...) \
    if ((api_type & type) != 0)                    \
        moronet8_lua_api_register_fun(lua, vm, moronet8_lua_api_##name, #name);
    MORONET8_API_FUN_LIST(MORONET8_REGISTER_API)
#undef MORONET8_REGISTER_API
}

static moronet8_api *moronet8_lua_api_load_string(moronet8_api *api, const char *buf, size_t size)
{
    MORONET8_UNUSED(size);

    lua_State *lua = (lua_State *)api->state;

    lua_settop(lua, 0);

    if (luaL_loadstring(lua, buf) != LUA_OK || lua_pcall(lua, 0, LUA_MULTRET, 0) != LUA_OK)
    {
        printf("error in load_string: %s", lua_tostring(lua, -1));
        return NULL;
    }

    return api;
}

static moronet8_api *moronet8_lua_api_tick(moronet8_api *api)
{
    lua_State *lua = (lua_State *)api->state;
    if (!lua)
    {
        return NULL;
    }

    lua_getglobal(lua, MORONET8_TICK_FN);
    if (!lua_isfunction(lua, -1))
    {
        lua_pop(lua, 1);
        return NULL;
    }

    if (moronet8_lua_api_do_call(lua, 0, 0) != LUA_OK)
    {
        printf("error in tick: %s", lua_tostring(lua, -1));
        return NULL;
    }

    return api;
}

void moronet8_lua_api_on_cart_loading(moronet8_api *api)
{
    lua_State *lua = (lua_State *)api->state;
    if (!lua)
    {
        return;
    }

    lua_getglobal(lua, "on_cart_loading");
    if (!lua_isfunction(lua, -1))
    {
        lua_pop(lua, 1);
        return;
    }

    if (moronet8_lua_api_do_call(lua, 0, 0) != LUA_OK)
    {
        printf("error in tick: %s", lua_tostring(lua, -1));
    }
}

void moronet8_lua_api_on_cart_loaded(moronet8_api *api)
{
    lua_State *lua = (lua_State *)api->state;
    if (!lua)
    {
        return;
    }

    lua_getglobal(lua, "on_cart_loaded");
    if (!lua_isfunction(lua, -1))
    {
        lua_pop(lua, 1);
        return;
    }

    if (moronet8_lua_api_do_call(lua, 0, 0) != LUA_OK)
    {
        printf("error in tick: %s", lua_tostring(lua, -1));
        return;
    }
}

static void moronet8_lua_api_delete(moronet8_api *api)
{
    if (api->state)
    {
        lua_close((lua_State *)api->state);
    }
}

MORONET8_PUBLIC(moronet8_api *)
moronet8_lua_api_init(moronet8_api *api, moronet8 *vm, moronet8_api_type type)
{
    lua_State *lua = luaL_newstate();
    if (!lua)
    {
        return NULL;
    }

    moronet8_lua_api_init_lua(lua, vm, type);

    api->state = (void *)lua;
    api->delete = &moronet8_lua_api_delete;
    api->load_string = &moronet8_lua_api_load_string;
    api->tick = &moronet8_lua_api_tick;
    api->on_cart_loading = &moronet8_lua_api_on_cart_loading;
    api->on_cart_loaded = &moronet8_lua_api_on_cart_loaded;

    return api;
}

#endif
