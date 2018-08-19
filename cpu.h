
#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef enum InstrType {
    INSTR_NOP,
    INSTR_HALT,
    INSTR_DISABLE_INTERRUPT,
    INSTR_ENABLE_INTERRUPT,
    INSTR_OUTPUT,
    INSTR_INPUT,
    INSTR_DOUBLE_ADD,
    INSTR_INCREMENT_REG_PAIR,
    INSTR_DECREMENT_REG_PAIR
} InstrType;

typedef enum InstrOpType {
    INSTR_OP_NONE,
    INSTR_OP_SINGLE_8,
    INSTR_OP_DOUBLE_8,
    INSTR_OP_16,
    INSTR_OP_REG_PAIR_B,
    INSTR_OP_REG_PAIR_D,
    INSTR_OP_REG_PAIR_H,
    INSTR_OP_REG_PAIR_SP
} InstrOpType;

typedef struct Instr {
    InstrType type;
    uint16_t address;
    uint8_t opcode;
    char mnenomic[5];
    int cycle_count;
    int byte_count;
    InstrOpType op_type;
    uint8_t operand_8_1;
    uint8_t operand_8_2;
    uint16_t operand_16;
} Instr;

void init_cpu(void);
Instr fetch_instr(void);

#endif
