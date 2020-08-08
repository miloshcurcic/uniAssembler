#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "includes.h"
#include "elf.h"

#define UND_NDX 0
#define SH_STR_TAB_NDX 1
#define STR_TAB_NDX 2
#define SYM_TAB_NDX 3

struct FW_Ref {
    Elf16_Addr addr;
    Elf16_Rel_Type type;
    Elf16_UWord shndx;
};

struct IS_Operation {
    bool subtract;
    IS_Data* symbol;
};

struct IS_Data {
    Elf16_Word value;
    unordered_map<Elf16_UWord, Elf16_Word> class_ndxs;
};

class Assembler {
public: 
    static Assembler& get_instance()
    {
        static Assembler instance;
        return instance;
    }
    
    void switch_to_section(string name);
    Elf16_ST_Entry* find_symbol(string name);
    void create_symbol(string name, Elf16_UWord value, Elf16_Sym_Link link, Elf16_UWord section);
    void add_or_set_symbol_cur(string name, Elf16_Sym_Link link);
    void add_or_set_extern_symbol(string name);
    void add_or_set_global_symbol(string name);
    void check_global_symbols();
    void handle_symbol(string name, Elf16_Rel_Type rel_type, Elf16_Offs next_ins_offs = 0);
    void generate_rel_tables();
    void add_forward_ref(string name, Elf16_Addr addr, Elf16_Rel_Type type);
    void resolve_forward_refs(string name);
    void resolve_forward_refs(Elf16_Word ndx);
    void add_rel_entry(Elf16_UWord section, Elf16_Addr offs, Elf16_Rel_Type type, Elf16_UWord stndx);
    void write_rel_entry_cur(const Elf16_Byte *data, Elf16_Rel_Type type, Elf16_UWord stndx);
    void write_to_cur_section(const Elf16_Byte *data, Elf16_UWord num);
    void write_fw_ref_cur(string name, const Elf16_Byte *data, Elf16_Rel_Type type);
    void finalize_assembling();
    void create_internal_symbol(string name, list<string> symbols);
private:
    Assembler();
    Elf16_Header header;
    vector<Elf16_SH_Entry> section_headers;
    vector<vector<Elf16_Byte>> sections;
    unordered_map<string, Elf16_UWord> section_ndxs;
    unordered_map<string, Elf16_UWord> symbol_ndxs;
    unordered_map<Elf16_UWord, vector<FW_Ref>> sym_fw_ref_tab;
    vector<string> external_symbols;
    Elf16_UWord current_section;
    unordered_map<string, vector<IS_Operation>> equ_operations;
    unordered_map<string, IS_Data> equ_symbols;

    
    Elf16_ST_Entry* find_symbol(Elf16_Word nx);
    Elf16_UWord create_section(string name, Elf16_Section_Type type);
    void write_to_section(string data, Elf16_UWord section);
    void write_to_section(const Elf16_Byte *data, Elf16_UWord num, Elf16_UWord section);
    void update_headers();
};

#endif