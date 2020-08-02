#ifndef _DIRECTIVE_H_
#define _DIRECTIVE_H_

#include "includes.h"

#define TOTAL_DIR

enum DirectiveName {
    DN_GLOBAL,
    DN_EXTERN,
    DN_SECTION,
    DN_EQU,
    DN_BYTE,
    DN_WORD,
    DN_SKIP,
    DN_END
};

struct Directive {
    DirectiveName dir_name;
    list<string>* dir_list;
};

class DirectiveHandler {
public:
    static const string dir_names[];
    static Directive* prep_directive(string directive, list<string> *dir_list);
    static void handle_directive(Directive* dir);
};

#endif