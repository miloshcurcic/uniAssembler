#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "includes.h"
#include "elf.h"

#define UND_NDX 0
#define SH_STR_TAB_NDX 1
#define STR_TAB_NDX 2
#define SYM_TAB_NDX 3

struct ASH_Entry {
    ASH_Entry(string name, Elf16_Section_Type type) :
        name(name),
        type(type),
        size(0),
        rel(UND_NDX)
    {}
    
    string name;
    Elf16_Section_Type type;
    Elf16_UWord size;
    Elf16_UWord rel;
};

struct AST_Entry {
    Elf16_UWord value;
    Elf16_Sym_Link link;
    Elf16_UWord shndx;
};

struct ART_Entry {
    Elf16_Offs offs;
    Elf16_Rel_Type type;
    AST_Entry* sym;
};

struct AFW_Ref {
    Elf16_Addr addr;
    Elf16_Rel_Type type;
    Elf16_UWord shndx;
};

struct AIS_Operation {
    AIS_Data* symbol;
    bool subtract = false;
};

struct AIS_Data {
    string name;
    Elf16_Word value = 0;
    unordered_map<Elf16_UWord, Elf16_Word> class_ndxs;
    Elf16_UWord ops = 0;
};

class Assembler {
public: 
    static Assembler& get_instance()
    {
        static Assembler instance;
        return instance;
    }
    
    void switch_section(string name);
    void write_to_cur_section(const Elf16_Byte *data, Elf16_UWord num);
    
    void handle_symbol(string name, Elf16_Rel_Type rel_type, Elf16_Offs next_ins_offs = 0);
   
    void resolve_forward_refs(string name);
    
    void generate_rel_tables();
    void finalize_assembling();
    
    AIS_Data* find_internal_symbol(string name);
    void create_internal_symbol(string name, list<string> symbols);
    void update_internal_symbol(AIS_Data* symbol, bool subtract, Elf16_Word value, Elf16_UWord section);
    void resolve_int_sym_ops(string name, Elf16_Word value);
private:
    Assembler();
    
    Elf16_Header header;
    vector<ASH_Entry> section_headers;
    unordered_map<string, Elf16_UWord> section_ndxs;
    unordered_map<string, AST_Entry*> symbol_table;
    unordered_map<Elf16_UWord, vector<ART_Entry>> rel_tables;
    vector<vector<Elf16_Byte>> binary_sections;
    unordered_map<string, vector<AFW_Ref>> forward_ref_tab;
    vector<string> external_symbols;
    vector<string> global_symbols;
    Elf16_UWord current_section;
    unordered_map<string, vector<AIS_Operation>> int_sym_operations;
    unordered_map<string, AIS_Data*> int_symbols;

    
    Elf16_UWord create_section(string name, Elf16_Section_Type type);
    void write_to_section(string data, Elf16_UWord section);
    void write_to_section(const Elf16_Byte *data, Elf16_UWord num, Elf16_UWord section);
    

    void create_symbol(string name, Elf16_UWord value, Elf16_UWord shndx);
    void create_symbol(string name);
    AST_Entry* find_symbol(string name);
    void set_extern_symbol(string name);
    void set_global_symbol(string name);
    void check_global_symbols();
   
    void add_rel_entry(string symbol, Elf16_UWord section, Elf16_Addr offs, Elf16_Rel_Type type);
    void write_and_add_rel_entry(string symbol, Elf16_Rel_Type type, const Elf16_Byte *data);
    void write_and_add_fw_ref(string symbol, Elf16_Rel_Type type, const Elf16_Byte *data);
    
    void update_headers();
};

#endif