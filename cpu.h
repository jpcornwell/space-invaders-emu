
#ifndef CPU_H
#define CPU_H

typedef enum InstrType {
    INSTR_MISC,
    INSTR_JUMP,
    INSTR_MOVE,
    INSTR_MATH,
} InstrType;

typedef struct Instr {
    InstrType type;
    char opcode[4];
    int cycles;
} Instr;

Instr fetch_instr(void);

#endif
