
#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "cpu.h"
#include "display.h"

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    bool quit = false;
    SDL_Event e;

    Instr instr;
    uint8_t memory[65536];

    init_cpu(memory);
    init_display(memory);

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        instr = fetch_instr();
        exec_instr(instr);
        update_display();
    }
}
