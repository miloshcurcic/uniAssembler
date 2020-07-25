#include "instruction.h"

const string InstructionHandler::op_names[] = { "halt", "iret", "ret", "int", "call", "jmp", "jeq", "jne", "jgt", "push", "pop", "xchg", "mov", "add", "sub", "mul", "div", "cmp", "not", "and", "or", "xor", "test", "shl", "shr" };

InstructionOperand* InstructionHandler::prep_ins_op(AddressingMode mode, string value, bool literal_value, Register reg, RegisterByte reg_byte) {
    InstructionOperand *op = new InstructionOperand();
    op->op_descr = mode << OP_ADDRESSING_OFFSET | reg << OP_REGISTER_OFFSET | reg_byte << OP_REGISTER_BYTE_OFFSET;
    op->value = value;
    op->literal_value = literal_value;

    if (reg == Register::R7) {
        op->rel_type = Elf16_Rel_Type::R_PC16;
    } else {
        op->rel_type = Elf16_Rel_Type::R_16;
    }

    return op;
}

Instruction* InstructionHandler::prep_ins(string operation, InstructionOperand* op1, InstructionOperand* op2) {
    Instruction *ins = new Instruction();
    ins->op1 = op1;
    ins->op2 = op2;
    
    if (operation.back() == 'b') {
        operation.pop_back();
        ins->ins_descr = OpBytes::ONE << INS_SIZE_OFFSET;
    } else {
        if (operation.back() == 'w') {
            operation.pop_back();
        }
        ins->ins_descr = OpBytes::TWO << INS_SIZE_OFFSET;
    }
    
    for(int i=0; i < 25; i++) {
        if (op_names[i].compare(operation) == 0) {
            ins->ins_descr |= i << INS_OPERATION_CODE_OFFSET;
            break;
        }
    }

    return ins;
}