
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

    if (instr.type == INSTR_MOVE) {
        int op_src = instr.opcode & 0x7;
        int op_dest = (instr.opcode >> 3) & 0x7;

        switch (op_src) {
            case 0:
                instr.move_source = MOV_LOC_B;
                break;
            case 1:
                instr.move_source = MOV_LOC_C;
                break;
            case 2:
                instr.move_source = MOV_LOC_D;
                break;
            case 3:
                instr.move_source = MOV_LOC_E;
                break;
            case 4:
                instr.move_source = MOV_LOC_H;
                break;
            case 5:
                instr.move_source = MOV_LOC_L;
                break;
            case 6:
                instr.move_source = MOV_LOC_MEM_REF;
                break;
            case 7:
                instr.move_source = MOV_LOC_A;
                break;
        }

        switch (op_dest) {
            case 0:
                instr.move_destination = MOV_LOC_B;
                break;
            case 1:
                instr.move_destination = MOV_LOC_C;
                break;
            case 2:
                instr.move_destination = MOV_LOC_D;
                break;
            case 3:
                instr.move_destination = MOV_LOC_E;
                break;
            case 4:
                instr.move_destination = MOV_LOC_H;
                break;
            case 5:
                instr.move_destination = MOV_LOC_L;
                break;
            case 6:
                instr.move_destination = MOV_LOC_MEM_REF;
                break;
            case 7:
                instr.move_destination = MOV_LOC_A;
                break;
        }
    }

    return instr;
}

