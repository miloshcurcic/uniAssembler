#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "includes.h"
#include "elf.h"

#define UND_NDX 0
#define SYM_TAB_NDX 1
#define STR_TAB_NDX 2
#define SH_STR_TAB_NDX 3

class Assembler {
public: 
    static Assembler& getInstance()
    {
        static Assembler instance;
        return instance;
    }
    
    void switch_to_section(string name);
    Elf16_ST_Entry* find_symbol(string name);
    void add_symbol(string name, Elf16_UWord value, Elf16_Sym_Link link, Elf16_UWord section);
    void add_forward_ref(string name, Elf16_Addr addr);
    void resolve_forward_refs(string name, Elf16_Word value);
    void resolve_forward_refs(Elf16_Word ndx, Elf16_Word value);
    void add_rel_entry(Elf16_UWord section, Elf16_Addr offs, Elf16_Rel_Type type, Elf16_UWord stndx);
    void write_to_cur_section(const Elf16_Byte *data, Elf16_UWord num);
    void write_fw_ref_cur(string name, const Elf16_Byte *data, Elf16_UWord num);
private:
    Assembler();
    vector<Elf16_SH_Entry> section_headers;
    vector<vector<Elf16_Byte>> sections;
    unordered_map<string, Elf16_UWord> section_ndxs;
    unordered_map<string, Elf16_UWord> symbol_ndxs;
    unordered_map<Elf16_UWord, list<Elf16_Addr>> sym_fw_ref_tab;
    Elf16_UWord current_section;
    
    Elf16_ST_Entry* find_symbol(Elf16_Word nx);
    Elf16_UWord create_section(string name, Elf16_Section_Type type);
    void write_to_section(string data, Elf16_UWord section);
    void write_to_section(const Elf16_Byte *data, Elf16_UWord num, Elf16_UWord section);
};

#endif