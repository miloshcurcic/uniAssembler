#include "assembler.h"
#include "utility.h"



Elf16_UWord Assembler::create_section(string name, Elf16_Section_Type type) {
    auto id = section_headers.size();
    // Disallow internal section names?
    section_headers.emplace_back(name, type);
    binary_sections.emplace_back();
    section_ndxs[name] = id; 
    return id;
}

void Assembler::switch_section(string name) {
    auto section = section_ndxs.find(name);
    Elf16_UWord result;
    
    if (section == section_ndxs.end()) {
        result = create_section(name);
        create_symbol(name, 0, Elf16_Sym_Link::ESL_LOCAL);
    } else {
        result = section->second;
    }

    current_section = result;
}

void Assembler::write_to_section(string data, Elf16_UWord section) {
    write_to_section((Elf16_Byte*)data.c_str(), data.length() + 1, section);
}

void Assembler::write_to_section(const Elf16_Byte *data, Elf16_UWord num, Elf16_UWord section) {
    binary_sections[section].insert(binary_sections[section].end(), data, data + num);
    section_headers[section].size += num;
}

void Assembler::write_to_cur_section(const Elf16_Byte *data, Elf16_UWord num) {
    write_to_section(data, num, current_section);
}


void Assembler::create_symbol(string name, Elf16_UWord value, Elf16_UWord shndx) {
    Elf16_Sym_Link link = Elf16_Sym_Link::ESL_LOCAL;

    if (int_symbols.find(name) != int_symbols.end()) {
        // Error, already defined
    }

    if (find(external_symbols.begin(), external_symbols.end(), name) != external_symbols.end()) {
        link = Elf16_Sym_Link::ESL_GLOBAL;
    }

    if (find(global_symbols.begin(), global_symbols.end(), name) != global_symbols.end()) {
        link = Elf16_Sym_Link::ESL_GLOBAL;
    }

    auto symbol = new AST_Entry({value, link, shndx});
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
            symbol->link = Elf16_Sym_Link::ESL_GLOBAL;
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
        symbol->link = Elf16_Sym_Link::ESL_GLOBAL;
    }
}

void Assembler::handle_symbol(string name, Elf16_Rel_Type rel_type, Elf16_Offs next_ins_offs) {
    auto symbol = find_symbol(name);
    auto int_symbol = find_internal_symbol(name);

    if (symbol == nullptr || symbol->shndx == UND_NDX) {
        if (symbol == nullptr && int_symbol == nullptr) {
            create_symbol(name, 0, UND_NDX);
        }

        auto value = (rel_type == Elf16_Rel_Type::ERT_PC16 ? -next_ins_offs : 0);

        add_fw_ref(name, rel_type);
        write_to_cur_section((Elf16_Byte*)&value, (rel_type == Elf16_Rel_Type::ERT_8 ? sizeof(Elf16_Half) : sizeof(Elf16_Word)));
    } else {
        if (rel_type == Elf16_Rel_Type::ERT_PC16) {
            if (symbol->shndx == current_section) {
                auto value = symbol->value - next_ins_offs - section_headers[current_section].size;

                write_to_cur_section((Elf16_Byte*)&value, sizeof(Elf16_Offs));
            } else {
                auto value = symbol->value - next_ins_offs;
            
                if (symbol->shndx == ABS_NDX) {
                    add_rel_entry(nullptr, current_section, section_headers[current_section].size, rel_type);
                } else {
                    add_rel_entry(symbol->shndx, current_section, section_headers[current_section].size, rel_type);
                }
                write_to_cur_section((Elf16_Byte*)&value, sizeof(Elf16_Offs));
            }
        } else {
            auto value = symbol->value;

            if (symbol->shndx != ABS_NDX) {
                add_rel_entry(symbol->shndx, current_section, section_headers[current_section].size, rel_type);
            }

            if (rel_type == Elf16_Rel_Type::ERT_8) {
                write_to_cur_section((Elf16_Byte*)&value, sizeof(Elf16_Half));   
            } else {
                write_to_cur_section((Elf16_Byte*)&value, sizeof(Elf16_Word));
            }
        }
    }
}

