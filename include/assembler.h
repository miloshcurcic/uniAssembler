#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "includes.h"
#include "elf.h"

struct ASH_Entry {
    ASH_Entry(string name, Elf16_Section_Type type) :
        name(name),
        type(type),
        rel(UND_NDX),
        size(0)
    {}
    
    string name;
    Elf16_Section_Type type;
    Elf16_UWord rel;
    Elf16_UWord size;
};

struct AST_Entry {
    Elf16_UWord value;
    Elf16_Sym_Link link;
    Elf16_UWord shndx;
    Elf16_UWord stndx;
};

struct ART_Entry {
    Elf16_Offs offs;
    Elf16_Rel_Type type;
    AST_Entry* sym;
    Elf16_UWord sym_sec;
};

struct AFW_Ref {
    Elf16_Offs offs;
    Elf16_Rel_Type type;
    Elf16_UWord shndx;
};

struct AIS_Data {
    string name;
    Elf16_Word value = 0;
    unordered_map<Elf16_UWord, Elf16_Word> class_ndxs;
    unordered_map<string, vector<bool>> op_list;
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
    
    void create_symbol(string name);
    void create_symbol(AIS_Data* int_symbol);
    void handle_symbol(string name, Elf16_Rel_Type rel_type, Elf16_Offs next_ins_offs = 0);
    void set_extern_symbol(string name);
    void set_global_symbol(string name);
    
    void resolve_forward_refs(string name);

    void finalize_assembling();
    
    AIS_Data* find_internal_symbol(string name);
    void create_internal_symbol(string name, list<string> symbols);
    void update_internal_symbol(AIS_Data* symbol, bool subtract, Elf16_Word value, Elf16_UWord section);
    void resolve_int_sym_ops(string name);

private:
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
    unordered_map<string, set<AIS_Data*>> int_sym_operations;
    unordered_map<string, AIS_Data*> int_symbols;

    Elf16_UWord create_section(string name, Elf16_Section_Type type = Elf16_Section_Type::EST_PROGBITS);
    void write_to_section(string data, Elf16_UWord section);
    void write_to_section(const Elf16_Byte *data, Elf16_UWord num, Elf16_UWord section);
    

    void create_symbol(string name, Elf16_UWord value, Elf16_UWord shndx);
    AST_Entry* find_symbol(string name);
   
    void add_rel_entry(AST_Entry* symbol, Elf16_UWord section, Elf16_Offs offs, Elf16_Rel_Type type);
    void add_rel_entry(Elf16_UWord symbol_section, Elf16_UWord section, Elf16_Offs offs, Elf16_Rel_Type type);
    void add_fw_ref(string symbol, Elf16_Rel_Type type);
    
    void finalize_global_symbols();
    void finalize_forward_refs();

    pair<Elf16_Header, vector<Elf16_SH_Entry>> generate_section_headers();
    void generate_sym_tab();
    void generate_rel_tabs();


    void write_binary_output(pair<Elf16_Header, vector<Elf16_SH_Entry>> headers);
    void write_textual_output(pair<Elf16_Header, vector<Elf16_SH_Entry>> headers);

    Assembler() {
        create_section("", Elf16_Section_Type::EST_UND);
    }

    ~Assembler() {
        for (auto& ptr:symbol_table) {
            delete ptr.second;
        }
    }
};

#endif