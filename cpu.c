
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cpu.h"

static uint16_t pc = 0;
static uint16_t sp = 0;

static uint8_t reg_A = 0;
static uint8_t reg_B = 0;
static uint8_t reg_C = 0;
static uint8_t reg_D = 0;
static uint8_t reg_E = 0;
static uint8_t reg_H = 0;
static uint8_t reg_L = 0;

static bool flag_sign = false;
static bool flag_zero = false;
static bool flag_aux_carry = false;
static bool flag_parity = false;
static bool flag_carry = false;

static uint8_t *memory = NULL;

void init_cpu(uint8_t *mem) {
    memory = mem;

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

CpuInnards expose_cpu_internals() {
    CpuInnards cpu;

    cpu.pc = &pc;
    cpu.sp = &sp;

    cpu.reg_A = &reg_A;
    cpu.reg_B = &reg_B;
    cpu.reg_C = &reg_C;
    cpu.reg_D = &reg_D;
    cpu.reg_E = &reg_E;
    cpu.reg_H = &reg_H;
    cpu.reg_L = &reg_L;

    cpu.flag_sign = &flag_sign;
    cpu.flag_zero = &flag_zero;
    cpu.flag_aux_carry = &flag_aux_carry;
    cpu.flag_parity = &flag_parity;
    cpu.flag_carry = &flag_carry;

    return cpu;
}

Instr populate_instr(InstrType type, char *mnemonic, int cycle_count,
        int byte_count, InstrOpType op_type) {
    Instr instr;

    instr.address = pc;
    instr.opcode = memory[pc];
    instr.type = type;
    strcpy(instr.mnemonic, mnemonic);
    instr.cycle_count = cycle_count;
    instr.byte_count = byte_count;

    instr.op_type = op_type;
    if (op_type == INSTR_OP_REG_B_AND_OP_8 ||
        op_type == INSTR_OP_REG_C_AND_OP_8 ||
        op_type == INSTR_OP_REG_D_AND_OP_8 ||
        op_type == INSTR_OP_REG_E_AND_OP_8 ||
        op_type == INSTR_OP_REG_H_AND_OP_8 ||
        op_type == INSTR_OP_REG_L_AND_OP_8 ||
        op_type == INSTR_OP_REG_A_AND_OP_8 ||
        op_type == INSTR_OP_MEM_REF_AND_OP_8 ||
        op_type == INSTR_OP_SINGLE_8) {
        instr.operand_8_1 = memory[pc+1];
    }
    if (op_type == INSTR_OP_DOUBLE_8) {
        instr.operand_8_1 = memory[pc+1];
        instr.operand_8_2 = memory[pc+2];
    }
    if (op_type == INSTR_OP_REG_PAIR_B_AND_OP_16 ||
        op_type == INSTR_OP_REG_PAIR_D_AND_OP_16 ||
        op_type == INSTR_OP_REG_PAIR_H_AND_OP_16 ||
        op_type == INSTR_OP_REG_PAIR_SP_AND_OP_16 ||
        op_type == INSTR_OP_16) {
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
    strcpy(instr.mnemonic, "999");
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
            instr = populate_instr(INSTR_ROTATE_ACCUMULATOR_LEFT, "RLC",
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
            instr = populate_instr(INSTR_ROTATE_ACCUMULATOR_RIGHT, "RRC",
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
            instr = populate_instr(INSTR_ROTATE_ACCUMULATOR_LEFT_CARRY, "RAL",
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
            instr = populate_instr(INSTR_ROTATE_ACCUMULATOR_RIGHT_CARRY, "RAR",
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
            instr = populate_instr(INSTR_RETURN_IF_MINUS, "RPE", 5, 1,
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
        case INSTR_OP_REG_B_AND_OP_8:
            return &reg_B;
            break;
        case INSTR_OP_REG_C:
        case INSTR_OP_REG_C_AND_OP_8:
            return &reg_C;
            break;
        case INSTR_OP_REG_D:
        case INSTR_OP_REG_D_AND_OP_8:
            return &reg_D;
            break;
        case INSTR_OP_REG_E:
        case INSTR_OP_REG_E_AND_OP_8:
            return &reg_E;
            break;
        case INSTR_OP_REG_H:
        case INSTR_OP_REG_H_AND_OP_8:
            return &reg_H;
            break;
        case INSTR_OP_REG_L:
        case INSTR_OP_REG_L_AND_OP_8:
            return &reg_L;
            break;
        case INSTR_OP_REG_A:
        case INSTR_OP_REG_A_AND_OP_8:
            return &reg_A;
            break;
        case INSTR_OP_MEM_REF:
        case INSTR_OP_MEM_REF_AND_OP_8:
            return &memory[(reg_H << 8) | reg_L];
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
    uint16_t swapped = ((uint16_t)val & 0xff00) >> 8;
    swapped = swapped | ((val & 0xff) << 8);
    return swapped;
}

uint8_t get_flag_reg() {
    uint8_t val;
    val = flag_sign << 7;
    val |= flag_zero << 6;
    val |= 0 << 5;
    val |= flag_aux_carry << 4;
    val |= 0 << 3;
    val |= flag_parity << 2;
    val |= 1 << 1;
    val |= flag_carry;
    return val;
}

void set_flag_reg(uint8_t val) {
    flag_sign = (val >> 7) & 0x1;
    flag_zero = (val >> 6) & 0x1;
    flag_aux_carry = (val >> 4) & 0x1;
    flag_parity = (val >> 2) & 0x1;
    flag_carry = val & 0x1;
}

void push(uint16_t val) {
    sp = (sp - 2) & 0xffff;
    memory[sp + 1] = (val >> 8);
    memory[sp] = (val & 0xff);
}

uint16_t pop() {
    uint16_t val;
    val = ((uint16_t)memory[sp + 1] << 8);
    val |= memory[sp];
    sp = sp + 2;
    return val;
}

void call_sub(uint16_t address) {
    push(pc);
    pc = address;
}

void return_sub() {
    pc = pop();
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
            //no_logic(instr);
            break;
        case INSTR_ENABLE_INTERRUPT:
            //no_logic(instr);
            break;
        case INSTR_OUTPUT:
            //no_logic(instr);
            break;
        case INSTR_INPUT:
            //no_logic(instr);
            break;
        case INSTR_DOUBLE_ADD:
            if (instr.op_type == INSTR_OP_REG_PAIR_B) {
                uint32_t op1 = ((uint32_t)reg_B << 8) | reg_C;
                uint32_t op2 = ((uint32_t)reg_H << 8) | reg_L;
                uint32_t sum = op1 + op2;
                flag_carry = sum >> 16;
                reg_H = (sum >> 8) & 0xff;
                reg_L = sum & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_D) {
                uint32_t op1 = ((uint32_t)reg_D << 8) | reg_E;
                uint32_t op2 = ((uint32_t)reg_H << 8) | reg_L;
                uint32_t sum = op1 + op2;
                flag_carry = sum >> 16;
                reg_H = (sum >> 8) & 0xff;
                reg_L = sum & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_H) {
                uint32_t op1 = ((uint32_t)reg_H << 8) | reg_L;
                uint32_t op2 = ((uint32_t)reg_H << 8) | reg_L;
                uint32_t sum = op1 + op2;
                flag_carry = sum >> 16;
                reg_H = (sum >> 8) & 0xff;
                reg_L = sum & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_SP) {
                uint32_t op1 = sp;
                uint32_t op2 = ((uint32_t)reg_H << 8) | reg_L;
                uint32_t sum = op1 + op2;
                flag_carry = sum >> 16;
                reg_H = (sum >> 8) & 0xff;
                reg_L = sum & 0xff;
            }
            break;
        case INSTR_INCREMENT_REG_PAIR: {
            uint16_t val;
            if (instr.op_type == INSTR_OP_REG_PAIR_B) {
                val = ((uint16_t)reg_B << 8) | reg_C;
                val++;
                reg_B = val >> 8;
                reg_C = val & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_D) {
                val = ((uint16_t)reg_D << 8) | reg_E;
                val++;
                reg_D = val >> 8;
                reg_E = val & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_H) {
                val = ((uint16_t)reg_H << 8) | reg_L;
                val++;
                reg_H = val >> 8;
                reg_L = val & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_SP) {
                sp++;
            }
            break;
        }
        case INSTR_DECREMENT_REG_PAIR: {
            uint16_t val;
            if (instr.op_type == INSTR_OP_REG_PAIR_B) {
                val = ((uint16_t)reg_B << 8) | reg_C;
                val--;
                reg_B = val >> 8;
                reg_C = val & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_D) {
                val = ((uint16_t)reg_D << 8) | reg_E;
                val--;
                reg_D = val >> 8;
                reg_E = val & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_H) {
                val = ((uint16_t)reg_H << 8) | reg_L;
                val--;
                reg_H = val >> 8;
                reg_L = val & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_SP) {
                sp--;
            }
            break;
        }
        case INSTR_POP:
            if (instr.op_type == INSTR_OP_REG_PAIR_B) {
                uint16_t val = pop();
                reg_B = val >> 8;
                reg_C = val & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_D) {
                uint16_t val = pop();
                reg_D = val >> 8;
                reg_E = val & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_H) {
                uint16_t val = pop();
                reg_H = val >> 8;
                reg_L = val & 0xff;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_PSW) {
                uint16_t val = pop();
                reg_A = val >> 8;
                set_flag_reg(val & 0xff);
            }
            break;
        case INSTR_PUSH:
            if (instr.op_type == INSTR_OP_REG_PAIR_B) {
                uint16_t val = ((uint16_t)reg_B << 8) | reg_C;
                push(val);
            } else if (instr.op_type == INSTR_OP_REG_PAIR_D) {
                uint16_t val = ((uint16_t)reg_D << 8) | reg_E;
                push(val);
            } else if (instr.op_type == INSTR_OP_REG_PAIR_H) {
                uint16_t val = ((uint16_t)reg_H << 8) | reg_L;
                push(val);
            } else if (instr.op_type == INSTR_OP_REG_PAIR_PSW) {
                uint16_t val = ((uint16_t)reg_A << 8) | get_flag_reg();
                push(val);
            }
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
        case INSTR_LOAD_HL_DIRECT: {
            uint16_t address = get_swapped_bytes(instr.operand_16);
            reg_H = memory[address + 1];
            reg_L = memory[address];
            break;
        }
        case INSTR_STORE_HL_DIRECT: {
            uint16_t address = get_swapped_bytes(instr.operand_16);
            memory[address] = reg_L;
            memory[address + 1] = reg_H;
            break;
        }
        case INSTR_LOAD_REG_PAIR_IMMEDIATE:
            if (instr.op_type == INSTR_OP_REG_PAIR_B_AND_OP_16) {
                reg_B = instr.operand_16 & 0xff;
                reg_C = instr.operand_16 >> 8;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_D_AND_OP_16) {
                reg_D = instr.operand_16 & 0xff;
                reg_E = instr.operand_16 >> 8;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_H_AND_OP_16) {
                reg_H = instr.operand_16 & 0xff;
                reg_L = instr.operand_16 >> 8;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_SP_AND_OP_16) {
                sp = (instr.operand_16 & 0xff) << 8;
                sp |= instr.operand_16 >> 8;
            }
            break;
        case INSTR_STORE_ACCUMULATOR: {
            if (instr.op_type == INSTR_OP_REG_PAIR_B) {
                memory[((uint16_t)reg_B << 8) | reg_C] = reg_A;
            } else if (instr.op_type == INSTR_OP_REG_PAIR_D) {
                memory[((uint16_t)reg_D << 8) | reg_E] = reg_A;
            }
            break;
        }
        case INSTR_LOAD_ACCUMULATOR: {
            if (instr.op_type == INSTR_OP_REG_PAIR_B) {
                uint16_t address = ((uint16_t)reg_B << 8) | reg_C;
                reg_A = memory[address];
            } else if (instr.op_type == INSTR_OP_REG_PAIR_D) {
                uint16_t address = ((uint16_t)reg_D << 8) | reg_E;
                reg_A = memory[address];
            }
            break;
        }
        case INSTR_STORE_ACCUMULATOR_DIRECT:
            memory[get_swapped_bytes(instr.operand_16)] = reg_A;
            break;
        case INSTR_LOAD_ACCUMULATOR_DIRECT:
            reg_A = memory[get_swapped_bytes(instr.operand_16)];
            break;
        case INSTR_MOVE_IMMEDIATE: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            *op_ptr = instr.operand_8_1;
            break;
        }
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
            flag_aux_carry = (val & 0x0f) == 0;
            *op_ptr = val;
            break;
        }
        case INSTR_DECREMENT_REG: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            uint8_t val = *op_ptr - 1;
            calculate_non_carry_flags(val);
            flag_aux_carry = !((val & 0x0f) == 0x0f);
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
        case INSTR_DECIMAL_ADJUST_ACCUMULATOR: {
            uint8_t low = reg_A & 0xf;
            if ((low > 9) || flag_aux_carry == true) {
                reg_A += 0x6;
                flag_aux_carry = (low + 6) > 0xf;
            }
            uint8_t high = (reg_A & 0xf0) >> 4;
            if ((high > 9) || flag_carry == true) {
                reg_A += 0x60;
                flag_carry = (high + 6) > 0xf;
            }
            break;
        }
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
            call_sub(0x00);
            break;
        case INSTR_RESTART_1:
            call_sub(0x08);
            break;
        case INSTR_RESTART_2:
            call_sub(0x10);
            break;
        case INSTR_RESTART_3:
            call_sub(0x18);
            break;
        case INSTR_RESTART_4:
            call_sub(0x20);
            break;
        case INSTR_RESTART_5:
            call_sub(0x28);
            break;
        case INSTR_RESTART_6:
            call_sub(0x30);
            break;
        case INSTR_RESTART_7:
            call_sub(0x38);
            break;
        case INSTR_CALL:
            call_sub(get_swapped_bytes(instr.operand_16));
            break;
        case INSTR_CALL_IF_CARRY:
            if (flag_carry) {
                call_sub(get_swapped_bytes(instr.operand_16));
            }
            break;
        case INSTR_CALL_IF_NO_CARRY:
            if (!flag_carry) {
                call_sub(get_swapped_bytes(instr.operand_16));
            }
            break;
        case INSTR_CALL_IF_ZERO:
            if (flag_zero) {
                call_sub(get_swapped_bytes(instr.operand_16));
            }
            break;
        case INSTR_CALL_IF_NOT_ZERO:
            if (!flag_zero) {
                call_sub(get_swapped_bytes(instr.operand_16));
            }
            break;
        case INSTR_CALL_IF_MINUS:
            if (flag_sign) {
                call_sub(get_swapped_bytes(instr.operand_16));
            }
            break;
        case INSTR_CALL_IF_PLUS:
            if (!flag_sign) {
                call_sub(get_swapped_bytes(instr.operand_16));
            }
            break;
        case INSTR_CALL_IF_PARITY_EVEN:
            if (flag_parity) {
                call_sub(get_swapped_bytes(instr.operand_16));
            }
            break;
        case INSTR_CALL_IF_PARITY_ODD:
            if (!flag_parity) {
                call_sub(get_swapped_bytes(instr.operand_16));
            }
            break;
        case INSTR_LOAD_PROGRAM_COUNTER: {
            uint16_t address = ((uint16_t)reg_H << 8) | reg_L;
            pc = address;
            break;
        }
        case INSTR_JUMP:
            pc = get_swapped_bytes(instr.operand_16);
            break;
        case INSTR_JUMP_IF_CARRY:
            if (flag_carry) {
                pc = get_swapped_bytes(instr.operand_16);
            }
            break;
        case INSTR_JUMP_IF_NO_CARRY:
            if (!flag_carry) {
                pc = get_swapped_bytes(instr.operand_16);
            }
            break;
        case INSTR_JUMP_IF_ZERO:
            if (flag_zero) {
                pc = get_swapped_bytes(instr.operand_16);
            }
            break;
        case INSTR_JUMP_IF_NOT_ZERO:
            if (!flag_zero) {
                pc = get_swapped_bytes(instr.operand_16);
            }
            break;
        case INSTR_JUMP_IF_MINUS:
            if (flag_sign) {
                pc = get_swapped_bytes(instr.operand_16);
            }
            break;
        case INSTR_JUMP_IF_PLUS:
            if (!flag_sign) {
                pc = get_swapped_bytes(instr.operand_16);
            }
            break;
        case INSTR_JUMP_IF_PARITY_EVEN:
            if (flag_parity) {
                pc = get_swapped_bytes(instr.operand_16);
            }
            break;
        case INSTR_JUMP_IF_PARITY_ODD:
            if (!flag_parity) {
                pc = get_swapped_bytes(instr.operand_16);
            }
            break;
        case INSTR_RETURN:
            return_sub();
            break;
        case INSTR_RETURN_IF_CARRY:
            if (flag_carry) {
                return_sub();
            }
            break;
        case INSTR_RETURN_IF_NO_CARRY:
            if (!flag_carry) {
                return_sub();
            }
            break;
        case INSTR_RETURN_IF_ZERO:
            if (flag_zero) {
                return_sub();
            }
            break;
        case INSTR_RETURN_IF_NOT_ZERO:
            if (!flag_zero) {
                return_sub();
            }
            break;
        case INSTR_RETURN_IF_MINUS:
            if (flag_sign) {
                return_sub();
            }
            break;
        case INSTR_RETURN_IF_PLUS:
            if (!flag_sign) {
                return_sub();
            }
            break;
        case INSTR_RETURN_IF_PARITY_EVEN:
            if (flag_parity) {
                return_sub();
            }
            break;
        case INSTR_RETURN_IF_PARITY_ODD:
            if (!flag_parity) {
                return_sub();
            }
            break;
        case INSTR_ADD_REG: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            uint16_t val = (uint16_t)*op_ptr + (uint16_t)reg_A;
            calculate_non_carry_flags(val & 0xff);
            flag_carry = (val >> 8) > 0;
            flag_aux_carry = ((*op_ptr & 0xf) + (reg_A & 0xf)) > 0xf;
            reg_A = val;
            break;
        }
        case INSTR_ADD_REG_WITH_CARRY: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            uint16_t val = (uint16_t)*op_ptr + (uint16_t)reg_A + flag_carry;
            calculate_non_carry_flags(val & 0xff);
            flag_aux_carry = ((*op_ptr & 0xf) + (reg_A & 0xf) 
                             + flag_carry) > 0xf;
            flag_carry = (val >> 8) > 0;
            reg_A = val;
            break;
        }
        case INSTR_SUBTRACT_REG: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            uint16_t val = ~(uint16_t)*op_ptr + 1 + (uint16_t)reg_A;
            calculate_non_carry_flags(val & 0xff);
            flag_carry = (val >> 8) > 0;
            flag_aux_carry = (~(*op_ptr & 0xf) + 1 + (reg_A & 0xf)) > 0xf;
            reg_A = val;
            break;
        }
        case INSTR_SUBTRACT_REG_WITH_BORROW: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            uint16_t val = ~((uint16_t)*op_ptr + flag_carry) + 1 
                           + (uint16_t)reg_A;
            calculate_non_carry_flags(val & 0xff);
            flag_aux_carry = (~(((*op_ptr) + flag_carry) & 0xf) + 1 
                              + (reg_A & 0xf)) > 0xf;
            flag_carry = (val >> 8) > 0;
            reg_A = val;
            break;
        }
        case INSTR_AND_REG: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            reg_A = reg_A & *op_ptr;
            calculate_non_carry_flags(reg_A);
            flag_carry = false;
            break;
        }
        case INSTR_XOR_REG: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            reg_A = reg_A ^ *op_ptr;
            calculate_non_carry_flags(reg_A);
            flag_carry = false;
            break;
        }
        case INSTR_OR_REG: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            reg_A = reg_A | *op_ptr;
            calculate_non_carry_flags(reg_A);
            flag_carry = false;
            break;
        }
        case INSTR_COMPARE_REG: {
            uint8_t *op_ptr = get_reg_op(instr.op_type);
            uint16_t val = ~(uint16_t)*op_ptr + 1 + (uint16_t)reg_A;
            calculate_non_carry_flags(val & 0xff);
            flag_carry = (val >> 8) > 0;
            flag_aux_carry = (~(*op_ptr & 0xf) + 1 + (reg_A & 0xf)) > 0xf;
            break;
        }
        case INSTR_ADD_IMMEDIATE: {
            uint16_t val = (uint16_t)instr.operand_8_1 + (uint16_t)reg_A;
            calculate_non_carry_flags(val & 0xff);
            flag_carry = (val >> 8) > 0;
            flag_aux_carry = ((instr.operand_8_1 & 0xf) + (reg_A & 0xf)) > 0xf;
            reg_A = val;
            break;
        }
        case INSTR_ADD_IMMEDIATE_WITH_CARRY: {
            uint16_t val = (uint16_t)instr.operand_8_1 + (uint16_t)reg_A 
                           + flag_carry;
            calculate_non_carry_flags(val & 0xff);
            flag_aux_carry = ((instr.operand_8_1 & 0xf) + (reg_A & 0xf) 
                             + flag_carry) > 0xf;
            flag_carry = (val >> 8) > 0;
            reg_A = val;
            break;
        }
        case INSTR_SUBTRACT_IMMEDIATE: {
            uint16_t val = ~(uint16_t)instr.operand_8_1 + 1 + (uint16_t)reg_A;
            calculate_non_carry_flags(val & 0xff);
            flag_carry = (val >> 8) > 0;
            flag_aux_carry = (~(instr.operand_8_1 & 0xf) + 1 
                              + (reg_A & 0xf)) > 0xf;
            reg_A = val;
            break;
        }
        case INSTR_SUBTRACT_IMMEDIATE_WITH_BORROW: {
            uint16_t val = ~((uint16_t)instr.operand_8_1 + flag_carry) + 1 
                           + (uint16_t)reg_A;
            calculate_non_carry_flags(val & 0xff);
            flag_aux_carry = (~((instr.operand_8_1 + flag_carry) & 0xf) + 1 
                              + (reg_A & 0xf)) > 0xf;
            flag_carry = (val >> 8) > 0;
            reg_A = val;
            break;
        }
        case INSTR_AND_IMMEDIATE:
            reg_A = reg_A & instr.operand_8_1;
            calculate_non_carry_flags(reg_A);
            flag_carry = false;
            break;
        case INSTR_XOR_IMMEDIATE:
            reg_A = reg_A ^ instr.operand_8_1;
            calculate_non_carry_flags(reg_A);
            flag_carry = false;
            break;
        case INSTR_OR_IMMEDIATE:
            reg_A = reg_A | instr.operand_8_1;
            calculate_non_carry_flags(reg_A);
            flag_carry = false;
            break;
        case INSTR_COMPARE_IMMEDIATE: {
            uint16_t val = ~(uint16_t)instr.operand_8_1 + 1 + (uint16_t)reg_A;
            calculate_non_carry_flags(val & 0xff);
            flag_carry = (val >> 8) > 0;
            flag_aux_carry = (~(instr.operand_8_1 & 0xf) + 1 + (reg_A & 0xf)) > 0xf;
            break;
        }
    }
}

