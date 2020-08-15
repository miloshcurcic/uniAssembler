#include "assembler.h"
#include "utility.h"
#include "asm_exception.h"

Word Assembler::create_section(string name, Elf16_Section_Type type) {
    auto id = section_headers.size();
    section_headers.emplace_back(name, type);
    binary_sections.emplace_back();
    section_ndxs[name] = id; 
    return id;
}

void Assembler::switch_section(string name) {
    auto section = section_ndxs.find(name);
    Word result;
    
    if (section == section_ndxs.end()) {
        result = create_section(name);
    } else {
        result = section->second;
    }

    current_section = result;
}

void Assembler::write_to_section(string data, Word section) {
    write_to_section((Byte*)data.c_str(), data.length() + 1, section);
}

void Assembler::write_to_section(const Byte *data, Word num, Word section) {
    binary_sections[section].insert(binary_sections[section].end(), data, data + num);
    section_headers[section].size += num;
}

void Assembler::write_to_cur_section(const Byte *data, Word num) {
    write_to_section(data, num, current_section);
}


void Assembler::create_symbol(string name, Word value, Word shndx) {
    Elf16_Sym_Bind bind = Elf16_Sym_Bind::ESB_LOCAL;

    if (find(external_symbols.begin(), external_symbols.end(), name) != external_symbols.end()) {
        bind = Elf16_Sym_Bind::ESB_GLOBAL;
    }

    if (find(global_symbols.begin(), global_symbols.end(), name) != global_symbols.end()) {
        bind = Elf16_Sym_Bind::ESB_GLOBAL;
    }

    symbol_table[name] = unique_ptr<AST_Entry>(new AST_Entry({value, bind, shndx}));   
}

void Assembler::create_symbol(string name) {
    auto symbol = find_symbol(name);

    if (symbol == nullptr) {
        create_symbol(name, section_headers[current_section].size, current_section);
    } else {
        // Check if symbol is declared as extern.
        if (find(external_symbols.begin(), external_symbols.end(), name) != external_symbols.end()) {
            string message = format_string(ERR_DEFINED_SYM_DECLARED_EXTERN, name);
            throw Assembler_Exception(message);
        }

        symbol->value = section_headers[current_section].size;
        symbol->shndx = current_section;
    }

    resolve_forward_refs(name);
    resolve_int_sym_ops(name);
}

AST_Entry* Assembler::find_symbol(string name) {
    auto symbol = symbol_table.find(name);

    if (symbol == symbol_table.end()) {
        return nullptr;
    }

    return symbol->second.get();
}

AIS_Data* Assembler::find_internal_symbol(string name) {
    auto symbol = int_symbols.find(name);

    if (symbol == int_symbols.end()) {
        return nullptr;
    }

    return symbol->second.get();
}

void Assembler::set_extern_symbol(string name) {
    // Symbol already set as extern, ignore multiple declarations.
    if (find(external_symbols.begin(), external_symbols.end(), name) != external_symbols.end()) {
        return;
    }

    // Check if symbol is set as both, extern and global.
    if (find(global_symbols.begin(), global_symbols.end(), name) != global_symbols.end()) {
        string message = format_string(ERR_SYM_DECLARED_EXT_AND_GLB, name);
        throw Assembler_Exception(message);
    }

    auto symbol = find_symbol(name);

    if (symbol == nullptr) {
        external_symbols.push_back(name);
    } else {
        if (symbol->shndx != UND_NDX) {
            string message = format_string(ERR_DEFINED_SYM_DECLARED_EXTERN, name);
            throw Assembler_Exception(message);
        } else {
            symbol->bind = Elf16_Sym_Bind::ESB_GLOBAL;
        }
    }
}

