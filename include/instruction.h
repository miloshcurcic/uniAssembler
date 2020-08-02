#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "includes.h"
#include "elf.h"

#define INS_SIZE_OFFSET 2
#define INS_OPERATION_CODE_OFFSET 3
#define OP_REGISTER_BYTE_OFFSET 0
#define OP_REGISTER_OFFSET 1
#define OP_ADDRESSING_OFFSET 5
#define INS_SIZE_BITS 1
#define INS_OPERATION_CODE_BITS 5
#define OP_REGISTER_BYTE_BITS 1
#define OP_REGISTER_BITS 4
#define OP_ADDRESSING_BITS 3

enum Operation : Elf16_Byte {
    OP_HALT,
    OP_IRET,
    OP_RET,
    OP_INT,
    OP_CALL,
    OP_JMP,
    OP_JEQ,
    OP_JNE,
    OP_JGT,
    OP_PUSH,
    OP_POP,
    OP_XCHG,
    OP_MOV,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_CMP,
    OP_NOT,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_TEST,
    OP_SHL,
    OP_SHR
};

enum OpBytes : Elf16_Byte {
    OB_ONE,
    OB_TWO
};

enum AddressingMode : Elf16_Byte {
    AM_IMMED,
    AM_REGDIR,
    AM_REGIND,
    AM_BASEREG,
    AM_MEMDIR
};

enum Register : Elf16_Byte {
    R_0,
    R_1,
    R_2,
    R_3,
    R_4,
    R_5,
    R_6,
    R_7,
    R_PSW = 0xF
};

enum RegisterByte : Elf16_Byte {
    RB_LOW,
    RB_HIGH
};

struct InstructionOperand {
    Elf16_Byte op_descr;
    string value;
    bool literal_value;
    Elf16_Rel_Type rel_type;
};

struct Instruction {
    Elf16_Byte ins_descr;
    InstructionOperand* op1;
    InstructionOperand* op2;
};

class InstructionHandler {
public:
    static const string op_names[];
    static InstructionOperand* prep_ins_op(AddressingMode mode, string value = "", bool literal_value = false, Register reg = Register::R_0, RegisterByte reg_byte = RegisterByte::RB_LOW);
    static Instruction* prep_ins(string operation, InstructionOperand* op1 = nullptr, InstructionOperand* op2 = nullptr);
    static void handle_operand(Elf16_Byte op_descr, InstructionOperand* op);
    static void handle_instruction(Instruction* ins);
};

#endif