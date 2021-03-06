
#include <stdint.h>

#include <SDL.h>

#define PIXEL_COUNT_WIDTH 224
#define PIXEL_COUNT_HEIGHT 256

#define PIXEL_WIDTH 2
#define PIXEL_HEIGHT 2
#define WINDOW_WIDTH (PIXEL_COUNT_WIDTH * PIXEL_WIDTH)
#define WINDOW_HEIGHT (PIXEL_COUNT_HEIGHT * PIXEL_HEIGHT)

#define GREEN   0, 255,   0
#define RED   255,   0,   0
#define WHITE 255, 255, 255

#define GREEN_BOUNDARY       184
#define RED_BOUNDARY_TOP      33
#define RED_BOUNDARY_BOTTOM   55

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static uint8_t *v_ram = NULL;

void init_display(uint8_t *mem) {
    window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                              WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    v_ram = &mem[0x2400];


    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void update_display(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect pixel;

    int x_pos = 0;
    int y_pos = 256;
    for (int i = 0; i < 7168; i++) {
        uint8_t byte = v_ram[i];
        for (int j = 0; j < 8; j++) {
            if (byte & 0x01) {
                pixel.x = x_pos * PIXEL_WIDTH;
                pixel.y = y_pos * PIXEL_HEIGHT;
                pixel.w = PIXEL_WIDTH;
                pixel.h = PIXEL_HEIGHT;

                if (y_pos > GREEN_BOUNDARY) {
                    SDL_SetRenderDrawColor(renderer, GREEN, 255);
                } else if (y_pos > RED_BOUNDARY_TOP &&
                           y_pos < RED_BOUNDARY_BOTTOM) {
                    SDL_SetRenderDrawColor(renderer, RED, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, WHITE, 255);
                }

                SDL_RenderFillRect(renderer, &pixel);
            }

            byte = byte >> 1;
            y_pos--;
        }

        if (y_pos == 0) {
            y_pos = 256;
            x_pos++;
        }
    }

    SDL_RenderPresent(renderer);
}