void Assembler::set_global_symbol(string name) {
    // Symbol already set as global, ignore multiple declarations.
    if (find(global_symbols.begin(), global_symbols.end(), name) != global_symbols.end()) {
        return;
    }

    // Check if symbol is set as both, extern and global.
    if (find(global_symbols.begin(), global_symbols.end(), name) != global_symbols.end()) {
        string message = format_string(ERR_SYM_DECLARED_EXT_AND_GLB, name);
        throw Assembler_Exception(message);
    }

    auto symbol = find_symbol(name);
    
    if (symbol == nullptr) {
        global_symbols.push_back(name);
    } else {
        symbol->bind = Elf16_Sym_Bind::ESB_GLOBAL;
    }
}

void Assembler::handle_symbol(string name, Elf16_Rel_Type rel_type, Offs next_ins_offs) {
    auto symbol = find_symbol(name);
    auto int_symbol = find_internal_symbol(name);

    // If a symbol doesn't exist create it and add corresponding forward reference and addend.
    if (symbol == nullptr || symbol->shndx == UND_NDX) {
        if (symbol == nullptr && int_symbol == nullptr) {
            create_symbol(name, 0, UND_NDX);
        }

        auto value = (rel_type == Elf16_Rel_Type::ERT_PC16 ? -next_ins_offs : 0);

        add_fw_ref(name, rel_type);
        write_to_cur_section((Byte*)&value, (rel_type == Elf16_Rel_Type::ERT_8 ? sizeof(Byte) : sizeof(Word)));
    } else {
        // If a symbol exists, depending on rel_type add corresponding rel entry and addend.
        if (rel_type == Elf16_Rel_Type::ERT_PC16) {
            if (symbol->shndx == current_section) {
                auto value = symbol->value - next_ins_offs - section_headers[current_section].size;

                write_to_cur_section((Byte*)&value, sizeof(Offs));
            } else {
                auto value = symbol->value - next_ins_offs;
            
                if (symbol->shndx == ABS_NDX) {
                    add_rel_entry(nullptr, current_section, section_headers[current_section].size, rel_type);
                } else {
                    add_rel_entry(symbol->shndx, current_section, section_headers[current_section].size, rel_type);
                }

                write_to_cur_section((Byte*)&value, sizeof(Offs));
            }
        } else {
            auto value = symbol->value;

            if (symbol->shndx != ABS_NDX) {
                add_rel_entry(symbol->shndx, current_section, section_headers[current_section].size, rel_type);
            }

            if (rel_type == Elf16_Rel_Type::ERT_8) {
                write_to_cur_section((Byte*)&value, sizeof(Byte));   
            } else {
                write_to_cur_section((Byte*)&value, sizeof(Word));
            }
        }
    }
}

void Assembler::add_rel_entry(AST_Entry* symbol, Word section, Offs offs, Elf16_Rel_Type type) {
    rel_tables[section].push_back({offs, type, symbol, 0});
}

void Assembler::add_rel_entry(Word symbol_section, Word section, Offs offs, Elf16_Rel_Type type) {
    rel_tables[section].push_back({offs, type, nullptr, symbol_section});
}

void Assembler::add_fw_ref(string symbol, Elf16_Rel_Type type) {
    add_fw_ref(symbol, current_section, section_headers[current_section].size, type);
}

void Assembler::add_fw_ref(string symbol, Word section, Offs offs, Elf16_Rel_Type type) {
    forward_ref_tab[symbol].push_back({offs, type, section});
}

void Assembler::resolve_int_sym_ops(string name) {
    auto symbol = find_symbol(name);

    for(auto int_sym : int_sym_operations[name]) {
        update_internal_symbol(int_sym, int_sym->op_list[name], symbol->value, symbol->shndx);

        int_sym->op_list.erase(name);

        if (int_sym->op_list.size() == 0) {
            create_symbol(int_symbols[int_sym->name].get());
        }
    }

    int_sym_operations.erase(name);
}

void Assembler::update_internal_symbol(AIS_Data* symbol, Word coeff, Word value, Word section) {
    symbol->value += coeff * value;
    if (section != ABS_NDX) {
        symbol->class_ndxs[section] += coeff;
    }
}

