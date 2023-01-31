#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <cmocka.h>
#include "tests/fs_testutils.h"
#include "morobox8.h"

typedef enum morobox8_lang morobox8_lang;
typedef struct morobox8_api morobox8_api;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8 morobox8;

static void morobox8_assert_api(morobox8_api *api)
{
	assert_non_null(api->state);
	assert_non_null(api->delete);
	assert_non_null(api->load_string);
	assert_non_null(api->tick);
}

static void morobox8_assert_api_null(morobox8_api *api)
{
	assert_null(api->state);
	assert_null(api->delete);
	assert_null(api->load_string);
	assert_null(api->tick);
}

static morobox8 *_morobox8_create()
{
	morobox8 *o = morobox8_create();
	assert_non_null(o);
	morobox8_assert_api_null(&o->bios_api);
	morobox8_assert_api_null(&o->api);
	return o;
}

static void _morobox8_load_bios(morobox8 *vm, morobox8_cart *cart)
{
	assert_non_null(morobox8_load_bios(vm, cart));
}

#define morobox8_assert_create() _morobox8_create()
#define morobox8_assert_load_bios(vm, cart) _morobox8_load_bios(vm, cart)
