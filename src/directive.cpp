#include "directive.h"
#include "assembler.h"
#include "utility.h"

const string DirectiveHandler::dir_names[] = { ".global", ".extern", ".section", ".equ", ".byte", ".word", ".skip", ".end" };

Directive* DirectiveHandler::prep_directive(string directive, list<string> *dir_list) {
    auto result = new Directive();
    result->dir_list = dir_list;
    for (uint i=0; i<dir_names->length(); i++) {
        if (dir_names[i].compare(directive) == 0) {
            result->dir_name = (DirectiveName)i;
            break;
        }
    }
    return result;
}

void DirectiveHandler::handle_alloc_directive(Directive* dir, bool two_bytes) {
    auto size = (two_bytes ? 2 : 1);

    for (auto& val : *dir->dir_list) {
        if (Utility::is_literal(val)) {
            auto literal_value = Utility::cast_literal(val);

            Assembler::get_instance().write_to_cur_section((Elf16_Byte*)&literal_value, size);
        } else {
            Assembler::get_instance().handle_symbol(val, (two_bytes ? Elf16_Rel_Type::ERT_16 : Elf16_Rel_Type::ERT_8));
        }
    }
}

void DirectiveHandler::handle_directive(Directive* dir) {
    switch (dir->dir_name) {
        case DirectiveName::DN_GLOBAL: {
            for (auto& symbol : *dir->dir_list) {
                Assembler::get_instance().set_global_symbol(symbol);
            }

            break;
        }
        case DirectiveName::DN_EXTERN:{
            for (auto& symbol : *dir->dir_list) {
                Assembler::get_instance().set_extern_symbol(symbol);
            }
            
            break;
        }
        case DirectiveName::DN_SECTION: {
            auto section = dir->dir_list->back();
            Assembler::get_instance().switch_section(section);
            break;
        }
        case DirectiveName::DN_EQU: {
            auto name = dir->dir_list->front();
            dir->dir_list->pop_front();
            Assembler::get_instance().create_internal_symbol(name, *dir->dir_list);
            break;
        }
        case DirectiveName::DN_BYTE: {
            handle_alloc_directive(dir, false);

            break;
        }
        case DirectiveName::DN_WORD: {
            handle_alloc_directive(dir, true);

            break;
        }
        case DirectiveName::DN_SKIP: {
            auto literal = dir->dir_list->back();
            auto num = Utility::cast_literal(literal);
            auto temp = (Elf16_Byte*)calloc(num, sizeof(Elf16_Byte));
            Assembler::get_instance().write_to_cur_section(temp, num);
            free(temp);

            break;
        }
        case DirectiveName::DN_END: {
            break;
        }
    }

    delete dir->dir_list;
    delete dir;
}