Instr fetch_instr() {
    Instr instr;

    uint8_t opcode = memory[pc];

    // temporarily fill out some default values
    instr.type = INSTR_NOP;
    strcpy(instr.mnenomic, "999");
    instr.cycle_count = 999;
    instr.byte_count = 999;
    instr.address = pc;
    instr.opcode = opcode;

    switch (opcode) {
        case 0x00:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x01:
            instr = populate_instr(INSTR_LOAD_REG_PAIR_IMMEDIATE, "LXI", 10, 3,
                                   INSTR_OP_REG_PAIR_B_AND_OP_16);
            break;
        case 0x02:
            instr = populate_instr(INSTR_STORE_ACCUMULATOR, "STAX", 7, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0x03:
            instr = populate_instr(INSTR_INCREMENT_REG_PAIR, "INX", 5, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0x04:
            instr = populate_instr(INSTR_INCREMENT_REG, "INR", 5, 1,
                                   INSTR_OP_REG_B);
            break;
        case 0x05:
            instr = populate_instr(INSTR_DECREMENT_REG, "DCR", 5, 1,
                                   INSTR_OP_REG_B);
            break;
        case 0x06:
            instr = populate_instr(INSTR_MOVE_IMMEDIATE, "MVI", 7, 2,
                                   INSTR_OP_REG_B_AND_OP_8);
            break;
        case 0x07:
            instr = populate_instr(INSTR_ROTATE_ACCUMULATOR_LEFT_CARRY, "RLC",
                                   4, 1, INSTR_OP_NONE);
            break;
        case 0x08:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x09:
            instr = populate_instr(INSTR_DOUBLE_ADD, "DAD", 10, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0x0a:
            instr = populate_instr(INSTR_LOAD_ACCUMULATOR, "LDAX", 7, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0x0b:
            instr = populate_instr(INSTR_DECREMENT_REG_PAIR, "DCX", 5, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0x0c:
            instr = populate_instr(INSTR_INCREMENT_REG, "INR", 5, 1,
                                   INSTR_OP_REG_C);
            break;
        case 0x0d:
            instr = populate_instr(INSTR_DECREMENT_REG, "DCR", 5, 1,
                                   INSTR_OP_REG_C);
            break;
        case 0x0e:
            instr = populate_instr(INSTR_MOVE_IMMEDIATE, "MVI", 7, 2,
                                   INSTR_OP_REG_C_AND_OP_8);
            break;
        case 0x0f:
            instr = populate_instr(INSTR_ROTATE_ACCUMULATOR_RIGHT_CARRY, "RRC",
                                   4, 1, INSTR_OP_NONE);
            break;
        case 0x10:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x11:
            instr = populate_instr(INSTR_LOAD_REG_PAIR_IMMEDIATE, "LXI", 10, 3,
                                   INSTR_OP_REG_PAIR_D_AND_OP_16);
            break;
        case 0x12:
            instr = populate_instr(INSTR_STORE_ACCUMULATOR, "STAX", 7, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0x13:
            instr = populate_instr(INSTR_INCREMENT_REG_PAIR, "INX", 5, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0x14:
            instr = populate_instr(INSTR_INCREMENT_REG, "INR", 5, 1,
                                   INSTR_OP_REG_D);
            break;
        case 0x15:
            instr = populate_instr(INSTR_DECREMENT_REG, "DCR", 5, 1,
                                   INSTR_OP_REG_D);
            break;
        case 0x16:
            instr = populate_instr(INSTR_MOVE_IMMEDIATE, "MVI", 7, 2,
                                   INSTR_OP_REG_D_AND_OP_8);
            break;
        case 0x17:
            instr = populate_instr(INSTR_ROTATE_ACCUMULATOR_LEFT, "RAL",
                                   4, 1, INSTR_OP_NONE);
            break;
        case 0x18:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x19:
            instr = populate_instr(INSTR_DOUBLE_ADD, "DAD", 10, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0x1a:
            instr = populate_instr(INSTR_LOAD_ACCUMULATOR, "LDAX", 7, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0x1b:
            instr = populate_instr(INSTR_DECREMENT_REG_PAIR, "DCX", 5, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0x1c:
            instr = populate_instr(INSTR_INCREMENT_REG, "INR", 5, 1,
                                   INSTR_OP_REG_E);
            break;
        case 0x1d:
            instr = populate_instr(INSTR_DECREMENT_REG, "DCR", 5, 1,
                                   INSTR_OP_REG_E);
            break;
        case 0x1e:
            instr = populate_instr(INSTR_MOVE_IMMEDIATE, "MVI", 7, 2,
                                   INSTR_OP_REG_E_AND_OP_8);
            break;
        case 0x1f:
            instr = populate_instr(INSTR_ROTATE_ACCUMULATOR_RIGHT, "RAR",
                                   4, 1, INSTR_OP_NONE);
            break;
        case 0x20:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x21:
            instr = populate_instr(INSTR_LOAD_REG_PAIR_IMMEDIATE, "LXI", 10, 3,
                                   INSTR_OP_REG_PAIR_H_AND_OP_16);
            break;
        case 0x22:
            instr = populate_instr(INSTR_STORE_HL_DIRECT, "SHLD", 16, 3,
                                   INSTR_OP_16);
            break;
        case 0x23:
            instr = populate_instr(INSTR_INCREMENT_REG_PAIR, "INX", 5, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0x24:
            instr = populate_instr(INSTR_INCREMENT_REG, "INR", 5, 1,
                                   INSTR_OP_REG_H);
            break;
        case 0x25:
            instr = populate_instr(INSTR_DECREMENT_REG, "DCR", 5, 1,
                                   INSTR_OP_REG_H);
            break;
        case 0x26:
            instr = populate_instr(INSTR_MOVE_IMMEDIATE, "MVI", 7, 2,
                                   INSTR_OP_REG_H_AND_OP_8);
            break;
        case 0x27:
            instr = populate_instr(INSTR_DECIMAL_ADJUST_ACCUMULATOR, "DAA",
                                   4, 1, INSTR_OP_NONE);
            break;
        case 0x28:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x29:
            instr = populate_instr(INSTR_DOUBLE_ADD, "DAD", 10, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0x2a:
            instr = populate_instr(INSTR_LOAD_HL_DIRECT, "LHLD", 16, 3,
                                   INSTR_OP_16);
            break;
        case 0x2b:
            instr = populate_instr(INSTR_DECREMENT_REG_PAIR, "DCX", 5, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0x2c:
            instr = populate_instr(INSTR_INCREMENT_REG, "INR", 5, 1,
                                   INSTR_OP_REG_L);
            break;
        case 0x2d:
            instr = populate_instr(INSTR_DECREMENT_REG, "DCR", 5, 1,
                                   INSTR_OP_REG_L);
            break;
        case 0x2e:
            instr = populate_instr(INSTR_MOVE_IMMEDIATE, "MVI", 7, 2,
                                   INSTR_OP_REG_L_AND_OP_8);
            break;
        case 0x2f:
            instr = populate_instr(INSTR_COMPLEMENT_ACCUMULATOR, "CMA", 4, 1,
                                   INSTR_OP_NONE);
            break;
        case 0x30:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x31:
            instr = populate_instr(INSTR_LOAD_REG_PAIR_IMMEDIATE, "LXI", 10, 3,
                                   INSTR_OP_REG_PAIR_SP_AND_OP_16);
            break;
        case 0x32:
            instr = populate_instr(INSTR_STORE_ACCUMULATOR_DIRECT, "STA", 13,
                                   3, INSTR_OP_16);
            break;
        case 0x33:
            instr = populate_instr(INSTR_INCREMENT_REG_PAIR, "INX", 5, 1,
                                   INSTR_OP_REG_PAIR_SP);
            break;
        case 0x34:
            instr = populate_instr(INSTR_INCREMENT_REG, "INR", 10, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0x35:
            instr = populate_instr(INSTR_DECREMENT_REG, "DCR", 10, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0x36:
            instr = populate_instr(INSTR_MOVE_IMMEDIATE, "MVI", 10, 2,
                                   INSTR_OP_MEM_REF_AND_OP_8);
            break;
        case 0x37:
            instr = populate_instr(INSTR_SET_CARRY, "STC", 4, 1,
                                   INSTR_OP_NONE);
            break;
        case 0x38:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x39:
            instr = populate_instr(INSTR_DOUBLE_ADD, "DAD", 10, 1,
                                   INSTR_OP_REG_PAIR_SP);
            break;
        case 0x3a:
            instr = populate_instr(INSTR_LOAD_ACCUMULATOR_DIRECT, "LDA", 13,
                                   3, INSTR_OP_16);
            break;
        case 0x3b:
            instr = populate_instr(INSTR_DECREMENT_REG_PAIR, "DCX", 5, 1,
                                   INSTR_OP_REG_PAIR_SP);
            break;
        case 0x3c:
            instr = populate_instr(INSTR_INCREMENT_REG, "INR", 5, 1,
                                   INSTR_OP_REG_A);
            break;
        case 0x3d:
            instr = populate_instr(INSTR_DECREMENT_REG, "DCR", 5, 1,
                                   INSTR_OP_REG_A);
            break;
        case 0x3e:
            instr = populate_instr(INSTR_MOVE_IMMEDIATE, "MVI", 7, 2,
                                   INSTR_OP_REG_A_AND_OP_8);
            break;
        case 0x3f:
            instr = populate_instr(INSTR_COMPLEMENT_CARRY, "CMC", 4, 1,
                                   INSTR_OP_NONE);
            break;
        case 0x40:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x41:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x42:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x43:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x44:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x45:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x46:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x47:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x48:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x49:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x4a:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x4b:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x4c:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x4d:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x4e:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x4f:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x50:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x51:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x52:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x53:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x54:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x55:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x56:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x57:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x58:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x59:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x5a:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x5b:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x5c:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x5d:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x5e:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x5f:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x60:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x61:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x62:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x63:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x64:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x65:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x66:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x67:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x68:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x69:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x6a:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x6b:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x6c:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x6d:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x6e:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x6f:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x70:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x71:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x72:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x73:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x74:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x75:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x76:
            instr = populate_instr(INSTR_HALT, "HLT", 7, 1, INSTR_OP_NONE);
            break;
        case 0x77:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x78:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x79:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x7a:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x7b:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x7c:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x7d:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x7e:
            instr = populate_instr(INSTR_MOVE, "MOV", 7, 1, INSTR_OP_NONE);
            break;
        case 0x7f:
            instr = populate_instr(INSTR_MOVE, "MOV", 5, 1, INSTR_OP_NONE);
            break;
        case 0x80:
            break;
        case 0x81:
            break;
        case 0x82:
            break;
        case 0x83:
            break;
        case 0x84:
            break;
        case 0x85:
            break;
        case 0x86:
            break;
        case 0x87:
            break;
        case 0x88:
            break;
        case 0x89:
            break;
        case 0x8a:
            break;
        case 0x8b:
            break;
        case 0x8c:
            break;
        case 0x8d:
            break;
        case 0x8e:
            break;
        case 0x8f:
            break;
        case 0x90:
            break;
        case 0x91:
            break;
        case 0x92:
            break;
        case 0x93:
            break;
        case 0x94:
            break;
        case 0x95:
            break;
        case 0x96:
            break;
        case 0x97:
            break;
        case 0x98:
            break;
        case 0x99:
            break;
        case 0x9a:
            break;
        case 0x9b:
            break;
        case 0x9c:
            break;
        case 0x9d:
            break;
        case 0x9e:
            break;
        case 0x9f:
            break;
        case 0xa0:
            break;
        case 0xa1:
            break;
        case 0xa2:
            break;
        case 0xa3:
            break;
        case 0xa4:
            break;
        case 0xa5:
            break;
        case 0xa6:
            break;
        case 0xa7:
            break;
        case 0xa8:
            break;
        case 0xa9:
            break;
        case 0xaa:
            break;
        case 0xab:
            break;
        case 0xac:
            break;
        case 0xad:
            break;
        case 0xae:
            break;
        case 0xaf:
            break;
        case 0xb0:
            break;
        case 0xb1:
            break;
        case 0xb2:
            break;
        case 0xb3:
            break;
        case 0xb4:
            break;
        case 0xb5:
            break;
        case 0xb6:
            break;
        case 0xb7:
            break;
        case 0xb8:
            break;
        case 0xb9:
            break;
        case 0xba:
            break;
        case 0xbb:
            break;
        case 0xbc:
            break;
        case 0xbd:
            break;
        case 0xbe:
            break;
        case 0xbf:
            break;
        case 0xc0:
            break;
        case 0xc1:
            instr = populate_instr(INSTR_POP, "POP", 10, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0xc2:
            break;
        case 0xc3:
            break;
        case 0xc4:
            break;
        case 0xc5:
            instr = populate_instr(INSTR_PUSH, "PUSH", 11, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0xc6:
            break;
        case 0xc7:
            break;
        case 0xc8:
            break;
        case 0xc9:
            break;
        case 0xca:
            break;
        case 0xcb:
            break;
        case 0xcc:
            break;
        case 0xcd:
            break;
        case 0xce:
            break;
        case 0xcf:
            break;
        case 0xd0:
            break;
        case 0xd1:
            instr = populate_instr(INSTR_POP, "POP", 10, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0xd2:
            break;
        case 0xd3:
            instr = populate_instr(INSTR_OUTPUT, "OUT", 10, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xd4:
            break;
        case 0xd5:
            instr = populate_instr(INSTR_PUSH, "PUSH", 11, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0xd6:
            break;
        case 0xd7:
            break;
        case 0xd8:
            instr = populate_instr(INSTR_INPUT, "IN", 10, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xd9:
            break;
        case 0xda:
            break;
        case 0xdb:
            break;
        case 0xdc:
            break;
        case 0xdd:
            break;
        case 0xde:
            break;
        case 0xdf:
            break;
        case 0xe0:
            break;
        case 0xe1:
            instr = populate_instr(INSTR_POP, "POP", 10, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0xe2:
            break;
        case 0xe3:
            instr = populate_instr(INSTR_EXCHANGE_STACK, "XTHL", 18, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xe4:
            break;
        case 0xe5:
            instr = populate_instr(INSTR_PUSH, "PUSH", 11, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0xe6:
            break;
        case 0xe7:
            break;
        case 0xe8:
            break;
        case 0xe9:
            break;
        case 0xea:
            break;
        case 0xeb:
            instr = populate_instr(INSTR_EXCHANGE_REGS, "XCHG", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xec:
            break;
        case 0xed:
            break;
        case 0xee:
            break;
        case 0xef:
            break;
        case 0xf0:
            break;
        case 0xf1:
            instr = populate_instr(INSTR_POP, "POP", 10, 1,
                                   INSTR_OP_REG_PAIR_PSW);
            break;
        case 0xf2:
            break;
        case 0xf3:
            instr = populate_instr(INSTR_DISABLE_INTERRUPT, "DI", 4, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xf4:
            break;
        case 0xf5:
            instr = populate_instr(INSTR_PUSH, "PUSH", 11, 1,
                                   INSTR_OP_REG_PAIR_PSW);
            break;
        case 0xf6:
            break;
        case 0xf7:
            break;
        case 0xf8:
            instr = populate_instr(INSTR_ENABLE_INTERRUPT, "EI", 4, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xf9:
            instr = populate_instr(INSTR_LOAD_SP_FROM_HL, "SPHL", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xfa:
            break;
        case 0xfb:
            break;
        case 0xfc:
            break;
        case 0xfd:
            break;
        case 0xfe:
            break;
        case 0xff:
            break;
    }

    // Reached end of possible instructions
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
