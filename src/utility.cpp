#include "utility.h"
#include <iostream>

Elf16_Word Utility::cast_literal(string literal) {
    /* Range check */
    if (literal[0] == '0') {
        if (literal.length() == 1) {
            return 0;
        } else if (literal[1] == 'x') {
            return stoi(literal.substr(2), nullptr, 16);
        } else if (literal[1] == 'b') {
            return stoi(literal.substr(2), nullptr, 2);
        } else {
            return stoi(literal.substr(1), nullptr, 8);
        }
    } else if (literal[0] == '\'') {
        return (Elf16_Word)literal[1];
    } else {
        return stoi(literal);
    }
}