void Assembler::update_internal_symbol_dependency(AIS_Data* int_symbol, string name, Word coeff) {
    // Add operation to op_list.
    // .equ a, e + e - e - symbol e will have coeff 1 + 1 - 1 = 1
    // This value is updated in update_internal_symbol
    int_symbol->op_list[name] += coeff;
    // Add defining internal symbol to set of references to int_symbols where
    // this symbol is used.
    int_sym_operations[name].insert(int_symbol);
    if (int_symbol->op_list[name] == 0) {
        // Symbol coeff reduced to 0, erase it from internal symbol
        // referencing structures.
        int_symbol->op_list.erase(name);
        int_sym_operations[name].erase(int_symbol);
        
        if (int_sym_operations[name].size() == 0) {
            int_sym_operations.erase(name);
        }
    }
}

void Assembler::create_internal_symbol(string name, list<string> operations) {
    if (symbol_table.find(name) != symbol_table.end()) {
        if (symbol_table[name]->shndx != UND_NDX) {        
            string message = format_string(ERR_MULTIPLE_DEFINITIONS, name);
            throw Assembler_Exception(message);
        } else {
            // Delete undefined symbol entry from sym tab.
            symbol_table.erase(name);
        }
    }

    if (int_symbols.find(name) != int_symbols.end()) {
        string message = format_string(ERR_MULTIPLE_DEFINITIONS, name);
        throw Assembler_Exception(message);
    }

    int_symbols[name] = unique_ptr<AIS_Data>(new AIS_Data());
    int_symbols[name]->name = name;

    // Loop through operations
    for (auto op_string : operations) {
        string value = op_string.substr(1);
        Word coeff = (op_string[0] == '-' ? -1 : 1);

        if (Utility::is_literal(value)) {
            // If it's a literal add it to value.
            auto val = Utility::cast_literal(value);

            int_symbols[name]->value += coeff * val;
        } else {
            // If it's a symbol check if it's defined, if it is
            // update coeff and value, otherwise add it to internal
            // referencing structures.
            if (value == name) {
                string message = format_string(ERR_SYMBOL_DEFINITION_LOOP, name);
                throw Assembler_Exception(message);
            }

            auto symbol = find_symbol(value);

            if (symbol && symbol->shndx != UND_NDX) {
               update_internal_symbol(int_symbols[name].get(), coeff, symbol->value, symbol->shndx);
            } else {
                update_internal_symbol_dependency(int_symbols[name].get(), value, coeff);
            }
        }
    }

    // If all operations are resolved, create this symbol (move it to sym tab).
    if (int_symbols[name]->op_list.size() == 0) {
        create_symbol(int_symbols[name].get());
    }
}

Word Assembler::get_class_ndx(AIS_Data* int_symbol) {
    auto section = ABS_NDX;
    bool error = false;

    // Check overall classification index
    for (auto& entry : int_symbol->class_ndxs) {
        if (entry.second == 1 && section == ABS_NDX) {
            section = entry.first;
        } else if (entry.second < 0 || entry.second > 1 || (entry.second == 1 && section != ABS_NDX)) {
            error = true;
            break;
        }
    }

    if (error) {
        string message = format_string(ERR_INVALID_SYM_CLASS_NDX, int_symbol->name);
        throw Assembler_Exception(message);
    }

    return section;
}

void Assembler::create_symbol(AIS_Data* int_symbol) {
    auto section = get_class_ndx(int_symbol);

    create_symbol(int_symbol->name, int_symbol->value, section);

    auto name = int_symbol->name;
    int_symbols.erase(name);
    resolve_forward_refs(name);
    resolve_int_sym_ops(name);
}

