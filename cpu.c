
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cpu.h"

uint16_t pc;

// temporarily keep memory in here
uint8_t memory[65536];

void init_cpu() {
    FILE *fp = fopen("invaders.rom", "rb");
    
    if (fp == NULL) {
        printf("Error opening ROM file\n");
        exit(1);
    }

    fread(memory, 1, 65536, fp);

    fclose(fp);

    pc = 0;
}

Instr populate_instr(InstrType type, char *mnenomic, int cycle_count,
        int byte_count) {
    Instr instr;

    instr.address = pc;
    instr.opcode = memory[pc];
    instr.type = type;
    strcpy(instr.mnenomic, mnenomic);
    instr.cycle_count = cycle_count;
    instr.byte_count = byte_count;

    return instr;
}

Instr fetch_instr() {
    Instr instr;

    uint8_t opcode = memory[pc];

    // temporarily fill out some default values
    instr.type = INSTR_MISC;
    strcpy(instr.mnenomic, "999");
    instr.cycle_count = 999;
    instr.byte_count = 999;
    instr.address = pc;
    instr.opcode = opcode;

    // Handle misc opcodes
    if ((opcode & 0xf) == 0x0 ||
        (opcode & 0xf) == 0x8) {
        if ((opcode & 0xf0) == 0x00 ||
            (opcode & 0xf0) == 0x10 ||
            (opcode & 0xf0) == 0x20 ||
            (opcode & 0xf0) == 0x30) {

            instr = populate_instr(INSTR_MISC, "NOP", 4, 1);
        }
    }

    if (opcode == 0x76) {
        instr = populate_instr(INSTR_MISC, "HLT", 7, 1);
    }

    if (instr.cycle_count == 999) {
        printf("Error: Unrecognized opcode: 0x%02x\n", instr.opcode);
        exit(1);
    }

    // temporarily increment pc here
    pc += instr.byte_count;

    // temporarily exit when pc reaches end of ROM
    if (pc >= 0x1fff) {
        printf("Complete: have reached the end of ROM\n");
        exit(0);
    }

    return instr;
}
