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
#include "moronet8.h"

typedef enum moronet8_lang moronet8_lang;
typedef struct moronet8_api moronet8_api;
typedef struct moronet8_cart moronet8_cart;
typedef struct moronet8 moronet8;

static void moronet8_assert_api(moronet8_api *api)
{
	assert_non_null(api->state);
	assert_non_null(api->delete);
	assert_non_null(api->load_string);
	assert_non_null(api->tick);
}

static void moronet8_assert_api_null(moronet8_api *api)
{
	assert_null(api->state);
	assert_null(api->delete);
	assert_null(api->load_string);
	assert_null(api->tick);
}

static moronet8 *_moronet8_create()
{
	moronet8 *o = moronet8_create();
	assert_non_null(o);
	moronet8_assert_api_null(&o->bios_api);
	moronet8_assert_api_null(&o->api);
	return o;
}

static void _moronet8_load_bios(moronet8 *vm, moronet8_cart *cart)
{
	assert_non_null(moronet8_load_bios(vm, cart));
}

#define moronet8_assert_create() _moronet8_create()
#define moronet8_assert_load_bios(vm, cart) _moronet8_load_bios(vm, cart)