void Assembler::resolve_forward_refs(string name) {
    auto symbol = find_symbol(name);

    for (auto entry : forward_ref_tab[name]) {
        if (entry.type == Elf16_Rel_Type::ERT_PC16) {
            if (symbol->shndx == current_section) {
                (Addr&)*(binary_sections[entry.shndx].data() + entry.offs) += symbol->value - entry.offs;
            } else {
                (Addr&)*(binary_sections[entry.shndx].data() + entry.offs) += symbol->value;

                add_rel_entry(symbol->shndx, entry.shndx, entry.offs, entry.type);
            }
        } else {
            if (symbol->shndx != ABS_NDX) {
                add_rel_entry(symbol->shndx, entry.shndx, entry.offs, entry.type);
            }

            if (entry.type == Elf16_Rel_Type::ERT_8) {
                (Byte&)*(binary_sections[entry.shndx].data() + entry.offs) += symbol->value;
            } else {
                (Word&)*(binary_sections[entry.shndx].data() + entry.offs) += symbol->value;
            }
        }
    }

    forward_ref_tab.erase(name);
}


void Assembler::finalize_internal_symbols() {
    bool updated = true;
    while (updated) {
        updated = false;
        for (auto& int_symbol : int_symbols) {
            // Symbol is dependant on one other unkown symbol
            if (int_symbol.second->op_list.size() == 1) {
                // The other unknown symbol is not another unknown internal symbol
                auto unknown_symbol_name = int_symbol.second->op_list.begin()->first;

                if (int_symbols.find(unknown_symbol_name) == int_symbols.end()) {
                    auto section = get_class_ndx(int_symbol.second.get());
                    if (section != ABS_NDX || int_symbol.second->op_list[unknown_symbol_name] != 1) {
                        string message = format_string(ERR_INVALID_SYM_CLASS_NDX, int_symbol.first);
                        throw Assembler_Exception(message);
                    }

                    // Expand symbol to unknown symbol + const in references to this symbol
                    for (auto ptr : int_sym_operations[int_symbol.second->name]) {
                        auto coeff = ptr->op_list[int_symbol.first];

                        // Classification index for the UND symbol can only be 1
                        if (coeff != 1) {
                            string message = format_string(ERR_INVALID_SYM_CLASS_NDX, int_symbol.first);
                            throw Assembler_Exception(message);
                        }

                        ptr->op_list.erase(int_symbol.first);
                        update_internal_symbol(ptr, coeff, int_symbol.second->value, ABS_NDX);
                        update_internal_symbol_dependency(ptr, unknown_symbol_name, coeff);
                    
                        if (ptr->op_list.size() == 0) {
                            create_symbol(ptr);
                        }
                    }
                    int_sym_operations.erase(int_symbol.first);

                    // Add relocation entries for resolved symbol
                    for (auto& ref : forward_ref_tab[int_symbol.first]) {
                        switch (ref.type) {
                            case Elf16_Rel_Type::ERT_PC16: {
                                *(Addr*)(binary_sections[ref.shndx].data() + ref.offs) += int_symbol.second->value;
                                break;
                            }
                            case Elf16_Rel_Type::ERT_16: {
                                *(Word*)(binary_sections[ref.shndx].data() + ref.offs) += int_symbol.second->value;
                                break;
                            }
                            case Elf16_Rel_Type::ERT_8: {
                                *(Byte*)(binary_sections[ref.shndx].data() + ref.offs) += int_symbol.second->value;
                                break;
                            }
                        }
                        
                        auto symbol = find_symbol(unknown_symbol_name);

                        if (symbol == nullptr) {
                            create_symbol(unknown_symbol_name, 0, UND_NDX);
                        }

                        add_fw_ref(unknown_symbol_name, ref.shndx, ref.offs, ref.type);
                    }

                    forward_ref_tab.erase(int_symbol.first);
                    int_symbols.erase(int_symbol.first);
                    updated = true;
                    break;
                }
            }
        }
    }

    if (int_symbols.size() != 0) {
        throw Assembler_Exception(ERR_EQU_DEFINITION_LOOP);
    }
}

