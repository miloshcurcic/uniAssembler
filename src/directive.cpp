#include "directive.h"
#include "logger.h"
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
    if (two_bytes) {
        Logger::write_log("Writing words:");
    } else {
        Logger::write_log("Writing bytes:");    
    }
    
    auto size = (two_bytes ? 2 : 1);

    for (auto& val : *dir->dir_list) {
        if (Utility::is_literal(val)) {
            auto literal_value = Utility::cast_literal(val);
            Assembler::get_instance().write_to_cur_section((Elf16_Byte*)&literal_value, size);
            Logger::write_log("\tLiteral [" + val + "].");
        } else {
            auto symbol = Assembler::get_instance().find_symbol(val);
            
            if (symbol != nullptr) {
                Assembler::get_instance().write_to_cur_section((Elf16_Byte*)&symbol->value, size);
                
                Logger::write_log("\tSymbol [" + val + ", " + to_string(symbol->value) + "].");
            } else {
                Assembler::get_instance().add_symbol(val, 0, Elf16_Sym_Link::ESL_LOCAL, UND_NDX);

                const Elf16_UWord zero = 0;

                if (two_bytes) {
                    Assembler::get_instance().write_fw_ref_cur(val, (Elf16_Byte*)&zero, Elf16_Rel_Type::ERT_16); 
                } else {
                    Assembler::get_instance().write_fw_ref_cur(val, (Elf16_Byte*)&zero, Elf16_Rel_Type::ERT_8);
                }
                
                Logger::write_log("\tUndefined symbol [" + val + "], added to ST, added absolute FW ref.");
            }
        }
    }
}

void DirectiveHandler::handle_directive(Directive* dir) {
    switch (dir->dir_name) {
        case DirectiveName::DN_GLOBAL: {
            Logger::write_log("Directive handling not implemented.");
            break;
        }
        case DirectiveName::DN_EXTERN:{
            Logger::write_log("Directive handling not implemented.");
            break;
        }
        case DirectiveName::DN_SECTION: {
            auto section = dir->dir_list->back();
            Assembler::get_instance().switch_to_section(section);

            Logger::write_log("Switching to section [" + section + "].");
            break;
        }
        case DirectiveName::DN_EQU: {
            Logger::write_log("Directive handling not implemented.");
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

            Logger::write_log("Skipping " + to_string(num) + " bytes.");
            break;
        }
        case DirectiveName::DN_END: {
            Logger::write_log("Directive handling not implemented.");
            break;
        }
    }

    delete dir->dir_list;
    delete dir;
}