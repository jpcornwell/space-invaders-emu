
#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef enum InstrType {
    INSTR_NOP,
    INSTR_HALT,
    INSTR_DISABLE_INTERRUPT,
    INSTR_ENABLE_INTERRUPT,
    INSTR_OUTPUT,
    INSTR_INPUT,
    INSTR_DOUBLE_ADD,
    INSTR_INCREMENT_REG_PAIR,
    INSTR_DECREMENT_REG_PAIR,
    INSTR_POP,
    INSTR_PUSH,
    INSTR_EXCHANGE_STACK,
    INSTR_LOAD_SP_FROM_HL,
    INSTR_EXCHANGE_REGS,
    INSTR_LOAD_HL_DIRECT,
    INSTR_STORE_HL_DIRECT,
    INSTR_LOAD_REG_PAIR_IMMEDIATE,
    INSTR_STORE_ACCUMULATOR,
    INSTR_LOAD_ACCUMULATOR,
    INSTR_STORE_ACCUMULATOR_DIRECT,
    INSTR_LOAD_ACCUMULATOR_DIRECT,
    INSTR_MOVE_IMMEDIATE,
    INSTR_MOVE,
    INSTR_INCREMENT_REG,
    INSTR_DECREMENT_REG,
    INSTR_ROTATE_ACCUMULATOR_LEFT,
    INSTR_ROTATE_ACCUMULATOR_RIGHT,
    INSTR_ROTATE_ACCUMULATOR_LEFT_CARRY,
    INSTR_ROTATE_ACCUMULATOR_RIGHT_CARRY,
    INSTR_DECIMAL_ADJUST_ACCUMULATOR,
    INSTR_COMPLEMENT_ACCUMULATOR,
    INSTR_SET_CARRY,
    INSTR_COMPLEMENT_CARRY,
    INSTR_RESTART_0,
    INSTR_RESTART_1,
    INSTR_RESTART_2,
    INSTR_RESTART_3,
    INSTR_RESTART_4,
    INSTR_RESTART_5,
    INSTR_RESTART_6,
    INSTR_RESTART_7,
    INSTR_CALL,
    INSTR_CALL_IF_CARRY,
    INSTR_CALL_IF_NO_CARRY,
    INSTR_CALL_IF_ZERO,
    INSTR_CALL_IF_NOT_ZERO,
    INSTR_CALL_IF_MINUS,
    INSTR_CALL_IF_PLUS,
    INSTR_CALL_IF_PARITY_EVEN,
    INSTR_CALL_IF_PARITY_ODD,
    INSTR_LOAD_PROGRAM_COUNTER,
    INSTR_JUMP,
    INSTR_JUMP_IF_CARRY,
    INSTR_JUMP_IF_NO_CARRY,
    INSTR_JUMP_IF_ZERO,
    INSTR_JUMP_IF_NOT_ZERO,
    INSTR_JUMP_IF_MINUS,
    INSTR_JUMP_IF_PLUS,
    INSTR_JUMP_IF_PARITY_EVEN,
    INSTR_JUMP_IF_PARITY_ODD,
    INSTR_RETURN,
    INSTR_RETURN_IF_CARRY,
    INSTR_RETURN_IF_NO_CARRY,
    INSTR_RETURN_IF_ZERO,
    INSTR_RETURN_IF_NOT_ZERO,
    INSTR_RETURN_IF_MINUS,
    INSTR_RETURN_IF_PLUS,
    INSTR_RETURN_IF_PARITY_EVEN,
    INSTR_RETURN_IF_PARITY_ODD,
    INSTR_ADD_REG,
    INSTR_ADD_REG_WITH_CARRY,
    INSTR_SUBTRACT_REG,
    INSTR_SUBTRACT_REG_WITH_BORROW,
    INSTR_AND_REG,
    INSTR_XOR_REG,
    INSTR_OR_REG,
    INSTR_COMPARE_REG,
    INSTR_ADD_IMMEDIATE,
    INSTR_ADD_IMMEDIATE_WITH_CARRY,
    INSTR_SUBTRACT_IMMEDIATE,
    INSTR_SUBTRACT_IMMEDIATE_WITH_BORROW,
    INSTR_AND_IMMEDIATE,
    INSTR_XOR_IMMEDIATE,
    INSTR_OR_IMMEDIATE,
    INSTR_COMPARE_IMMEDIATE
} InstrType;

typedef enum InstrOpType {
    INSTR_OP_NONE,
    INSTR_OP_SINGLE_8,
    INSTR_OP_DOUBLE_8,
    INSTR_OP_16,
    INSTR_OP_REG_PAIR_B,
    INSTR_OP_REG_PAIR_D,
    INSTR_OP_REG_PAIR_H,
    INSTR_OP_REG_PAIR_SP,
    INSTR_OP_REG_PAIR_PSW,
    INSTR_OP_REG_PAIR_B_AND_OP_16,
    INSTR_OP_REG_PAIR_D_AND_OP_16,
    INSTR_OP_REG_PAIR_H_AND_OP_16,
    INSTR_OP_REG_PAIR_SP_AND_OP_16,
    INSTR_OP_REG_B,
    INSTR_OP_REG_C,
    INSTR_OP_REG_D,
    INSTR_OP_REG_E,
    INSTR_OP_REG_H,
    INSTR_OP_REG_L,
    INSTR_OP_REG_A,
    INSTR_OP_MEM_REF,
    INSTR_OP_REG_B_AND_OP_8,
    INSTR_OP_REG_C_AND_OP_8,
    INSTR_OP_REG_D_AND_OP_8,
    INSTR_OP_REG_E_AND_OP_8,
    INSTR_OP_REG_H_AND_OP_8,
    INSTR_OP_REG_L_AND_OP_8,
    INSTR_OP_REG_A_AND_OP_8,
    INSTR_OP_MEM_REF_AND_OP_8
} InstrOpType;

typedef enum IntSignal {
    INT_SIGNAL_0,
    INT_SIGNAL_1,
    INT_SIGNAL_2,
    INT_SIGNAL_3,
    INT_SIGNAL_4,
    INT_SIGNAL_5,
    INT_SIGNAL_6,
    INT_SIGNAL_7
} IntSignal;

typedef struct Instr {
    InstrType type;
    uint16_t address;
    uint8_t opcode;
    char mnemonic[5];
    int cycle_count;
    int byte_count;
    InstrOpType op_type;
    uint8_t operand_8_1;
    uint8_t operand_8_2;
    uint16_t operand_16;
    InstrOpType move_source;
    InstrOpType move_destination;
} Instr;

typedef struct CpuInnards {
    uint16_t *pc;
    uint16_t *sp;

    uint8_t *reg_A;
    uint8_t *reg_B;
    uint8_t *reg_C;
    uint8_t *reg_D;
    uint8_t *reg_E;
    uint8_t *reg_H;
    uint8_t *reg_L;

    bool *flag_sign;
    bool *flag_zero;
    bool *flag_aux_carry;
    bool *flag_parity;
    bool *flag_carry;

    bool *is_halted;
    bool *is_interruptible;

    uint8_t *input_ports;
    uint8_t *output_ports;
} CpuInnards;

void init_cpu(uint8_t *);
CpuInnards expose_cpu_internals(void);
Instr fetch_instr(void);
int exec_instr(Instr);
void process_interrupt_signal(IntSignal);
uint8_t read_port(uint8_t);
void write_port(uint8_t, uint8_t);
void write_port_bit(uint8_t, uint8_t, uint8_t);

#endif
