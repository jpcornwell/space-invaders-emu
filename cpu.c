
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
        int byte_count, InstrOpType op_type) {
    Instr instr;

    instr.address = pc;
    instr.opcode = memory[pc];
    instr.type = type;
    strcpy(instr.mnenomic, mnenomic);
    instr.cycle_count = cycle_count;
    instr.byte_count = byte_count;

    instr.op_type = op_type;
    if (op_type == INSTR_OP_SINGLE_8) {
        instr.operand_8_1 = memory[pc+1];
    }
    if (op_type == INSTR_OP_DOUBLE_8) {
        instr.operand_8_1 = memory[pc+1];
        instr.operand_8_2 = memory[pc+2];
    }
    if (op_type == INSTR_OP_16) {
        instr.operand_16 = (memory[pc+1] << 8) + memory[pc+2];
    }

    return instr;
}

Instr fetch_instr() {
    Instr instr;

    uint8_t opcode = memory[pc];
    uint8_t opcode_hi = opcode >> 4;  // high 4 bits
    uint8_t opcode_lo = opcode & 0xf; //  low 4 bits

    // temporarily fill out some default values
    instr.type = INSTR_MISC;
    strcpy(instr.mnenomic, "999");
    instr.cycle_count = 999;
    instr.byte_count = 999;
    instr.address = pc;
    instr.opcode = opcode;

    // Handle misc opcodes
    if (opcode_lo == 0x0 ||
        opcode_lo == 0x8) {
        if (opcode_hi <= 0x3) {
            instr = populate_instr(INSTR_MISC, "NOP", 4, 1, INSTR_OP_NONE);
        }
    }

    if (opcode == 0x76) {
        instr = populate_instr(INSTR_MISC, "HLT", 7, 1, INSTR_OP_NONE);
    }

    if (opcode == 0xf3) {
        instr = populate_instr(INSTR_MISC, "DI", 4, 1, INSTR_OP_NONE);
    }

    if (opcode == 0xf8) {
        instr = populate_instr(INSTR_MISC, "EI", 4, 1, INSTR_OP_NONE);
    }

    if (opcode == 0xd3) {
        instr = populate_instr(INSTR_MISC, "OUT", 10, 2, INSTR_OP_SINGLE_8);
        instr.operand_8_1 = memory[pc+1];
    }

    if (opcode == 0xd8) {
        instr = populate_instr(INSTR_MISC, "IN", 10, 2, INSTR_OP_SINGLE_8);
        instr.operand_8_1 = memory[pc+1];
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
