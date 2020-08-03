#include "instruction.h"
#include "logger.h"
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

Instruction* InstructionHandler::prep_ins(string operation, InstructionOperand* op1, InstructionOperand* op2) {
    Instruction *ins = new Instruction();
    ins->op1 = op1;
    ins->op2 = op2;
    
    if (operation.back() == 'b' && (operation.compare("sub") != 0)) {
        operation.pop_back();
        ins->ins_descr = OpBytes::OB_ONE << INS_SIZE_OFFSET;

        if (ins->op1 != nullptr) {
            ins->op1->rel_type = Elf16_Rel_Type::ERT_8;
        }

        if (ins->op2 != nullptr) {
            ins->op2->rel_type = Elf16_Rel_Type::ERT_8;
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

void InstructionHandler::handle_operand(Elf16_Byte op_descr, InstructionOperand* op) {
    if (op) {
        Assembler::get_instance().write_to_cur_section(&op->op_descr, 1);

        string log_message = "\tOperand";

        auto addressing_type = op->op_descr >> OP_ADDRESSING_OFFSET;
        auto two_byte_ins = (op_descr >> INS_SIZE_OFFSET) & 1;

        // Zero, one or two depending on how many additional bytes this operand needs.
        auto additional_bytes =
            addressing_type == AddressingMode::AM_REGDIR || addressing_type == AddressingMode::AM_REGIND ? 0 :
            addressing_type == AddressingMode::AM_BASEREG ? 2 :
            two_byte_ins ? 2 : 1;

        log_message += ", addressing_type [" + to_string(addressing_type) + "], size [" + to_string(additional_bytes + 1) + "]";

        if (additional_bytes != 0) {
            Elf16_Word value;
            bool value_defined = false;
            Elf16_ST_Entry* symbol = nullptr;

            if (op->literal_value) {
                value = Utility::cast_literal(op->value);
                value_defined = true;

                log_message += ", literal [" + to_string(value) + "]";
            } else {
                symbol = Assembler::get_instance().find_symbol(op->value);

                if (symbol != nullptr) {
                    value = symbol->value;
                    value_defined = symbol->shndx != UND_NDX;
                }

                log_message += ", symbol [" + op->value;
                if (value_defined) {
                    log_message += ", " + to_string(value);
                }
                log_message += "]";
            }

            if (value_defined) {
                Assembler::get_instance().write_to_cur_section((Elf16_Byte*)&value, additional_bytes);
            
                log_message += ", defined";
            } else {
                log_message += ", not defined";

                if (symbol == nullptr) {
                    Assembler::get_instance().add_symbol(op->value, 0, Elf16_Sym_Link::ESL_LOCAL, UND_NDX);

                    log_message += ", added to ST";
                }
        
                if (op->rel_type == Elf16_Rel_Type::ERT_16) {
                    const Elf16_UWord zero = 0;
                    Assembler::get_instance().write_fw_ref_cur(op->value, (Elf16_Byte*)&zero, op->rel_type);

                    log_message += ", added absolute FW ref";
                } else {
                    /* ToDo: What to write ? */

                    log_message += ", added relative FW ref";
                }
            }
        }
        
        Logger::write_log(log_message + ".");
    }
}

void InstructionHandler::handle_instruction(Instruction *ins) {
    Assembler::get_instance().write_to_cur_section(&ins->ins_descr, 1);

    Logger::write_log("Writing instruction " + op_names[ins->ins_descr >> INS_OPERATION_CODE_OFFSET] + ".");

    handle_operand(ins->ins_descr, ins->op1);
    handle_operand(ins->ins_descr, ins->op2);

    delete ins->op1;
    delete ins->op2;
    delete ins;
}