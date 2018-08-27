
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cpu.h"

uint16_t pc;
uint16_t sp;

uint8_t reg_A;
uint8_t reg_B;
uint8_t reg_C;
uint8_t reg_D;
uint8_t reg_E;
uint8_t reg_H;
uint8_t reg_L;

bool flag_sign;
bool flag_zero;
bool flag_aux_carry;
bool flag_parity;
bool flag_carry;

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
    sp = 0;

    reg_A = 0;
    reg_B = 0;
    reg_C = 0;
    reg_D = 0;
    reg_E = 0;
    reg_H = 0;
    reg_L = 0;

    flag_sign = false;
    flag_zero = false;
    flag_aux_carry = false;
    flag_parity = false;
    flag_carry = false;
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
                instr.move_source = INSTR_OP_REG_B;
                break;
            case 1:
                instr.move_source = INSTR_OP_REG_C;
                break;
            case 2:
                instr.move_source = INSTR_OP_REG_D;
                break;
            case 3:
                instr.move_source = INSTR_OP_REG_E;
                break;
            case 4:
                instr.move_source = INSTR_OP_REG_H;
                break;
            case 5:
                instr.move_source = INSTR_OP_REG_L;
                break;
            case 6:
                instr.move_source = INSTR_OP_MEM_REF;
                break;
            case 7:
                instr.move_source = INSTR_OP_REG_A;
                break;
        }

        switch (op_dest) {
            case 0:
                instr.move_destination = INSTR_OP_REG_B;
                break;
            case 1:
                instr.move_destination = INSTR_OP_REG_C;
                break;
            case 2:
                instr.move_destination = INSTR_OP_REG_D;
                break;
            case 3:
                instr.move_destination = INSTR_OP_REG_E;
                break;
            case 4:
                instr.move_destination = INSTR_OP_REG_H;
                break;
            case 5:
                instr.move_destination = INSTR_OP_REG_L;
                break;
            case 6:
                instr.move_destination = INSTR_OP_MEM_REF;
                break;
            case 7:
                instr.move_destination = INSTR_OP_REG_A;
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
            instr = populate_instr(INSTR_ADD_REG, "ADD", 4, 1, INSTR_OP_REG_B);
            break;
        case 0x81:
            instr = populate_instr(INSTR_ADD_REG, "ADD", 4, 1, INSTR_OP_REG_C);
            break;
        case 0x82:
            instr = populate_instr(INSTR_ADD_REG, "ADD", 4, 1, INSTR_OP_REG_D);
            break;
        case 0x83:
            instr = populate_instr(INSTR_ADD_REG, "ADD", 4, 1, INSTR_OP_REG_E);
            break;
        case 0x84:
            instr = populate_instr(INSTR_ADD_REG, "ADD", 4, 1, INSTR_OP_REG_H);
            break;
        case 0x85:
            instr = populate_instr(INSTR_ADD_REG, "ADD", 4, 1, INSTR_OP_REG_L);
            break;
        case 0x86:
            instr = populate_instr(INSTR_ADD_REG, "ADD", 7, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0x87:
            instr = populate_instr(INSTR_ADD_REG, "ADD", 4, 1, INSTR_OP_REG_A);
            break;
        case 0x88:
            instr = populate_instr(INSTR_ADD_REG_WITH_CARRY, "ADC", 4, 1,
                                   INSTR_OP_REG_B);
            break;
        case 0x89:
            instr = populate_instr(INSTR_ADD_REG_WITH_CARRY, "ADC", 4, 1,
                                   INSTR_OP_REG_C);
            break;
        case 0x8a:
            instr = populate_instr(INSTR_ADD_REG_WITH_CARRY, "ADC", 4, 1,
                                   INSTR_OP_REG_D);
            break;
        case 0x8b:
            instr = populate_instr(INSTR_ADD_REG_WITH_CARRY, "ADC", 4, 1,
                                   INSTR_OP_REG_E);
            break;
        case 0x8c:
            instr = populate_instr(INSTR_ADD_REG_WITH_CARRY, "ADC", 4, 1,
                                   INSTR_OP_REG_H);
            break;
        case 0x8d:
            instr = populate_instr(INSTR_ADD_REG_WITH_CARRY, "ADC", 4, 1,
                                   INSTR_OP_REG_L);
            break;
        case 0x8e:
            instr = populate_instr(INSTR_ADD_REG_WITH_CARRY, "ADC", 7, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0x8f:
            instr = populate_instr(INSTR_ADD_REG_WITH_CARRY, "ADC", 4, 1,
                                   INSTR_OP_REG_A);
            break;
        case 0x90:
            instr = populate_instr(INSTR_SUBTRACT_REG, "SUB", 4, 1,
                                   INSTR_OP_REG_B);
            break;
        case 0x91:
            instr = populate_instr(INSTR_SUBTRACT_REG, "SUB", 4, 1,
                                   INSTR_OP_REG_C);
            break;
        case 0x92:
            instr = populate_instr(INSTR_SUBTRACT_REG, "SUB", 4, 1,
                                   INSTR_OP_REG_D);
            break;
        case 0x93:
            instr = populate_instr(INSTR_SUBTRACT_REG, "SUB", 4, 1,
                                   INSTR_OP_REG_E);
            break;
        case 0x94:
            instr = populate_instr(INSTR_SUBTRACT_REG, "SUB", 4, 1,
                                   INSTR_OP_REG_H);
            break;
        case 0x95:
            instr = populate_instr(INSTR_SUBTRACT_REG, "SUB", 4, 1,
                                   INSTR_OP_REG_L);
            break;
        case 0x96:
            instr = populate_instr(INSTR_SUBTRACT_REG, "SUB", 7, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0x97:
            instr = populate_instr(INSTR_SUBTRACT_REG, "SUB", 4, 1,
                                   INSTR_OP_REG_A);
            break;
        case 0x98:
            instr = populate_instr(INSTR_SUBTRACT_REG_WITH_BORROW, "SBB", 4, 1,
                                   INSTR_OP_REG_B);
            break;
        case 0x99:
            instr = populate_instr(INSTR_SUBTRACT_REG_WITH_BORROW, "SBB", 4, 1,
                                   INSTR_OP_REG_C);
            break;
        case 0x9a:
            instr = populate_instr(INSTR_SUBTRACT_REG_WITH_BORROW, "SBB", 4, 1,
                                   INSTR_OP_REG_D);
            break;
        case 0x9b:
            instr = populate_instr(INSTR_SUBTRACT_REG_WITH_BORROW, "SBB", 4, 1,
                                   INSTR_OP_REG_E);
            break;
        case 0x9c:
            instr = populate_instr(INSTR_SUBTRACT_REG_WITH_BORROW, "SBB", 4, 1,
                                   INSTR_OP_REG_H);
            break;
        case 0x9d:
            instr = populate_instr(INSTR_SUBTRACT_REG_WITH_BORROW, "SBB", 4, 1,
                                   INSTR_OP_REG_L);
            break;
        case 0x9e:
            instr = populate_instr(INSTR_SUBTRACT_REG_WITH_BORROW, "SBB", 7, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0x9f:
            instr = populate_instr(INSTR_SUBTRACT_REG_WITH_BORROW, "SBB", 4, 1,
                                   INSTR_OP_REG_A);
            break;
        case 0xa0:
            instr = populate_instr(INSTR_AND_REG, "ANA", 4, 1,
                                   INSTR_OP_REG_B);
            break;
        case 0xa1:
            instr = populate_instr(INSTR_AND_REG, "ANA", 4, 1,
                                   INSTR_OP_REG_C);
            break;
        case 0xa2:
            instr = populate_instr(INSTR_AND_REG, "ANA", 4, 1,
                                   INSTR_OP_REG_D);
            break;
        case 0xa3:
            instr = populate_instr(INSTR_AND_REG, "ANA", 4, 1,
                                   INSTR_OP_REG_E);
            break;
        case 0xa4:
            instr = populate_instr(INSTR_AND_REG, "ANA", 4, 1,
                                   INSTR_OP_REG_H);
            break;
        case 0xa5:
            instr = populate_instr(INSTR_AND_REG, "ANA", 4, 1,
                                   INSTR_OP_REG_L);
            break;
        case 0xa6:
            instr = populate_instr(INSTR_AND_REG, "ANA", 7, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0xa7:
            instr = populate_instr(INSTR_AND_REG, "ANA", 4, 1,
                                   INSTR_OP_REG_A);
            break;
        case 0xa8:
            instr = populate_instr(INSTR_XOR_REG, "XRA", 4, 1,
                                   INSTR_OP_REG_B);
            break;
        case 0xa9:
            instr = populate_instr(INSTR_XOR_REG, "XRA", 4, 1,
                                   INSTR_OP_REG_C);
            break;
        case 0xaa:
            instr = populate_instr(INSTR_XOR_REG, "XRA", 4, 1,
                                   INSTR_OP_REG_D);
            break;
        case 0xab:
            instr = populate_instr(INSTR_XOR_REG, "XRA", 4, 1,
                                   INSTR_OP_REG_E);
            break;
        case 0xac:
            instr = populate_instr(INSTR_XOR_REG, "XRA", 4, 1,
                                   INSTR_OP_REG_H);
            break;
        case 0xad:
            instr = populate_instr(INSTR_XOR_REG, "XRA", 4, 1,
                                   INSTR_OP_REG_L);
            break;
        case 0xae:
            instr = populate_instr(INSTR_XOR_REG, "XRA", 7, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0xaf:
            instr = populate_instr(INSTR_XOR_REG, "XRA", 4, 1,
                                   INSTR_OP_REG_A);
            break;
        case 0xb0:
            instr = populate_instr(INSTR_OR_REG, "ORA", 4, 1,
                                   INSTR_OP_REG_B);
            break;
        case 0xb1:
            instr = populate_instr(INSTR_OR_REG, "ORA", 4, 1,
                                   INSTR_OP_REG_C);
            break;
        case 0xb2:
            instr = populate_instr(INSTR_OR_REG, "ORA", 4, 1,
                                   INSTR_OP_REG_D);
            break;
        case 0xb3:
            instr = populate_instr(INSTR_OR_REG, "ORA", 4, 1,
                                   INSTR_OP_REG_E);
            break;
        case 0xb4:
            instr = populate_instr(INSTR_OR_REG, "ORA", 4, 1,
                                   INSTR_OP_REG_H);
            break;
        case 0xb5:
            instr = populate_instr(INSTR_OR_REG, "ORA", 4, 1,
                                   INSTR_OP_REG_L);
            break;
        case 0xb6:
            instr = populate_instr(INSTR_OR_REG, "ORA", 7, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0xb7:
            instr = populate_instr(INSTR_OR_REG, "ORA", 4, 1,
                                   INSTR_OP_REG_A);
            break;
        case 0xb8:
            instr = populate_instr(INSTR_COMPARE_REG, "CMP", 4, 1,
                                   INSTR_OP_REG_B);
            break;
        case 0xb9:
            instr = populate_instr(INSTR_COMPARE_REG, "CMP", 4, 1,
                                   INSTR_OP_REG_C);
            break;
        case 0xba:
            instr = populate_instr(INSTR_COMPARE_REG, "CMP", 4, 1,
                                   INSTR_OP_REG_D);
            break;
        case 0xbb:
            instr = populate_instr(INSTR_COMPARE_REG, "CMP", 4, 1,
                                   INSTR_OP_REG_E);
            break;
        case 0xbc:
            instr = populate_instr(INSTR_COMPARE_REG, "CMP", 4, 1,
                                   INSTR_OP_REG_H);
            break;
        case 0xbd:
            instr = populate_instr(INSTR_COMPARE_REG, "CMP", 4, 1,
                                   INSTR_OP_REG_L);
            break;
        case 0xbe:
            instr = populate_instr(INSTR_COMPARE_REG, "CMP", 7, 1,
                                   INSTR_OP_MEM_REF);
            break;
        case 0xbf:
            instr = populate_instr(INSTR_COMPARE_REG, "CMP", 4, 1,
                                   INSTR_OP_REG_A);
            break;
        case 0xc0:
            instr = populate_instr(INSTR_RETURN_IF_NOT_ZERO, "RNZ", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xc1:
            instr = populate_instr(INSTR_POP, "POP", 10, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0xc2:
            instr = populate_instr(INSTR_JUMP_IF_NOT_ZERO, "JNZ", 10, 3,
                                   INSTR_OP_16);
            break;
        case 0xc3:
            instr = populate_instr(INSTR_JUMP, "JMP", 10, 3, INSTR_OP_16);
            break;
        case 0xc4:
            instr = populate_instr(INSTR_CALL_IF_NOT_ZERO, "CNZ", 11, 3,
                                   INSTR_OP_16);
            break;
        case 0xc5:
            instr = populate_instr(INSTR_PUSH, "PUSH", 11, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0xc6:
            instr = populate_instr(INSTR_ADD_IMMEDIATE, "ADI", 7, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xc7:
            instr = populate_instr(INSTR_RESTART_0, "RST", 11, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xc8:
            instr = populate_instr(INSTR_RETURN_IF_ZERO, "RZ", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xc9:
            instr = populate_instr(INSTR_RETURN, "RET", 10, 1, INSTR_OP_NONE);
            break;
        case 0xca:
            instr = populate_instr(INSTR_JUMP_IF_ZERO, "JZ", 10, 3,
                                   INSTR_OP_16);
            break;
        case 0xcb:
            instr = populate_instr(INSTR_JUMP, "JMP", 10, 3, INSTR_OP_16);
            break;
            break;
        case 0xcc:
            instr = populate_instr(INSTR_CALL_IF_ZERO, "CZ", 11, 3,
                                   INSTR_OP_16);
            break;
        case 0xcd:
            instr = populate_instr(INSTR_CALL, "CALL", 17, 3, INSTR_OP_16);
            break;
        case 0xce:
            instr = populate_instr(INSTR_ADD_IMMEDIATE_WITH_CARRY, "ACI", 7, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xcf:
            instr = populate_instr(INSTR_RESTART_1, "RST", 11, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xd0:
            instr = populate_instr(INSTR_RETURN_IF_NO_CARRY, "RNC", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xd1:
            instr = populate_instr(INSTR_POP, "POP", 10, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0xd2:
            instr = populate_instr(INSTR_JUMP_IF_NO_CARRY, "JNC", 10, 3,
                                   INSTR_OP_16);
            break;
        case 0xd3:
            instr = populate_instr(INSTR_OUTPUT, "OUT", 10, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xd4:
            instr = populate_instr(INSTR_CALL_IF_NO_CARRY, "CNC", 11, 3,
                                   INSTR_OP_16);
            break;
        case 0xd5:
            instr = populate_instr(INSTR_PUSH, "PUSH", 11, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0xd6:
            instr = populate_instr(INSTR_SUBTRACT_IMMEDIATE, "SUI", 7, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xd7:
            instr = populate_instr(INSTR_RESTART_2, "RST", 11, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xd8:
            instr = populate_instr(INSTR_RETURN_IF_CARRY, "RC", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xd9:
            instr = populate_instr(INSTR_RETURN, "RET", 10, 1, INSTR_OP_NONE);
            break;
        case 0xda:
            instr = populate_instr(INSTR_JUMP_IF_CARRY, "JC", 10, 3,
                                   INSTR_OP_16);
            break;
        case 0xdb:
            instr = populate_instr(INSTR_INPUT, "IN", 10, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xdc:
            instr = populate_instr(INSTR_CALL_IF_CARRY, "CC", 11, 3,
                                   INSTR_OP_16);
            break;
        case 0xdd:
            instr = populate_instr(INSTR_CALL, "CALL", 17, 3, INSTR_OP_16);
            break;
        case 0xde:
            instr = populate_instr(INSTR_SUBTRACT_IMMEDIATE_WITH_BORROW, "SBI",
                                   7, 2, INSTR_OP_SINGLE_8);
            break;
        case 0xdf:
            instr = populate_instr(INSTR_RESTART_3, "RST", 11, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xe0:
            instr = populate_instr(INSTR_RETURN_IF_PARITY_ODD, "RPO", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xe1:
            instr = populate_instr(INSTR_POP, "POP", 10, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0xe2:
            instr = populate_instr(INSTR_JUMP_IF_PARITY_ODD, "JPO", 10, 3,
                                   INSTR_OP_16);
            break;
        case 0xe3:
            instr = populate_instr(INSTR_EXCHANGE_STACK, "XTHL", 18, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xe4:
            instr = populate_instr(INSTR_CALL_IF_PARITY_ODD, "CPO", 11, 3,
                                   INSTR_OP_16);
            break;
        case 0xe5:
            instr = populate_instr(INSTR_PUSH, "PUSH", 11, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0xe6:
            instr = populate_instr(INSTR_AND_IMMEDIATE, "ANI", 7, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xe7:
            instr = populate_instr(INSTR_RESTART_4, "RST", 11, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xe8:
            instr = populate_instr(INSTR_RETURN_IF_PARITY_EVEN, "RPE", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xe9:
            instr = populate_instr(INSTR_LOAD_PROGRAM_COUNTER, "PCHL", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xea:
            instr = populate_instr(INSTR_JUMP_IF_PARITY_EVEN, "JPE", 10, 3,
                                   INSTR_OP_16);
            break;
        case 0xeb:
            instr = populate_instr(INSTR_EXCHANGE_REGS, "XCHG", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xec:
            instr = populate_instr(INSTR_CALL_IF_PARITY_EVEN, "CPE", 11, 3,
                                   INSTR_OP_16);
            break;
        case 0xed:
            instr = populate_instr(INSTR_CALL, "CALL", 17, 3, INSTR_OP_16);
            break;
        case 0xee:
            instr = populate_instr(INSTR_XOR_IMMEDIATE, "XRI", 7, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xef:
            instr = populate_instr(INSTR_RESTART_5, "RST", 11, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xf0:
            instr = populate_instr(INSTR_RETURN_IF_PLUS, "RP", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xf1:
            instr = populate_instr(INSTR_POP, "POP", 10, 1,
                                   INSTR_OP_REG_PAIR_PSW);
            break;
        case 0xf2:
            instr = populate_instr(INSTR_JUMP_IF_PLUS, "JP", 10, 3,
                                   INSTR_OP_16);
            break;
        case 0xf3:
            instr = populate_instr(INSTR_DISABLE_INTERRUPT, "DI", 4, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xf4:
            instr = populate_instr(INSTR_CALL_IF_PLUS, "CP", 11, 3,
                                   INSTR_OP_16);
            break;
        case 0xf5:
            instr = populate_instr(INSTR_PUSH, "PUSH", 11, 1,
                                   INSTR_OP_REG_PAIR_PSW);
            break;
        case 0xf6:
            instr = populate_instr(INSTR_OR_IMMEDIATE, "ORI", 7, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xf7:
            instr = populate_instr(INSTR_RESTART_6, "RST", 11, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xf8:
            instr = populate_instr(INSTR_RETURN_IF_PARITY_EVEN, "RPE", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xf9:
            instr = populate_instr(INSTR_LOAD_SP_FROM_HL, "SPHL", 5, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xfa:
            instr = populate_instr(INSTR_JUMP_IF_MINUS, "JM", 10, 3,
                                   INSTR_OP_16);
            break;
        case 0xfb:
            instr = populate_instr(INSTR_ENABLE_INTERRUPT, "EI", 4, 1,
                                   INSTR_OP_NONE);
            break;
        case 0xfc:
            instr = populate_instr(INSTR_CALL_IF_MINUS, "CM", 11, 3,
                                   INSTR_OP_16);
            break;
        case 0xfd:
            instr = populate_instr(INSTR_CALL, "CALL", 17, 3, INSTR_OP_16);
            break;
        case 0xfe:
            instr = populate_instr(INSTR_COMPARE_IMMEDIATE, "CPI", 7, 2,
                                   INSTR_OP_SINGLE_8);
            break;
        case 0xff:
            instr = populate_instr(INSTR_RESTART_7, "RST", 11, 1,
                                   INSTR_OP_NONE);
            break;
    }

    // Reached end of possible instructions
    if (instr.cycle_count == 999) {
        printf("Error: Unrecognized opcode: 0x%02x\n", instr.opcode);
        exit(1);
    }

    return instr;
}

void no_logic(Instr instr) {
    printf("Instruction logic not implemented.\nOpcode: 0x%02x\n",
            instr.opcode);
    exit(0);
}

uint8_t *get_reg_op(InstrOpType op_type) {
    switch (op_type) {
        case INSTR_OP_REG_B:
            return &reg_B;
            break;
        case INSTR_OP_REG_C:
            return &reg_C;
            break;
        case INSTR_OP_REG_D:
            return &reg_D;
            break;
        case INSTR_OP_REG_E:
            return &reg_E;
            break;
        case INSTR_OP_REG_H:
            return &reg_H;
            break;
        case INSTR_OP_REG_L:
            return &reg_L;
            break;
        case INSTR_OP_REG_A:
            return &reg_A;
            break;
        case INSTR_OP_MEM_REF:
            return &memory[(reg_H << 8) + reg_L];
            break;
        default:
            printf("Error: Operand type not recognized\n");
            exit(0);
            break;
    }
}

bool is_parity_even(uint8_t val) {
    bool parity = true;
    while (val) {
        parity ^= val & 0x1;
        val = val >> 1;
    }
    return parity;
}

void calculate_non_carry_flags(uint8_t val) {
    flag_zero = (val == 0);
    flag_sign = val >> 7;
    flag_parity = is_parity_even(val);
}

uint16_t get_swapped_bytes(uint16_t val) {
    uint16_t swapped = (val & 0xf0) >> 8;
    swapped = swapped | ((val & 0x0f) << 8);
    return swapped;
}

void exec_instr(Instr instr) {

    pc += instr.byte_count;

    switch (instr.type) {
        case INSTR_NOP:
            break;
        case INSTR_HALT:
            no_logic(instr);
            break;
        case INSTR_DISABLE_INTERRUPT:
            no_logic(instr);
            break;
        case INSTR_ENABLE_INTERRUPT:
            no_logic(instr);
            break;
        case INSTR_OUTPUT:
            no_logic(instr);
            break;
        case INSTR_INPUT:
            no_logic(instr);
            break;
        case INSTR_DOUBLE_ADD:
            no_logic(instr);
            break;
        case INSTR_INCREMENT_REG_PAIR:
            no_logic(instr);
            break;
        case INSTR_DECREMENT_REG_PAIR:
            no_logic(instr);
            break;
        case INSTR_POP:
            no_logic(instr);
            break;
        case INSTR_PUSH:
            no_logic(instr);
            break;
        case INSTR_EXCHANGE_STACK: {
            uint8_t temp;
            temp = reg_H;
            reg_H = memory[sp + 1];
            memory[sp + 1] = temp;
            temp = reg_L;
            reg_L = memory[sp];
            memory[sp] = temp;
            break;
        }
        case INSTR_LOAD_SP_FROM_HL:
            sp = ((uint16_t)reg_H << 8) | reg_L;
            break;
        case INSTR_EXCHANGE_REGS: {
            uint8_t temp;
            temp = reg_H;
            reg_H = reg_D;
            reg_D = temp;
            temp = reg_L;
            reg_L = reg_E;
            reg_E = temp;
            break;
        }
        case INSTR_LOAD_HL_DIRECT:
            no_logic(instr);
            break;
        case INSTR_STORE_HL_DIRECT:
            no_logic(instr);
            break;
        case INSTR_LOAD_REG_PAIR_IMMEDIATE:
            no_logic(instr);
            break;
        case INSTR_STORE_ACCUMULATOR: {
            if (instr.op_type == INSTR_OP_REG_B) {
                memory[((uint16_t)reg_B << 8) | reg_C] = reg_A;
            } else if (instr.op_type == INSTR_OP_REG_D) {
                memory[((uint16_t)reg_D << 8) | reg_E] = reg_A;
            }
            break;
        }
        case INSTR_LOAD_ACCUMULATOR: {
            if (instr.op_type == INSTR_OP_REG_B) {
                reg_A = memory[((uint16_t)reg_B << 8) | reg_C];
            } else if (instr.op_type == INSTR_OP_REG_D) {
                reg_A = memory[((uint16_t)reg_D << 8) | reg_E];
            }
            break;
        }
        case INSTR_STORE_ACCUMULATOR_DIRECT:
            no_logic(instr);
            break;
        case INSTR_LOAD_ACCUMULATOR_DIRECT:
            no_logic(instr);
            break;
        case INSTR_MOVE_IMMEDIATE:
            no_logic(instr);
            break;
        case INSTR_MOVE: {
            uint8_t *source_ptr = get_reg_op(instr.move_source);
            uint8_t *dest_ptr = get_reg_op(instr.move_destination);
            *dest_ptr = *source_ptr;
            break;
        }
        case INSTR_INCREMENT_REG: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            uint8_t val = *op_ptr + 1;
            calculate_non_carry_flags(val);
            flag_aux_carry = ((*op_ptr & 0xf) + 1) > 0xf;
            *op_ptr = val;
            break;
        }
        case INSTR_DECREMENT_REG: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            uint8_t val = *op_ptr - 1;
            calculate_non_carry_flags(val);
            flag_aux_carry = ((*op_ptr & 0xf) - 1) > 0xf;
            *op_ptr = val;
            break;
        }
        case INSTR_ROTATE_ACCUMULATOR_LEFT:
            flag_carry = reg_A & 0x80;
            reg_A = reg_A << 1;
            reg_A = reg_A | (flag_carry ? 0x01 : 0x00);
            break;
        case INSTR_ROTATE_ACCUMULATOR_RIGHT:
            flag_carry = reg_A & 0x01;
            reg_A = reg_A >> 1;
            reg_A = reg_A | (flag_carry ? 0x80 : 0x00);
            break;
        case INSTR_ROTATE_ACCUMULATOR_LEFT_CARRY: {
            bool old_carry = flag_carry;
            flag_carry = reg_A & 0x80;
            reg_A = reg_A << 1;
            reg_A = reg_A | (old_carry ? 0x01 : 0x00);
            break;
        }
        case INSTR_ROTATE_ACCUMULATOR_RIGHT_CARRY: {
            bool old_carry = flag_carry;
            flag_carry = reg_A & 0x01;
            reg_A = reg_A >> 1;
            reg_A = reg_A | (old_carry ? 0x80 : 0x00);
            break;
        }
        case INSTR_DECIMAL_ADJUST_ACCUMULATOR:
            no_logic(instr);
            break;
        case INSTR_COMPLEMENT_ACCUMULATOR:
            reg_A = ~reg_A;
            break;
        case INSTR_SET_CARRY:
            flag_carry = true;
            break;
        case INSTR_COMPLEMENT_CARRY:
            flag_carry = !flag_carry;
            break;
        case INSTR_RESTART_0:
            no_logic(instr);
            break;
        case INSTR_RESTART_1:
            no_logic(instr);
            break;
        case INSTR_RESTART_2:
            no_logic(instr);
            break;
        case INSTR_RESTART_3:
            no_logic(instr);
            break;
        case INSTR_RESTART_4:
            no_logic(instr);
            break;
        case INSTR_RESTART_5:
            no_logic(instr);
            break;
        case INSTR_RESTART_6:
            no_logic(instr);
            break;
        case INSTR_RESTART_7:
            no_logic(instr);
            break;
        case INSTR_CALL:
            no_logic(instr);
            break;
        case INSTR_CALL_IF_CARRY:
            no_logic(instr);
            break;
        case INSTR_CALL_IF_NO_CARRY:
            no_logic(instr);
            break;
        case INSTR_CALL_IF_ZERO:
            no_logic(instr);
            break;
        case INSTR_CALL_IF_NOT_ZERO:
            no_logic(instr);
            break;
        case INSTR_CALL_IF_MINUS:
            no_logic(instr);
            break;
        case INSTR_CALL_IF_PLUS:
            no_logic(instr);
            break;
        case INSTR_CALL_IF_PARITY_EVEN:
            no_logic(instr);
            break;
        case INSTR_CALL_IF_PARITY_ODD:
            no_logic(instr);
            break;
        case INSTR_LOAD_PROGRAM_COUNTER:
            no_logic(instr);
            break;
        case INSTR_JUMP: {
            uint16_t address = get_swapped_bytes(instr.operand_16);
            pc = memory[address];
            break;
        }
        case INSTR_JUMP_IF_CARRY:
            no_logic(instr);
            break;
        case INSTR_JUMP_IF_NO_CARRY:
            no_logic(instr);
            break;
        case INSTR_JUMP_IF_ZERO:
            no_logic(instr);
            break;
        case INSTR_JUMP_IF_NOT_ZERO:
            no_logic(instr);
            break;
        case INSTR_JUMP_IF_MINUS:
            no_logic(instr);
            break;
        case INSTR_JUMP_IF_PLUS:
            no_logic(instr);
            break;
        case INSTR_JUMP_IF_PARITY_EVEN:
            no_logic(instr);
            break;
        case INSTR_JUMP_IF_PARITY_ODD:
            no_logic(instr);
            break;
        case INSTR_RETURN:
            no_logic(instr);
            break;
        case INSTR_RETURN_IF_CARRY:
            no_logic(instr);
            break;
        case INSTR_RETURN_IF_NO_CARRY:
            no_logic(instr);
            break;
        case INSTR_RETURN_IF_ZERO:
            no_logic(instr);
            break;
        case INSTR_RETURN_IF_NOT_ZERO:
            no_logic(instr);
            break;
        case INSTR_RETURN_IF_MINUS:
            no_logic(instr);
            break;
        case INSTR_RETURN_IF_PLUS:
            no_logic(instr);
            break;
        case INSTR_RETURN_IF_PARITY_EVEN:
            no_logic(instr);
            break;
        case INSTR_RETURN_IF_PARITY_ODD:
            no_logic(instr);
            break;
        case INSTR_ADD_REG:
            no_logic(instr);
            break;
        case INSTR_ADD_REG_WITH_CARRY:
            no_logic(instr);
            break;
        case INSTR_SUBTRACT_REG:
            no_logic(instr);
            break;
        case INSTR_SUBTRACT_REG_WITH_BORROW:
            no_logic(instr);
            break;
        case INSTR_AND_REG:
            no_logic(instr);
            break;
        case INSTR_XOR_REG:
            no_logic(instr);
            break;
        case INSTR_OR_REG:
            no_logic(instr);
            break;
        case INSTR_COMPARE_REG:
            no_logic(instr);
            break;
        case INSTR_ADD_IMMEDIATE:
            no_logic(instr);
            break;
        case INSTR_ADD_IMMEDIATE_WITH_CARRY:
            no_logic(instr);
            break;
        case INSTR_SUBTRACT_IMMEDIATE:
            no_logic(instr);
            break;
        case INSTR_SUBTRACT_IMMEDIATE_WITH_BORROW:
            no_logic(instr);
            break;
        case INSTR_AND_IMMEDIATE:
            no_logic(instr);
            break;
        case INSTR_XOR_IMMEDIATE:
            no_logic(instr);
            break;
        case INSTR_OR_IMMEDIATE:
            no_logic(instr);
            break;
        case INSTR_COMPARE_IMMEDIATE:
            no_logic(instr);
            break;
    }
}