void Assembler::finalize_forward_refs() {
    for (auto& symbol_refs : forward_ref_tab) {
        auto symbol = find_symbol(symbol_refs.first);

        if (find(external_symbols.begin(), external_symbols.end(), symbol_refs.first) == external_symbols.end()) {
            string message = format_string(ERR_UNKNOWN_SYMBOL_NOT_DECLARED_EXTERN, symbol_refs.first);
            throw Assembler_Exception(message);
        }

        for (auto& ref : symbol_refs.second) {                
            add_rel_entry(symbol, ref.shndx, ref.offs, ref.type);
        }
    }
}

void Assembler::finalize_global_symbols() {
    for (auto& symbol : global_symbols) {
        if ((symbol_table.find(symbol) == symbol_table.end()) || (symbol_table[symbol]->shndx == UND_NDX)) {
            string message = format_string(ERR_UNDEF_SYM_DECLARED_GLOBAL, symbol);
            throw Assembler_Exception(message);
        }

        symbol_table[symbol]->bind = Elf16_Sym_Bind::ESB_GLOBAL;
    }
}

pair<Word, Word> Assembler::generate_sym_tab() {
    vector<Elf16_ST_Entry> sym_tab;
    vector<Byte> str_tab;

    auto sym_tab_sec = create_section(".symtab", Elf16_Section_Type::EST_SYMTAB);
    auto str_tab_sec = create_section(".strtab", Elf16_Section_Type::EST_STRTAB);
    
    // Insert UND symbol
    string name = "";
    sym_tab.push_back({(Word)str_tab.size(), 0, Elf16_Sym_Bind::ESB_LOCAL, Elf16_Sym_Type::EST_NOTYPE, 0});
    str_tab.insert(str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);

    // Insert SECTION symbols
    for (uint i = 1; i < section_headers.size(); i++) {
        if (section_headers[i].type == Elf16_Section_Type::EST_PROGBITS || section_headers[i].type == Elf16_Section_Type::EST_UND) {
            Elf16_ST_Entry entry = {(Word)str_tab.size(), 0, Elf16_Sym_Bind::ESB_LOCAL, Elf16_Sym_Type::EST_SECTION, (Word)i};
            auto name = section_headers[i].name;

            str_tab.insert(str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);

            sym_tab.push_back(entry);
        }
    }

    // Insert other symbols
    for (auto& symbol : symbol_table) {
        Elf16_ST_Entry entry = {(Word)str_tab.size(), symbol.second->value, symbol.second->bind, Elf16_Sym_Type::EST_NOTYPE, symbol.second->shndx};
        
        auto name = symbol.first;
        str_tab.insert(str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);
        
        symbol.second->stndx = sym_tab.size();
        sym_tab.push_back(entry);
    }

    write_to_section((Byte*)sym_tab.data(), sym_tab.size() * sizeof(Elf16_ST_Entry), sym_tab_sec);
    write_to_section(str_tab.data(), str_tab.size(), str_tab_sec);

    return {sym_tab_sec, str_tab_sec};
}

void Assembler::generate_rel_tabs() {
    for (auto& entry : rel_tables) {
        auto id = create_section(section_headers[entry.first].name + ".rel", Elf16_Section_Type::EST_REL);
        section_headers[entry.first].link = id;

        vector<Elf16_RT_Entry> rel_tab;

        for (auto& rel_entry : entry.second) {
            Elf16_RT_Entry entry;

            if (rel_entry.sym != nullptr) {
                entry = {rel_entry.offs, rel_entry.type, rel_entry.sym->stndx};
            } else {
                entry = {rel_entry.offs, rel_entry.type, rel_entry.sym_sec};
            }

            rel_tab.push_back(entry);
        }

        write_to_section((Byte*)rel_tab.data(), rel_tab.size() * sizeof(Elf16_RT_Entry), id);
    }
}

