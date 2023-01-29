#include "moronet8.h"

#include <stdio.h>
#include <SDL.h>

#define MORONET8_WINDOW_TITLE "moronet8"
#define MORONET8_WINDOW_SCALE 2
#define MORONET8_FRAMERATE 30
#define MORONET8_SAMPLERATE 1
#define MORONET8_SAMPLE_CHANNELS 1

struct state
{
    SDL_mutex *mutex;
    int quit;
};

#include <stdint.h>
#include <stdbool.h>

typedef enum moronet8_lang moronet8_lang;
typedef struct moronet8_cart moronet8_cart;
typedef struct moronet8 moronet8;

int moronet8_run_player(moronet8 *vm)
{
    moronet8_u16 frame_buffer[MORONET8_SCREEN_WIDTH * MORONET8_SCREEN_HEIGHT];
    vm->ram.vram = (void *)&frame_buffer[0];

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_Window *window = SDL_CreateWindow(MORONET8_WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MORONET8_SCREEN_WIDTH * MORONET8_WINDOW_SCALE, MORONET8_SCREEN_HEIGHT * MORONET8_WINDOW_SCALE, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR565, SDL_TEXTUREACCESS_STREAMING, MORONET8_SCREEN_WIDTH, MORONET8_SCREEN_HEIGHT);

    struct state state;
    state.quit = 1 == 0;
    {
        state.mutex = SDL_CreateMutex();
    }

    const moronet8_u64 Delta = SDL_GetPerformanceFrequency() / MORONET8_FRAMERATE;
    moronet8_u64 nextTick = SDL_GetPerformanceCounter();

    while (!state.quit)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                state.quit = true;
                break;
            case SDL_KEYUP:
                // Quit when pressing the escape button.
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    state.quit = true;
                }
                break;
            }
        }

        {
            const uint8_t *keyboard = SDL_GetKeyboardState(NULL);
            memset((void *)&vm->ram.buttons, 0, 2);

            static const struct
            {
                SDL_Scancode code;
                enum moronet8_button button;
            } Keys[] =
                {
                    {SDL_SCANCODE_LEFT, MORONET8_BUTTON_LEFT},
                    {SDL_SCANCODE_RIGHT, MORONET8_BUTTON_RIGHT},
                    {SDL_SCANCODE_UP, MORONET8_BUTTON_UP},
                    {SDL_SCANCODE_DOWN, MORONET8_BUTTON_DOWN},

                    {SDL_SCANCODE_A, MORONET8_BUTTON_LEFT},
                    {SDL_SCANCODE_D, MORONET8_BUTTON_RIGHT},
                    {SDL_SCANCODE_Z, MORONET8_BUTTON_UP},
                    {SDL_SCANCODE_S, MORONET8_BUTTON_DOWN},
                    {SDL_SCANCODE_SPACE, MORONET8_BUTTON_A},
                    {SDL_SCANCODE_TAB, MORONET8_BUTTON_START},
                };

            for (moronet8_u32 i = 0; i < SDL_arraysize(Keys); i++)
            {
                if (keyboard[Keys[i].code])
                {
                    ((moronet8_u8 *)&vm->ram.buttons)[0] |= 1 << Keys[i].button;
                }
            }
        }

        SDL_LockMutex(state.mutex);
        {
            moronet8_netsessionpoll(vm);
            moronet8_tick(vm, 1.0f / MORONET8_FRAMERATE);
        }
        SDL_UnlockMutex(state.mutex);

        SDL_RenderClear(renderer);

        {
            void *pixels = NULL;
            moronet8_s32 pitch = 0;
            SDL_Rect destination;
            SDL_LockTexture(texture, NULL, &pixels, &pitch);
            SDL_memcpy(pixels, (const void *)frame_buffer, pitch * MORONET8_SCREEN_HEIGHT);
            SDL_UnlockTexture(texture);

            // Render the image in the proper aspect ratio.
            {
                moronet8_s32 windowWidth, windowHeight;
                SDL_GetWindowSize(window, &windowWidth, &windowHeight);
                float widthRatio = (float)windowWidth / MORONET8_SCREEN_WIDTH;
                float heightRatio = (float)windowHeight / MORONET8_SCREEN_HEIGHT;
                float optimalSize = widthRatio < heightRatio ? widthRatio : heightRatio;
                destination.w = (moronet8_s32)(MORONET8_SCREEN_WIDTH * optimalSize);
                destination.h = (moronet8_s32)(MORONET8_SCREEN_HEIGHT * optimalSize);
                destination.x = windowWidth / 2 - destination.w / 2;
                destination.y = windowHeight / 2 - destination.h / 2;
            }

            SDL_RenderCopy(renderer, texture, NULL, &destination);
        }

        SDL_RenderPresent(renderer);

        {
            moronet8_s64 delay = (nextTick += Delta) - SDL_GetPerformanceCounter();

            if (delay > 0)
                SDL_Delay((moronet8_u32)(delay * 1000 / SDL_GetPerformanceFrequency()));
        }
    }

    SDL_DestroyMutex(state.mutex);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // SDL_free(cart);

    MORONET8_FREE(vm->screen);
    vm->screen = NULL;

    return 0;
}