void Assembler::add_rel_entry(AST_Entry* symbol, Elf16_UWord section, Elf16_Offs offs, Elf16_Rel_Type type) {
    rel_tables[section].push_back({offs, type, symbol, 0});
}

void Assembler::add_rel_entry(Elf16_UWord symbol_section, Elf16_UWord section, Elf16_Offs offs, Elf16_Rel_Type type) {
    rel_tables[section].push_back({offs, type, nullptr, symbol_section});
}

void Assembler::add_fw_ref(string symbol, Elf16_Rel_Type type) {
    forward_ref_tab[symbol].push_back({(Elf16_Offs)section_headers[current_section].size, type, current_section});
}

void Assembler::resolve_int_sym_ops(string name) {
    auto symbol = find_symbol(name);
    
    for(auto int_sym : int_sym_operations[name]) {
        for (auto subtract : int_sym->op_list[name]) {
            update_internal_symbol(int_sym, subtract, symbol->value, symbol->shndx);
        }

        int_sym->op_list.erase(name);

        if (int_sym->op_list.size() == 0) {
            create_symbol(int_symbols[int_sym->name]);
        }
    }
}

void Assembler::update_internal_symbol(AIS_Data* symbol, bool subtract, Elf16_Word value, Elf16_UWord section) {
    if (subtract) {
        symbol->value -= value;
        if (symbol->class_ndxs.find(section) != symbol->class_ndxs.end()) {
            symbol->class_ndxs[section]--;
        } else {
            symbol->class_ndxs[section] = -1;
        }
    } else {
        symbol->value += value;
        if (symbol->class_ndxs.find(section) != symbol->class_ndxs.end()) {
            symbol->class_ndxs[section]++;
        } else {
            symbol->class_ndxs[section] = 1;
        }
    }
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
        bool subtract = op_string[0] == '-';

        if (Utility::is_literal(value)) {
            auto val = Utility::cast_literal(value);

            if (subtract) {
                int_symbols[name]->value -= val;    
            } else {
                int_symbols[name]->value += val;
            }
        } else {
            auto symbol = find_symbol(value);

            if (symbol && symbol->shndx != UND_NDX) {
               update_internal_symbol(int_symbols[name], subtract, symbol->value, symbol->shndx);
            } else {
                int_symbols[name]->op_list[value.substr(1)].push_back(subtract);
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

    if (section == ABS_NDX) {
        create_symbol(int_symbol->name, int_symbol->value, ABS_NDX);
    } else {
        // ToDo: ...
    }
    
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
                (Elf16_Offs&)*(binary_sections[entry.shndx].data() + entry.offs) += symbol->value - entry.offs;
            } else {
                (Elf16_Offs&)*(binary_sections[entry.shndx].data() + entry.offs) += symbol->value;

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
                (Elf16_Half&)*(binary_sections[entry.shndx].data() + entry.offs) += symbol->value;
            } else {
                (Elf16_Word&)*(binary_sections[entry.shndx].data() + entry.offs) += symbol->value;
            }
        }
    }

    forward_ref_tab.erase(name);
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
        if (symbol_table[symbol]->shndx == UND_NDX) {
            // error
        }

        symbol_table[symbol]->link = Elf16_Sym_Link::ESL_LOCAL;
    }
}

void Assembler::generate_sym_tab() {
    vector<Elf16_ST_Entry> sym_tab;
    vector<Elf16_Byte> str_tab;

    auto sym_tab_sec = create_section(".symtab", Elf16_Section_Type::EST_SYMTAB);
    auto str_tab_sec = create_section(".str_tab", Elf16_Section_Type::EST_STRTAB);

    for (uint i = 0; i <= section_headers.size(); i++) {
        Elf16_ST_Entry entry = {(Elf16_UWord)str_tab.size(), 0, Elf16_Sym_Link::ESL_LOCAL, (Elf16_UWord)i};

        auto name = section_headers[i].name;
        str_tab.insert(str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);
        
        sym_tab.push_back(entry);
    }

    for (auto& symbol : symbol_table) {
        Elf16_ST_Entry entry = {(Elf16_UWord)str_tab.size(), symbol.second->value, symbol.second->link, symbol.second->shndx};
        
        auto name = symbol.first;
        str_tab.insert(str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);
        
        symbol.second->stndx = sym_tab.size();
        sym_tab.push_back(entry);
    }

    write_to_section((Elf16_Byte*)sym_tab.data(), sym_tab.size() * sizeof(Elf16_ST_Entry), sym_tab_sec);
    write_to_section(str_tab.data(), str_tab.size(), str_tab_sec);
}

