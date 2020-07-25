#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "includes.h"
#include "elf.h"

#define INS_SIZE_OFFSET 2
#define INS_OPERATION_CODE_OFFSET 3
#define OP_REGISTER_BYTE_OFFSET 0
#define OP_REGISTER_OFFSET 1
#define OP_ADDRESSING_OFFSET 5

enum Operation : Elf16_Byte {
    HALT,
    IRET,
    RET,
    INT,
    CALL,
    JMP,
    JEQ,
    JNE,
    JGT,
    PUSH,
    POP,
    XCHG,
    MOV,
    ADD,
    SUB,
    MUL,
    DIV,
    CMP,
    NOT,
    AND,
    OR,
    XOR,
    TEST,
    SHL,
    SHR
};

enum OpBytes : Elf16_Byte {
    ONE,
    TWO
};

enum AddressingMode : Elf16_Byte {
    IMMED,
    REGDIR,
    REGIND,
    BASEREG,
    MEMDIR
};

enum Register : Elf16_Byte {
    R0,
    R1,
    R2,
    R4,
    R5,
    R6,
    R7,
    PSW = 0xF
};

enum RegisterByte : Elf16_Byte {
    LOW,
    HIGH
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
    static InstructionOperand* prep_ins_op(AddressingMode mode, string value = "", bool literal_value = false, Register reg = Register::R0, RegisterByte reg_byte = RegisterByte::LOW);
    static Instruction* prep_ins(string operation, InstructionOperand* op1 = nullptr, InstructionOperand* op2 = nullptr);
};
#endif