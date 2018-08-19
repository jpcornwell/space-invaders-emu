
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
            break;
        case 0x02:
            break;
        case 0x03:
            instr = populate_instr(INSTR_INCREMENT_REG_PAIR, "INX", 5, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0x04:
            break;
        case 0x05:
            break;
        case 0x06:
            break;
        case 0x07:
            break;
        case 0x08:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x09:
            instr = populate_instr(INSTR_DOUBLE_ADD, "DAD", 10, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0x0a:
            break;
        case 0x0b:
            instr = populate_instr(INSTR_DECREMENT_REG_PAIR, "DCX", 5, 1,
                                   INSTR_OP_REG_PAIR_B);
            break;
        case 0x0c:
            break;
        case 0x0d:
            break;
        case 0x0e:
            break;
        case 0x0f:
            break;
        case 0x10:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x11:
            break;
        case 0x12:
            break;
        case 0x13:
            instr = populate_instr(INSTR_INCREMENT_REG_PAIR, "INX", 5, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0x14:
            break;
        case 0x15:
            break;
        case 0x16:
            break;
        case 0x17:
            break;
        case 0x18:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x19:
            instr = populate_instr(INSTR_DOUBLE_ADD, "DAD", 10, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0x1a:
            break;
        case 0x1b:
            instr = populate_instr(INSTR_DECREMENT_REG_PAIR, "DCX", 5, 1,
                                   INSTR_OP_REG_PAIR_D);
            break;
        case 0x1c:
            break;
        case 0x1d:
            break;
        case 0x1e:
            break;
        case 0x1f:
            break;
        case 0x20:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x21:
            break;
        case 0x22:
            break;
        case 0x23:
            instr = populate_instr(INSTR_INCREMENT_REG_PAIR, "INX", 5, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0x24:
            break;
        case 0x25:
            break;
        case 0x26:
            break;
        case 0x27:
            break;
        case 0x28:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x29:
            instr = populate_instr(INSTR_DOUBLE_ADD, "DAD", 10, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0x2a:
            break;
        case 0x2b:
            instr = populate_instr(INSTR_DECREMENT_REG_PAIR, "DCX", 5, 1,
                                   INSTR_OP_REG_PAIR_H);
            break;
        case 0x2c:
            break;
        case 0x2d:
            break;
        case 0x2e:
            break;
        case 0x2f:
            break;
        case 0x30:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x31:
            break;
        case 0x32:
            break;
        case 0x33:
            instr = populate_instr(INSTR_INCREMENT_REG_PAIR, "INX", 5, 1,
                                   INSTR_OP_REG_PAIR_SP);
            break;
        case 0x34:
            break;
        case 0x35:
            break;
        case 0x36:
            break;
        case 0x37:
            break;
        case 0x38:
            instr = populate_instr(INSTR_NOP, "NOP", 4, 1, INSTR_OP_NONE);
            break;
        case 0x39:
            instr = populate_instr(INSTR_DOUBLE_ADD, "DAD", 10, 1,
                                   INSTR_OP_REG_PAIR_SP);
            break;
        case 0x3a:
            break;
        case 0x3b:
            instr = populate_instr(INSTR_DECREMENT_REG_PAIR, "DCX", 5, 1,
                                   INSTR_OP_REG_PAIR_SP);
            break;
        case 0x3c:
            break;
        case 0x3d:
            break;
        case 0x3e:
            break;
        case 0x3f:
            break;
        case 0x40:
            break;
        case 0x41:
            break;
        case 0x42:
            break;
        case 0x43:
            break;
        case 0x44:
            break;
        case 0x45:
            break;
        case 0x46:
            break;
        case 0x47:
            break;
        case 0x48:
            break;
        case 0x49:
            break;
        case 0x4a:
            break;
        case 0x4b:
            break;
        case 0x4c:
            break;
        case 0x4d:
            break;
        case 0x4e:
            break;
        case 0x4f:
            break;
        case 0x50:
            break;
        case 0x51:
            break;
        case 0x52:
            break;
        case 0x53:
            break;
        case 0x54:
            break;
        case 0x55:
            break;
        case 0x56:
            break;
        case 0x57:
            break;
        case 0x58:
            break;
        case 0x59:
            break;
        case 0x5a:
            break;
        case 0x5b:
            break;
        case 0x5c:
            break;
        case 0x5d:
            break;
        case 0x5e:
            break;
        case 0x5f:
            break;
        case 0x60:
            break;
        case 0x61:
            break;
        case 0x62:
            break;
        case 0x63:
            break;
        case 0x64:
            break;
        case 0x65:
            break;
        case 0x66:
            break;
        case 0x67:
            break;
        case 0x68:
            break;
        case 0x69:
            break;
        case 0x6a:
            break;
        case 0x6b:
            break;
        case 0x6c:
            break;
        case 0x6d:
            break;
        case 0x6e:
            break;
        case 0x6f:
            break;
        case 0x70:
            break;
        case 0x71:
            break;
        case 0x72:
            break;
        case 0x73:
            break;
        case 0x74:
            break;
        case 0x75:
            break;
        case 0x76:
            instr = populate_instr(INSTR_HALT, "HLT", 7, 1, INSTR_OP_NONE);
            break;
        case 0x77:
            break;
        case 0x78:
            break;
        case 0x79:
            break;
        case 0x7a:
            break;
        case 0x7b:
            break;
        case 0x7c:
            break;
        case 0x7d:
            break;
        case 0x7e:
            break;
        case 0x7f:
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
            break;
        case 0xc2:
            break;
        case 0xc3:
            break;
        case 0xc4:
            break;
        case 0xc5:
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
            break;
        case 0xe2:
            break;
        case 0xe3:
            break;
        case 0xe4:
            break;
        case 0xe5:
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