void Assembler::generate_rel_tabs() {
    for (auto& entry : rel_tables) {
        auto id = create_section(section_headers[entry.first].name + ".rel", Elf16_Section_Type::EST_REL);
        section_headers[entry.first].rel = id;

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

        write_to_section((Elf16_Byte*)rel_tab.data(), rel_tab.size() * sizeof(Elf16_RT_Entry), id);
    }
}

pair<Elf16_Header, vector<Elf16_SH_Entry>>  Assembler::generate_section_headers() {
    auto sh_str_tab_sec = create_section(".sh_str_tab", Elf16_Section_Type::EST_STRTAB);
    Elf16_Header header;

    header.type = Elf16_File_Type::EFT_REL;
    header.pentry = 0;
    header.phoffs = 0;
    header.phentries = 0;


    vector<Elf16_Byte> sh_str_tab;
    vector<Elf16_SH_Entry> sh_list;

    Elf16_Offs prev_size = sizeof(Elf16_Header);

    for (uint i=0; i<section_headers.size() - 1; i++) {
        Elf16_SH_Entry entry = {(Elf16_UWord)sh_str_tab.size(), section_headers[i].type, 0, prev_size, section_headers[i].size, section_headers[i].rel};

        auto name = section_headers[i].name;
        sh_str_tab.insert(sh_str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);
        
        prev_size += section_headers[i].size;
        sh_list.push_back(entry);
    }

    auto name = section_headers[sh_str_tab_sec].name;
    Elf16_SH_Entry entry = {(Elf16_UWord)sh_str_tab.size(), section_headers[sh_str_tab_sec].type, 0, prev_size, (Elf16_UWord)(sh_str_tab.size() + name.size() + 1), section_headers[sh_str_tab_sec].rel};

    sh_str_tab.insert(sh_str_tab.end(), name.c_str(), name.c_str() + name.length() + 1);
    prev_size += sh_str_tab.size();

    sh_list.push_back(entry);
    write_to_section(sh_str_tab.data(), sh_str_tab.size(), sh_str_tab_sec);

    header.shoffs = prev_size;
    header.shentries = section_headers.size();
    header.shstrndx = sh_str_tab_sec;

    return pair<Elf16_Header, vector<Elf16_SH_Entry>>(header, sh_list);
}


void Assembler::write_binary_output(pair<Elf16_Header, vector<Elf16_SH_Entry>> headers) {
    ofstream out_file("out.o", ios::out | ios::trunc | ios::binary);

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


void Assembler::write_textual_output(pair<Elf16_Header, vector<Elf16_SH_Entry>> headers) {
    Utility::print_file_header("out.o", headers.first);
    Utility::print_section_headers(headers.second);
    
    for (uint i = 0; i<binary_sections.size(); i++) {
        if (section_headers[i].type == Elf16_Section_Type::EST_UND) {
            continue;
        } else if (section_headers[i].type == Elf16_Section_Type::EST_SYMTAB) {
            Utility::print_sym_tab(string((char*)binary_sections[headers.first.shstrndx].data() + headers.second[i].name), (vector<Elf16_ST_Entry>&)binary_sections[i]);
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

void Assembler::finalize_assembling() {
    finalize_global_symbols();
    finalize_forward_refs();

    generate_sym_tab();
    generate_rel_tabs();
    
    auto res = generate_section_headers();

    // Binary output
    write_binary_output(res);

    // Textual output
    write_textual_output(res);
}