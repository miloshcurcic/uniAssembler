#include "instruction.h"
#include "utility.h"
#include "assembler.h"

const string InstructionHandler::op_names[] = { "halt", "iret", "ret", "int", "call", "jmp", "jeq", "jne", "jgt", "push", "pop", "xchg", "mov", "add", "sub", "mul", "div", "cmp", "not", "and", "or", "xor", "test", "shl", "shr" };

InstructionOperand* InstructionHandler::prep_ins_op(AddressingMode mode, string value, bool literal_value, Register reg, RegisterByte reg_byte) {
    InstructionOperand *op = new InstructionOperand();
    op->op_descr = mode << OP_ADDRESSING_OFFSET | reg << OP_REGISTER_OFFSET | reg_byte << OP_REGISTER_BYTE_OFFSET;
    op->value = value;
    op->literal_value = literal_value;

    if (reg == Register::R_7) {
        op->rel_type = Elf16_Rel_Type::ERT_PC16;
    } else {
        op->rel_type = Elf16_Rel_Type::ERT_16;
    }

    return op;
}

Instruction* InstructionHandler::prep_ins(string operation, InstructionOperand* op0, InstructionOperand* op1) {
    Instruction *ins = new Instruction();
    ins->op0 = op0;
    ins->op1 = op1;
    
    if (operation.back() == 'b' && (operation.compare("sub") != 0)) {
        operation.pop_back();
        ins->ins_descr = OpBytes::OB_ONE << INS_SIZE_OFFSET;

        if (ins->op0 != nullptr && (ins->op0->op_descr >> OP_ADDRESSING_OFFSET) == AddressingMode::AM_IMMED) {
            ins->op0->rel_type = Elf16_Rel_Type::ERT_8;
        
            if (ins->op1 != nullptr && (ins->op1->op_descr >> OP_ADDRESSING_OFFSET) == AddressingMode::AM_IMMED) {
                ins->op1->rel_type = Elf16_Rel_Type::ERT_8;
            }
        }
    } else {
        if (operation.back() == 'w') {
            operation.pop_back();
        }

        ins->ins_descr = OpBytes::OB_TWO << INS_SIZE_OFFSET;
    }
    
    for(uint i=0; i < 25; i++) {
        if (op_names[i].compare(operation) == 0) {
            ins->ins_descr |= i << INS_OPERATION_CODE_OFFSET;
            break;
        }
    }

    return ins;
}

void InstructionHandler::handle_instruction(Instruction *ins) {
    Assembler::get_instance().write_to_cur_section(&ins->ins_descr, 1);

    if (ins->op0) {
        Assembler::get_instance().write_to_cur_section(&ins->op0->op_descr, sizeof(Byte));

        auto two_byte_ins = (ins->ins_descr >> INS_SIZE_OFFSET) & 1;
        auto addressing_type_0 = ins->op0->op_descr >> OP_ADDRESSING_OFFSET;

        // Zero, one or two depending on how many additional bytes this operand needs.
        auto additional_bytes_0 =
            addressing_type_0 == AddressingMode::AM_REGDIR || addressing_type_0 == AddressingMode::AM_REGIND ? 0 :
            addressing_type_0 == AddressingMode::AM_BASEREG ? 2 :
            two_byte_ins ? 2 : 1;
        
        // Zero, one or two depending on how many additional bytes this operand needs.
        Word additional_bytes_1 = 0;

        if (ins->op1 != nullptr) {
            auto addressing_type_1 = ins->op1->op_descr >> OP_ADDRESSING_OFFSET;
            
            additional_bytes_1 =
            addressing_type_1 == AddressingMode::AM_REGDIR || addressing_type_1 == AddressingMode::AM_REGIND ? 0 :
            addressing_type_1 == AddressingMode::AM_BASEREG ? 2 :
            two_byte_ins ? 2 : 1;
        }
            
        if (additional_bytes_0 != 0) {
            if (ins->op0->literal_value) {
                auto value = Utility::cast_literal(ins->op0->value);

                Assembler::get_instance().write_to_cur_section((Byte*)&value, additional_bytes_0);
            } else {
                if (ins->op1 != nullptr) {
                    Assembler::get_instance().handle_symbol(ins->op0->value, ins->op0->rel_type, (additional_bytes_0 + additional_bytes_1 + 1) * sizeof(Byte));
                } else {
                    Assembler::get_instance().handle_symbol(ins->op0->value, ins->op0->rel_type, (additional_bytes_0 + additional_bytes_1) * sizeof(Byte));    
                }
            }
        }

        if (ins->op1 != nullptr) {
            Assembler::get_instance().write_to_cur_section(&ins->op1->op_descr, sizeof(Byte));

            if (additional_bytes_1 != 0) {
                if (ins->op1->literal_value) {
                    auto value = Utility::cast_literal(ins->op1->value);

                    Assembler::get_instance().write_to_cur_section((Byte*)&value, additional_bytes_1);
                } else {
                    Assembler::get_instance().handle_symbol(ins->op1->value, ins->op1->rel_type, additional_bytes_1 * sizeof(Byte));
                }
            }
        }
    }

    delete ins->op0;
    delete ins->op1;
    delete ins;
}