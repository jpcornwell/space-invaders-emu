
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
    uint8_t opcode;
    char mnenomic[4];
    int cycles;
} Instr;

void init_cpu(void);
Instr fetch_instr(void);

#endif
