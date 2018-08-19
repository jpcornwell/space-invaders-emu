
#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef enum InstrType {
    INSTR_MISC,
    INSTR_JUMP,
    INSTR_MOVE,
    INSTR_MATH,
} InstrType;

typedef struct Instr {
    InstrType type;
    uint16_t address;
    uint8_t opcode;
    char mnenomic[4];
    int cycle_count;
    int byte_count;
} Instr;

void init_cpu(void);
Instr fetch_instr(void);

#endif
