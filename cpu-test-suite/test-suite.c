
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cpu.h"

uint8_t memory[65536];

void load_memory(char *path, uint16_t start_addr) {
    FILE *fp = fopen(path, "rb");
    size_t file_size = 0;

    if (fp == NULL) {
        printf("Error opening file %s\n", path);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    memset(memory, 0, sizeof memory);
    fread(memory + start_addr, 1, file_size, fp);

    fclose(fp);
}

void run_test(char *path) {
    Instr instr;

    init_cpu(memory);
    load_memory(path, 0x100);

    CpuInnards cpu = expose_cpu_internals();

    // Test ROMs start at 0x100
    *(cpu.pc) = 0x100;

    // Inject RET at 0x5 to handle "CALL 5"
    memory[5] = 0xc9;

    printf("*******************\n");

    while (true) {
        if (memory[*(cpu.pc)] == 0x76) {
            printf("HLT at %04x\n", *(cpu.pc));
        }

        if (*(cpu.pc) == 5) {
            // Prints characters stored in memory at (DE)
            // until character '$' (0x24 in ASCII) is found
            if (*(cpu.reg_C) == 9) {
                uint16_t addr = ((uint16_t)(*(cpu.reg_D)) << 8) | *(cpu.reg_E);
                do {
                    printf("%c", memory[addr]);
                    addr++;
                } while (memory[addr] != 0x24);
            }

            // Prints a single character stored in register E
            if (*(cpu.reg_C) == 2) {
                printf("%c", *(cpu.reg_E));
            }
        }

        // Execute instruction as one normally would do
        instr = fetch_instr();
        //printf("INSTR: %02x: %s\n", instr.address, instr.mnemonic);
        exec_instr(instr);

        if (*(cpu.pc) == 0) {
            printf("\nJumped to 0x0000\n");
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    run_test("tests/TST8080.COM");
    run_test("tests/CPUTEST.COM");
    run_test("tests/8080PRE.COM");
    run_test("tests/8080EXM.COM");
}
