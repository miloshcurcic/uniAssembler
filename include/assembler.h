#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "includes.h"
#include "elf.h"

#define UND_NX 0
#define SYM_TAB_NX 1
#define STR_TAB_NX 2
#define SH_STR_TAB_NX 3

class Assembler {
public: 
    static Assembler& getInstance()
    {
        static Assembler instance;
        return instance;
    }
private:
    Assembler();
    vector<Elf16_SH_Entry> section_headers;
    vector<vector<Elf16_Byte>> sections;
    unordered_map<string, Elf16_UWord> section_nxs;
    unordered_map<string, Elf16_UWord> symbol_nxs;
    Elf16_UWord current_section;

    Elf16_ST_Entry* find_symbol(string name);
    void add_symbol(string name, Elf16_UWord value, Elf16_Sym_Link link, Elf16_UWord section);
    void create_section(string name, Elf16_Section_Type type);
    void write_to_section(string data, Elf16_UWord section);
    void write_to_section(const Elf16_Byte *data, Elf16_UWord num, Elf16_UWord section);
};

#endif