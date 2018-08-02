
#include <stdio.h>

#include "cpu.h"

int main(int argc, char *argv[]) {
    Instr instr;

    instr = fetch_instr();
    printf("Opcode %s\n", instr.opcode);
}
