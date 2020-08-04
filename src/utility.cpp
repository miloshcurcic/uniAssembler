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

bool Utility::is_literal(string value) {
    if ((value[0] >= '0' && value[0] <= '9') || value[0] == '\'') {
        return true;
    } else {
        return false;
    }
}

void Utility::print_section_headers(vector<Elf16_SH_Entry>& headers) {
    printf("\n#sections\n");
    // flags temp removed
    printf("#%7s %10s %7s %5s %7s\n", "name", "type", "offs", "size", "rel");
    
    const string type_names[] = { "UND", "PROGBITS", "SYMTAB", "STRTAB", "REL" };

    for (auto header : headers) {
        printf(" 0x%05x %10s 0x%05x %5d 0x%05x\n", header.name, type_names[header.type].c_str(), header.offs, header.size, header.rel);
    }
    
    printf("\n");
}

void Utility::print_sym_tab(string name, vector<Elf16_ST_Entry>& section) {
    printf("\n#%s\n", name.c_str());
    printf("#%7s %7s %4s %5s\n", "name", "value", "link", "shndx");
    for (uint i = 0; i < section.size(); i++) {
        printf(" 0x%05x 0x%05x %4c %5s\n", section[i].name, section[i].value, section[i].link ? 'g' : 'l', section[i].shndx ? to_string(section[i].shndx).c_str() : "UND");
    }
    printf("\n");
}

void Utility::print_section(string name, vector<Elf16_Byte>& section) {
    printf("\n#%s\n", name.c_str());
    for (uint i = 0; i < section.size(); i++) {
        if (i != 0 && i % 25 == 0) {
            printf("\n");
        }
        printf("%02x", section[i]);
    }
    printf("\n");
}

void Utility::print_str_tab(string name, vector<Elf16_Byte>& section) {
    printf("\n#%s\n", name.c_str());
    for (uint i = 0; i < section.size(); i++) {
        if (i != 0 && i % 25 == 0) {
            printf("\n");
        }
        printf("%c", section[i]);
    }
    printf("\n");
}

void Utility::print_rel_section(string name, vector<Elf16_RT_Entry>& section) {
    printf("\n#%s\n", name.c_str());
    printf("#%7s %8s %5s\n", "offset", "type", "stndx");
    
    const string type_names[] = { "ERT_8", "ERT_16", "ERT_PC16" };
    
    for (uint i = 0; i < section.size(); i++) {
        printf(" 0x%05x %7s %5d\n", section[i].offs, type_names[section[i].type].c_str(), section[i].stndx);
    }
    printf("\n");
}