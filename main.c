
#include <stdio.h>

#include "cpu.h"

int main(int argc, char *argv[]) {
    Instr instr;

    init_cpu();

    while (1) {
        instr = fetch_instr();
        printf("Opcode: 0x%x\n", instr.opcode);
    }
}
