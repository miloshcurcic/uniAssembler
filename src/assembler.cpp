#include "assembler.h"
#include "utility.h"



Word Assembler::create_section(string name, Elf16_Section_Type type) {
    auto id = section_headers.size();
    // Disallow internal section names?
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

    if (int_symbols.find(name) != int_symbols.end()) {
        // Error, already defined
    }

    if (find(external_symbols.begin(), external_symbols.end(), name) != external_symbols.end()) {
        bind = Elf16_Sym_Bind::ESB_GLOBAL;
    }

    if (find(global_symbols.begin(), global_symbols.end(), name) != global_symbols.end()) {
        bind = Elf16_Sym_Bind::ESB_GLOBAL;
    }

    auto symbol = new AST_Entry({value, bind, shndx});
    symbol_table[name] = symbol;   
}

void Assembler::create_symbol(string name) {
    auto symbol = find_symbol(name);

    if (symbol == nullptr) {
        create_symbol(name, section_headers[current_section].size, current_section);
    } else {
        if (find(external_symbols.begin(), external_symbols.end(), name) != external_symbols.end()) {
            /* ToDo: Error */
            cout << "Error0" << endl;
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

    return symbol->second;
}

AIS_Data* Assembler::find_internal_symbol(string name) {
    auto symbol = int_symbols.find(name);

    if (symbol == int_symbols.end()) {
        return nullptr;
    }

    return symbol->second;
}

void Assembler::set_extern_symbol(string name) {
    if (find(external_symbols.begin(), external_symbols.end(), name) != external_symbols.end()) {
        return;
    }

    if (find(global_symbols.begin(), global_symbols.end(), name) != global_symbols.end()) {
        /* Error, importing own symbol */
    }

    auto symbol = find_symbol(name);

    if (symbol == nullptr) {
        external_symbols.push_back(name);
    } else {
        if (symbol->shndx != UND_NDX) {
            /* ToDo: Error, defined symbol set as extern */
        } else {
            symbol->bind = Elf16_Sym_Bind::ESB_GLOBAL;
        }
    }
}

void Assembler::set_global_symbol(string name) {
    if (find(global_symbols.begin(), global_symbols.end(), name) != global_symbols.end()) {
        return;
    }

    if (find(global_symbols.begin(), global_symbols.end(), name) != global_symbols.end()) {
        /* Error, exporting extern symbol */
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

    if (symbol == nullptr || symbol->shndx == UND_NDX) {
        if (symbol == nullptr && int_symbol == nullptr) {
            create_symbol(name, 0, UND_NDX);
        }

        auto value = (rel_type == Elf16_Rel_Type::ERT_PC16 ? -next_ins_offs : 0);

        add_fw_ref(name, rel_type);
        write_to_cur_section((Byte*)&value, (rel_type == Elf16_Rel_Type::ERT_8 ? sizeof(Byte) : sizeof(Word)));
    } else {
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
    forward_ref_tab[symbol].push_back({(Offs)section_headers[current_section].size, type, current_section});
}

void Assembler::resolve_int_sym_ops(string name) {
    auto symbol = find_symbol(name);

    for(auto int_sym : int_sym_operations[name]) {
        update_internal_symbol(int_sym, int_sym->op_list[name], symbol->value, symbol->shndx);

        int_sym->op_list.erase(name);

        if (int_sym->op_list.size() == 0) {
            create_symbol(int_symbols[int_sym->name]);
        }
    }

    int_sym_operations.erase(name);
}

void Assembler::update_internal_symbol(AIS_Data* symbol, Word coeff, Word value, Word section) {
    symbol->value += coeff * value;
    symbol->class_ndxs[section] += coeff;
}

void Assembler::create_internal_symbol(string name, list<string> operations) {
    if (symbol_table.find(name) != symbol_table.end()) {
        if (symbol_table[name]->shndx != UND_NDX) {
            // Error, already defined
        } else {
            delete symbol_table[name];
            symbol_table.erase(name);
        }
    }

    int_symbols[name] = new AIS_Data();
    int_symbols[name]->name = name;

    for (auto op_string : operations) {
        string value = op_string.substr(1);
        Word coeff = (op_string[0] == '-' ? -1 : 1);

        if (Utility::is_literal(value)) {
            auto val = Utility::cast_literal(value);

            int_symbols[name]->value += coeff * val;
        } else {
            auto symbol = find_symbol(value);

            if (value == name) {
                // symbol definition loop
            }

            if (symbol && symbol->shndx != UND_NDX) {
               update_internal_symbol(int_symbols[name], coeff, symbol->value, symbol->shndx);
            } else {
                int_symbols[name]->op_list[value] += coeff;
                int_sym_operations[value].insert(int_symbols[name]);

                if (int_symbols[name]->op_list[value] == 0) {
                    int_symbols[name]->op_list.erase(value);
                    int_sym_operations.erase(value);
                }
            }
        }
    }

    if (int_symbols[name]->op_list.size() == 0) {
        create_symbol(int_symbols[name]);
    }
}

void Assembler::create_symbol(AIS_Data* int_symbol) {
    auto section = ABS_NDX;
    bool error = false;

    for (auto& entry : int_symbol->class_ndxs) {
        if (entry.first == ABS_NDX) {
            continue;
        }

        if (entry.second == 1 && section == ABS_NDX) {
            section = entry.first;
        }
        
        if (entry.second < 0 || entry.second > 1 || (entry.second == 1 && section != ABS_NDX)) {
            error = true;
            break;
        }
    }

    if (error) {
        // ToDo: Handle error
    }

    create_symbol(int_symbol->name, int_symbol->value, section);

    auto name = int_symbol->name;
    delete int_symbol;
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

                if (symbol->shndx == ABS_NDX) {
                    add_rel_entry(nullptr, current_section, section_headers[current_section].size, entry.type);
                } else {
                    add_rel_entry(symbol->shndx, entry.shndx, entry.offs, entry.type);
                }
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
    for (auto& int_symbol : int_symbols) {

    }
}

void Assembler::finalize_forward_refs() {
    for (auto& symbol_refs : forward_ref_tab) {
        for (auto& ref : symbol_refs.second) {
            auto symbol = find_symbol(symbol_refs.first);

            if (symbol != nullptr) {
                add_rel_entry(symbol, ref.shndx, ref.offs, ref.type);
            } else {
                // ToDo: internal symbol...
            }
        }
    }
}

void Assembler::finalize_global_symbols() {
    for (auto& symbol : global_symbols) {
        if ((symbol_table.find(symbol) == symbol_table.end()) || (symbol_table[symbol]->shndx == UND_NDX)) {
            // error
        }

        symbol_table[symbol]->bind = Elf16_Sym_Bind::ESB_GLOBAL;
    }
}

pair<Word, Word> Assembler::generate_sym_tab() {
    vector<Elf16_ST_Entry> sym_tab;
    vector<Byte> str_tab;

    auto sym_tab_sec = create_section(".symtab", Elf16_Section_Type::EST_SYMTAB);
    auto str_tab_sec = create_section(".str_tab", Elf16_Section_Type::EST_STRTAB);
    
    string name = "";
    sym_tab.push_back({(Word)str_tab.size(), 0, Elf16_Sym_Bind::ESB_LOCAL, Elf16_Sym_Type::EST_NOTYPE, 0});
    str_tab.insert(str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);

    for (uint i = 1; i < section_headers.size(); i++) {
        if (section_headers[i].type == Elf16_Section_Type::EST_PROGBITS || section_headers[i].type == Elf16_Section_Type::EST_UND) {
            Elf16_ST_Entry entry = {(Word)str_tab.size(), 0, Elf16_Sym_Bind::ESB_LOCAL, Elf16_Sym_Type::EST_SECTION, (Word)i};
            auto name = section_headers[i].name;

            str_tab.insert(str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);

            sym_tab.push_back(entry);
        }
    }

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
    auto sh_str_tab_sec = create_section(".sh_str_tab", Elf16_Section_Type::EST_STRTAB);
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
        // Error
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