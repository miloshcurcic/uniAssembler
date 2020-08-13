#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "includes.h"
#include "elf.h"

struct ASH_Entry {
    ASH_Entry(string name, Elf16_Section_Type type) :
        name(name),
        type(type)
    {}
    
    string name;
    Elf16_Section_Type type;
    Word size = 0;
    Word link = UND_NDX;
};

struct AST_Entry {
    Word value;
    Elf16_Sym_Bind bind;
    Word shndx;
    Word stndx;
};

struct ART_Entry {
    Offs offs;
    Elf16_Rel_Type type;
    AST_Entry* sym;
    Word sym_sec;
};

struct AFW_Ref {
    Offs offs;
    Elf16_Rel_Type type;
    Word shndx;
};

struct AIS_Data {
    string name;
    Word value = 0;
    unordered_map<Word, Word> class_ndxs;
    unordered_map<string, Word> op_list;
};

class Assembler {
public: 
    static Assembler& get_instance()
    {
        static Assembler instance;
        return instance;
    }
    
    void switch_section(string name);
    void write_to_cur_section(const Byte *data, Word num);
    
    void create_symbol(string name);
    void create_symbol(AIS_Data* int_symbol);
    void handle_symbol(string name, Elf16_Rel_Type rel_type, Offs next_ins_offs = 0);
    void set_extern_symbol(string name);
    void set_global_symbol(string name);
    
    void resolve_forward_refs(string name);

    void finalize_assembling(string out_file);
    
    AIS_Data* find_internal_symbol(string name);
    void create_internal_symbol(string name, list<string> symbols);
    void update_internal_symbol(AIS_Data* symbol, Word coeff, Word value, Word section);
    void resolve_int_sym_ops(string name);

private:
    Elf16_Header header;
    vector<ASH_Entry> section_headers;
    unordered_map<string, Word> section_ndxs;
    unordered_map<string, AST_Entry*> symbol_table;
    unordered_map<Word, vector<ART_Entry>> rel_tables;
    vector<vector<Byte>> binary_sections;
    unordered_map<string, vector<AFW_Ref>> forward_ref_tab;
    vector<string> external_symbols;
    vector<string> global_symbols;
    Word current_section;
    unordered_map<string, set<AIS_Data*>> int_sym_operations;
    unordered_map<string, AIS_Data*> int_symbols;

    Word create_section(string name, Elf16_Section_Type type = Elf16_Section_Type::EST_PROGBITS);
    void write_to_section(string data, Word section);
    void write_to_section(const Byte *data, Word num, Word section);
    

    void create_symbol(string name, Word value, Word shndx);
    AST_Entry* find_symbol(string name);
   
    void add_rel_entry(AST_Entry* symbol, Word section, Offs offs, Elf16_Rel_Type type);
    void add_rel_entry(Word symbol_section, Word section, Offs offs, Elf16_Rel_Type type);
    void add_fw_ref(string symbol, Elf16_Rel_Type type);
    
    void finalize_internal_symbols();
    void finalize_global_symbols();
    void finalize_forward_refs();

    pair<Elf16_Header, vector<Elf16_SH_Entry>> generate_section_headers(pair<Word, Word>  tab_ndxs);
    pair<Word, Word>  generate_sym_tab();
    void generate_rel_tabs();


    void write_binary_output(string out_file_name, pair<Elf16_Header, vector<Elf16_SH_Entry>> headers);
    void write_textual_output(string out_file_name, pair<Elf16_Header, vector<Elf16_SH_Entry>> headers);

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