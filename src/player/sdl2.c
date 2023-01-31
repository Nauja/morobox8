#include "morobox8.h"

#include <stdio.h>
#include <SDL.h>

#define MOROBOX8_WINDOW_TITLE "morobox8"
#define MOROBOX8_WINDOW_SCALE 2
#define MOROBOX8_FRAMERATE 30
#define MOROBOX8_SAMPLERATE 1
#define MOROBOX8_SAMPLE_CHANNELS 1

struct state
{
    SDL_mutex *mutex;
    int quit;
};

#include <stdint.h>
#include <stdbool.h>

typedef enum morobox8_lang morobox8_lang;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8 morobox8;

int morobox8_run_player(morobox8 *vm)
{
    morobox8_u16 frame_buffer[MOROBOX8_SCREEN_WIDTH * MOROBOX8_SCREEN_HEIGHT];
    vm->ram.vram = (void *)&frame_buffer[0];

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_Window *window = SDL_CreateWindow(MOROBOX8_WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MOROBOX8_SCREEN_WIDTH * MOROBOX8_WINDOW_SCALE, MOROBOX8_SCREEN_HEIGHT * MOROBOX8_WINDOW_SCALE, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR565, SDL_TEXTUREACCESS_STREAMING, MOROBOX8_SCREEN_WIDTH, MOROBOX8_SCREEN_HEIGHT);

    struct state state;
    state.quit = 1 == 0;
    {
        state.mutex = SDL_CreateMutex();
    }

    const morobox8_u64 Delta = SDL_GetPerformanceFrequency() / MOROBOX8_FRAMERATE;
    morobox8_u64 nextTick = SDL_GetPerformanceCounter();

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
                enum morobox8_button button;
            } Keys[] =
                {
                    {SDL_SCANCODE_LEFT, MOROBOX8_BUTTON_LEFT},
                    {SDL_SCANCODE_RIGHT, MOROBOX8_BUTTON_RIGHT},
                    {SDL_SCANCODE_UP, MOROBOX8_BUTTON_UP},
                    {SDL_SCANCODE_DOWN, MOROBOX8_BUTTON_DOWN},

                    {SDL_SCANCODE_A, MOROBOX8_BUTTON_LEFT},
                    {SDL_SCANCODE_D, MOROBOX8_BUTTON_RIGHT},
                    {SDL_SCANCODE_Z, MOROBOX8_BUTTON_UP},
                    {SDL_SCANCODE_S, MOROBOX8_BUTTON_DOWN},
                    {SDL_SCANCODE_SPACE, MOROBOX8_BUTTON_A},
                    {SDL_SCANCODE_TAB, MOROBOX8_BUTTON_START},
                };

            for (morobox8_u32 i = 0; i < SDL_arraysize(Keys); i++)
            {
                if (keyboard[Keys[i].code])
                {
                    ((morobox8_u8 *)&vm->ram.buttons)[0] |= 1 << Keys[i].button;
                }
            }
        }

        SDL_LockMutex(state.mutex);
        {
            morobox8_netsessionpoll(vm);
            morobox8_tick(vm, 1.0f / MOROBOX8_FRAMERATE);
        }
        SDL_UnlockMutex(state.mutex);

        SDL_RenderClear(renderer);

        {
            void *pixels = NULL;
            morobox8_s32 pitch = 0;
            SDL_Rect destination;
            SDL_LockTexture(texture, NULL, &pixels, &pitch);
            SDL_memcpy(pixels, (const void *)frame_buffer, pitch * MOROBOX8_SCREEN_HEIGHT);
            SDL_UnlockTexture(texture);

            // Render the image in the proper aspect ratio.
            {
                morobox8_s32 windowWidth, windowHeight;
                SDL_GetWindowSize(window, &windowWidth, &windowHeight);
                float widthRatio = (float)windowWidth / MOROBOX8_SCREEN_WIDTH;
                float heightRatio = (float)windowHeight / MOROBOX8_SCREEN_HEIGHT;
                float optimalSize = widthRatio < heightRatio ? widthRatio : heightRatio;
                destination.w = (morobox8_s32)(MOROBOX8_SCREEN_WIDTH * optimalSize);
                destination.h = (morobox8_s32)(MOROBOX8_SCREEN_HEIGHT * optimalSize);
                destination.x = windowWidth / 2 - destination.w / 2;
                destination.y = windowHeight / 2 - destination.h / 2;
            }

            SDL_RenderCopy(renderer, texture, NULL, &destination);
        }

        SDL_RenderPresent(renderer);

        {
            morobox8_s64 delay = (nextTick += Delta) - SDL_GetPerformanceCounter();

            if (delay > 0)
                SDL_Delay((morobox8_u32)(delay * 1000 / SDL_GetPerformanceFrequency()));
        }
    }

    SDL_DestroyMutex(state.mutex);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // SDL_free(cart);

    MOROBOX8_FREE(vm->screen);
    vm->screen = NULL;

    return 0;
}
