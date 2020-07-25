#ifndef _DIRECTIVE_H_
#define _DIRECTIVE_H_

#include "includes.h"

struct Directive {
    string dir_name;
    list<string>* dir_list;

    Directive(string dir_name, list<string>* dir_list) :
        dir_name(dir_name),
        dir_list(dir_list) 
    {
    }
};

class DirectiveHandler {
};

#endif