pair<Elf16_Header, vector<Elf16_SH_Entry>>  Assembler::generate_section_headers(pair<Word, Word>  tab_ndxs) {
    auto sh_str_tab_sec = create_section(".shstrtab", Elf16_Section_Type::EST_STRTAB);
    Elf16_Header header;

    header.type = Elf16_File_Type::EFT_REL;
    header.pentry = 0;
    header.phoffs = 0;
    header.phentries = 0;
    header.symtabndx = tab_ndxs.first;
    header.strndx = tab_ndxs.second;


    vector<Byte> sh_str_tab;
    vector<Elf16_SH_Entry> sh_list;

    Offs prev_size = sizeof(Elf16_Header);

    for (uint i=0; i<section_headers.size() - 1; i++) {
        Elf16_SH_Entry entry = {(Word)sh_str_tab.size(), section_headers[i].type, prev_size, section_headers[i].size, section_headers[i].link};

        auto name = section_headers[i].name;
        sh_str_tab.insert(sh_str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);
        
        prev_size += section_headers[i].size;
        sh_list.push_back(entry);
    }

    auto name = section_headers[sh_str_tab_sec].name;
    Elf16_SH_Entry entry = {(Word)sh_str_tab.size(), section_headers[sh_str_tab_sec].type, prev_size, (Word)(sh_str_tab.size() + name.size() + 1), section_headers[sh_str_tab_sec].link};

    sh_str_tab.insert(sh_str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);
    prev_size += sh_str_tab.size();

    sh_list.push_back(entry);
    write_to_section(sh_str_tab.data(), sh_str_tab.size(), sh_str_tab_sec);

    header.shoffs = prev_size;
    header.shentries = section_headers.size();
    header.shstrndx = sh_str_tab_sec;

    return pair<Elf16_Header, vector<Elf16_SH_Entry>>(header, sh_list);
}


void Assembler::write_binary_output(string out_file_name, pair<Elf16_Header, vector<Elf16_SH_Entry>> headers) {
    ofstream out_file(out_file_name, ios::out | ios::trunc | ios::binary);

    if (out_file.is_open()) {
        out_file.write((char*)&headers.first, sizeof(Elf16_Header));
     
        for (auto &section : binary_sections) {
            out_file.write((char*)section.data(), section.size());
        }   

        out_file.write((char*)headers.second.data(), headers.second.size() * sizeof(Elf16_SH_Entry));
    } else {
        string message = format_string(ERR_OPEN_OUT_FILE_FAILED, out_file_name);
        throw Assembler_Exception(message);
    }

    out_file.close();
}


void Assembler::write_textual_output(string out_file_name, pair<Elf16_Header, vector<Elf16_SH_Entry>> headers) {
    Utility::print_file_header(out_file_name, headers.first);
    Utility::print_section_headers(headers.second, binary_sections[headers.first.shstrndx]);
    
    for (uint i = 0; i<binary_sections.size(); i++) {
        if (section_headers[i].type == Elf16_Section_Type::EST_UND) {
            continue;
        } else if (section_headers[i].type == Elf16_Section_Type::EST_SYMTAB) {
            Utility::print_sym_tab(string((char*)binary_sections[headers.first.shstrndx].data() + headers.second[i].name), (vector<Elf16_ST_Entry>&)binary_sections[i], binary_sections[headers.first.strndx]);
        } else if (section_headers[i].type == Elf16_Section_Type::EST_STRTAB) {
            Utility::print_str_tab(string((char*)binary_sections[headers.first.shstrndx].data() + headers.second[i].name), binary_sections[i]);
        } else if (section_headers[i].type == Elf16_Section_Type::EST_REL) {
            Utility::print_rel_section(string((char*)binary_sections[headers.first.shstrndx].data() + headers.second[i].name), (vector<Elf16_RT_Entry>&)binary_sections[i]);
        } 
        else {
            Utility::print_section(string((char*)binary_sections[headers.first.shstrndx].data() + headers.second[i].name), binary_sections[i]);
        }
    }
}

void Assembler::finalize_assembling(string out_file) {
    finalize_internal_symbols();
    finalize_global_symbols();
    finalize_forward_refs();

    auto ndxs = generate_sym_tab();
    generate_rel_tabs();
    auto headers = generate_section_headers(ndxs);

    // Binary output
    write_binary_output(out_file, headers);

    // Textual output
    write_textual_output(out_file, headers);
}