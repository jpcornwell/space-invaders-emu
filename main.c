
#include <stdio.h>

#include "cpu.h"

int main(int argc, char *argv[]) {
    Instr instr;

    init_cpu();

    while (1) {
        instr = fetch_instr();
        printf("%x: %s (0x%02x)\n", instr.address, instr.mnenomic, instr.opcode);
    }
}
