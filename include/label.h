#ifndef _LABEL_H_
#define _LABEL_H_

#include "includes.h"

class Instruction;
class Directive;

struct LabeledContent {
    string label;
    Instruction *ins = nullptr;
    Directive *dir = nullptr;
};

class LabelHandler {
public:
    static LabeledContent* prep_labeled_content(string label, Directive* dir);
    static LabeledContent* prep_labeled_content(string label, Instruction* ins);
    static void handle_labeled_content(LabeledContent* content);
};

#endif