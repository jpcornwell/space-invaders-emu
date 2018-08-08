
#include <stdint.h>
#include <string.h>

#include "cpu.h"

uint16_t pc;

// temporarily keep memory in here
uint8_t memory[65536];

void init_cpu() {
    memory[0] = 0x38;
}

Instr fetch_instr() {
    Instr instr;

    uint8_t opcode = memory[pc];

    // temporarily fill out some default values
    instr.type = INSTR_JUMP;
    instr.opcode = opcode;
    strcpy(instr.mnenomic, "DEF");
    instr.cycles = 100;

    // Handle misc opcodes
    if ((opcode & 0xf) == 0x0 ||
        (opcode & 0xf) == 0x8) {
        if ((opcode & 0xf0) == 0x00 ||
            (opcode & 0xf0) == 0x10 ||
            (opcode & 0xf0) == 0x20 ||
            (opcode & 0xf0) == 0x30) {

            instr.type = INSTR_MISC;
            instr.opcode = opcode;
            strcpy(instr.mnenomic, "NOP");
            instr.cycles = 4;
        }
    }

    return instr;
}
