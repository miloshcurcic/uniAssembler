#include "directive.h"
#include "logger.h"
#include "assembler.h"

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
            Logger::write_log("Switching to section [" + section + "].");
            Assembler::getInstance().switch_to_section(section);
            break;
        }
        case DirectiveName::DN_EQU: {
            Logger::write_log("Directive handling not implemented.");
            break;
        }
        case DirectiveName::DN_BYTE: {
            Logger::write_log("Directive handling not implemented.");
            break;
        }
        case DirectiveName::DN_WORD: {
            Logger::write_log("Directive handling not implemented.");
            break;
        }
        case DirectiveName::DN_SKIP: {
            Logger::write_log("Directive handling not implemented.");
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