#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "includes.h"
#include "elf.h"

class Utility {
public:
    static Word cast_literal(string literal);
    static bool is_literal(string value);
    static void print_section_headers(vector<Elf16_SH_Entry>& section, vector<Byte>& sh_str_tab);
    static void print_section(string name, vector<Byte>& section);
    static void print_str_tab(string name, vector<Byte>& section);
    static void print_sym_tab(string name, vector<Elf16_ST_Entry>& section, vector<Byte>& str_tab);
    static void print_rel_section(string name, vector<Elf16_RT_Entry>& section);
    static void print_file_header(string name, Elf16_Header& header);
};

#endif