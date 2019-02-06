
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

// Generate this interrupt when screen is half way drawn
void half_draw_interrupt(void) {
    process_interrupt_signal(INT_SIGNAL_1);
}

// Generate this interrupt when screen is fully drawn
void full_draw_interrupt(void) {
    process_interrupt_signal(INT_SIGNAL_2);
}

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    bool quit = false;
    SDL_Event e;

    Uint32 start_tick_time;
    Uint32 elapsed_tick_time;
    bool interrupt_flip_flop = false;

    Instr instr;

    load_memory("invaders.rom");
    init_cpu(memory);
    init_display(memory);

    while (!quit) {
        // TODO: improve timing mechanism
        // Currently hard coded to run loop every 16 ms
        // Runs 32,000 instructions in this time
        // Executes one of the two interrupts
        // Updates display every other loop
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        start_tick_time = SDL_GetTicks();

        // TODO: Improve timing by actually taking into account cycles per
        // instruction
        for (int i = 0; i < 32000; i++) {
            instr = fetch_instr();
            exec_instr(instr);
        }

        interrupt_flip_flop = !interrupt_flip_flop;
        
        if (interrupt_flip_flop) {
            half_draw_interrupt();
        } else {
            update_display();
            full_draw_interrupt();
        }

        elapsed_tick_time = SDL_GetTicks() - start_tick_time;
        if (elapsed_tick_time < 16) {
            SDL_Delay(16 - elapsed_tick_time);
        }
        // TODO: handle inputs
        // TODO: handle outputs
    }
}
