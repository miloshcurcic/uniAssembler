#include "label.h"
#include "logger.h"

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
    Logger::write_log("Labeled content handling not implemented.");
}