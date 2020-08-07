#include "label.h"
#include "instruction.h"
#include "directive.h"
#include "assembler.h"

LabeledContent* LabelHandler::prep_labeled_content(string label, Instruction* ins) {
    auto result = new LabeledContent();
    result->label = label;
    result->ins = ins;
    return result;
}

LabeledContent* LabelHandler::prep_labeled_content(string label, Directive* dir) {
    auto result = new LabeledContent();
    result->label = label;
    result->dir = dir;
    return result;
}

void LabelHandler::handle_labeled_content(LabeledContent* content) {
    Assembler::get_instance().add_or_set_symbol_cur(content->label, Elf16_Sym_Link::ESL_LOCAL);
    Assembler::get_instance().resolve_forward_refs(content->label);

    if (content->dir != nullptr) {
        DirectiveHandler::handle_directive(content->dir);;
    } else {
        InstructionHandler::handle_instruction(content->ins);
    }
}