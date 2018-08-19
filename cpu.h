
#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef enum InstrType {
    INSTR_MISC,
    INSTR_JUMP,
    INSTR_MOVE,
    INSTR_MATH,
} InstrType;

typedef enum InstrOpType {
    INSTR_OP_NONE,
    INSTR_OP_SINGLE_8,
    INSTR_OP_DOUBLE_8,
    INSTR_OP_16
} InstrOpType;

typedef struct Instr {
    InstrType type;
    uint16_t address;
    uint8_t opcode;
    char mnenomic[4];
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
