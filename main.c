
#include <stdbool.h>
#include <stdio.h>

#include "SDL.h"
#include "SDL_mixer.h"

#include "cpu.h"
#include "display.h"
#include "audio.h"

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

void process_shift_register() {
    static uint16_t reg_shift = 0;
    uint8_t in = read_port(4);
    uint8_t offset = read_port(2) & 0x07;
    uint8_t out = 0;

    reg_shift = (reg_shift >> 8) | ((uint16_t)in << 8);
    out = (reg_shift >> (8 - offset)) & 0xff;

    write_port(3, out);
}

void process_sound() {
    static bool previous_values[9] = {0};
    bool val;

    val = read_port_bit(3, 1);
    if (val && !previous_values[0]) {
        play_sound(SOUND_SHOOT);
    }
    previous_values[0] = val;

    val = read_port_bit(3, 2);
    if (val && !previous_values[1]) {
        play_sound(SOUND_EXPLOSION);
    }
    previous_values[1] = val;

    val = read_port_bit(3, 3);
    if (val && !previous_values[2]) {
        play_sound(SOUND_INVADER_KILLED);
    }
    previous_values[2] = val;

    val = read_port_bit(5, 0);
    if (val && !previous_values[3]) {
        play_sound(SOUND_INVADER_1);
    }
    previous_values[3] = val;

    val = read_port_bit(5, 1);
    if (val && !previous_values[4]) {
        play_sound(SOUND_INVADER_2);
    }
    previous_values[4] = val;

    val = read_port_bit(5, 2);
    if (val && !previous_values[5]) {
        play_sound(SOUND_INVADER_3);
    }
    previous_values[5] = val;

    val = read_port_bit(5, 3);
    if (val && !previous_values[6]) {
        play_sound(SOUND_INVADER_4);
    }
    previous_values[6] = val;

    val = read_port_bit(3, 0);
    if (val && !previous_values[7]) {
        play_sound(SOUND_UFO_LOW);
    }
    previous_values[7] = val;

    val = read_port_bit(5, 4);
    if (val && !previous_values[8]) {
        play_sound(SOUND_UFO_HIGH);
    }
    previous_values[8] = val;
}

void set_dip_switches() {
    // bit 0 = DIP3
    // bit 1 = DIP5
    // bits 1 & 0
    //   00 = 3 ships
    //   01 = 4 ships
    //   10 = 5 ships
    //   11 = 6 ships
    write_port_bit(2, 0, 1);
    write_port_bit(2, 1, 1);

    // bit 3 = DIP6
    // 0 = extra ship at 1500
    // 1 = extra ship at 1000
    write_port_bit(2, 3, 1);

    // bit 7 = DIP7
    // 0 = display coin info on demo screen
    // 1 = don't display coin info on demo screen
    write_port_bit(2, 7, 0);
}

void handle_inputs() {
    SDL_PumpEvents();
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // CREDIT (1 if deposited)
    // Port 1 Bit 0
    if (state[SDL_SCANCODE_RETURN]) {
        write_port_bit(1, 0, 1);
    } else {
        write_port_bit(1, 0, 0);
    }

    // 1P Start (1 if pressed)
    // Port 1 Bit 2
    if (state[SDL_SCANCODE_1]) {
        write_port_bit(1, 2, 1);
    } else {
        write_port_bit(1, 2, 0);
    }

    // 2P Start (1 if pressed)
    // Port 1 Bit 1
    if (state[SDL_SCANCODE_2]) {
        write_port_bit(1, 1, 1);
    } else {
        write_port_bit(1, 1, 0);
    }

    // 1P Fire (1 if pressed)
    // Port 1 Bit 4
    // 2P Fire (1 if pressed)
    // Port 2 Bit 4
    if (state[SDL_SCANCODE_SPACE]) {
        write_port_bit(1, 4, 1);
        write_port_bit(2, 4, 1);
    } else {
        write_port_bit(1, 4, 0);
        write_port_bit(2, 4, 0);
    }

    // 1P Left (1 if pressed)
    // Port 1 Bit 5
    // 2P Left (1 if pressed)
    // Port 2 Bit 5
    if (state[SDL_SCANCODE_LEFT]) {
        write_port_bit(1, 5, 1);
        write_port_bit(2, 5, 1);
    } else {
        write_port_bit(1, 5, 0);
        write_port_bit(2, 5, 0);
    }

    // 1P Right (1 if pressed)
    // Port 1 Bit 6
    // 2P Right (1 if pressed)
    // Port 2 Bit 6
    if (state[SDL_SCANCODE_RIGHT]) {
        write_port_bit(1, 6, 1);
        write_port_bit(2, 6, 1);
    } else {
        write_port_bit(1, 6, 0);
        write_port_bit(2, 6, 0);
    }
}

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        printf("SDL Mixer could not initialize!\n");
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
    init_audio();
    set_dip_switches();

    while (!quit) {
        // TODO: improve timing mechanism
        // Currently hard coded to run loop every 8 ms
        // Runs 16,000 cycles in this time
        // Executes one of the two interrupts
        // Updates display every other loop
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        start_tick_time = SDL_GetTicks();

        int cycle_count = 0;
        while (cycle_count < 16000) {
            instr = fetch_instr();
            cycle_count += exec_instr(instr);
            process_shift_register();
            process_sound();
        }

        interrupt_flip_flop = !interrupt_flip_flop;
        
        if (interrupt_flip_flop) {
            half_draw_interrupt();
        } else {
            update_display();
            full_draw_interrupt();
        }

        elapsed_tick_time = SDL_GetTicks() - start_tick_time;
        if (elapsed_tick_time < 8) {
            SDL_Delay(8 - elapsed_tick_time);
        }
        handle_inputs();
        // TODO: handle outputs
    }
}
