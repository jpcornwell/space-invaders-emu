
#include <stdio.h>

#include "cpu.h"

int main(int argc, char *argv[]) {
    Instr instr;

    init_cpu();

    while (1) {
        instr = fetch_instr();
        exec_instr(instr);
    }
}
