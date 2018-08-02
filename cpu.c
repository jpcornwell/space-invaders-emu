
#include <stdint.h>
#include <string.h>

#include "cpu.h"

uint16_t pc;

Instr fetch_instr() {
    Instr instr;

    instr.type = INSTR_MISC;
    strcpy(instr.opcode, "FF");
    instr.cycles = 5;

    return instr;
}
