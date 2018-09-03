
#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "cpu.h"
#include "display.h"

static uint8_t memory[65536] = {0};

void load_memory(char *path) {
    FILE *fp = fopen(path, "rb");
    
    if (fp == NULL) {
        printf("Error opening ROM file\n");
        exit(1);
    }

    memset(memory, 0, sizeof memory);
    fread(memory, 1, 65536, fp);

    fclose(fp);
}

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    bool quit = false;
    SDL_Event e;

    Instr instr;

    load_memory("invaders.rom");
    init_cpu(memory);
    init_display(memory);

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Temporarily use this loop to speed up emulation 
        // (this reduces the number of display updates)
        for (int i = 0; i < 30; i++) {
            instr = fetch_instr();
            printf("%04x: %s\n", instr.address, instr.mnemonic);
            exec_instr(instr);
        }
        update_display();
    }
}
