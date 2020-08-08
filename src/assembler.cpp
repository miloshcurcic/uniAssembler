#include "assembler.h"
#include "utility.h"

Assembler::Assembler() {
    section_ndxs[".shstrtab"] = SH_STR_TAB_NDX;
    section_ndxs[".symtab"] = SYM_TAB_NDX;
    section_ndxs[".strtab"] = STR_TAB_NDX;
    
    sections.emplace_back();
    sections.emplace_back();
    sections.emplace_back();
    sections.emplace_back();
    
    /* ToDo: 0's */
    section_headers.push_back({0, Elf16_Section_Type::EST_UND, 0, 0, 0, 0});
    section_headers.push_back({sizeof(""), Elf16_Section_Type::EST_STRTAB, 0, 0, 0, 0});
    section_headers.push_back({sizeof("") + sizeof(".shstrtab"), Elf16_Section_Type::EST_STRTAB, 0, 0, 0, 0});
    section_headers.push_back({sizeof("") + sizeof(".shstrtab") + sizeof(".symtab"), Elf16_Section_Type::EST_SYMTAB, 0, 0, 0, 0});

    write_to_section("", SH_STR_TAB_NDX);
    write_to_section(".shstrtab", SH_STR_TAB_NDX);
    write_to_section(".strtab", SH_STR_TAB_NDX);
    write_to_section(".symtab", SH_STR_TAB_NDX);

    create_symbol("", 0, Elf16_Sym_Link::ESL_LOCAL, UND_NDX);
}


Elf16_UWord Assembler::create_section(string name, Elf16_Section_Type type) {
    auto id = section_headers.size();
    // Disallow internal section names?
    section_headers.emplace_back(name, type);
    section_ndxs[name] = id; 
    return id;
}

void Assembler::switch_section(string name) {
    auto section = section_ndxs.find(name);
    Elf16_UWord result;
    
    if (section == section_ndxs.end()) {
        result = create_section(name, Elf16_Section_Type::EST_PROGBITS);
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
}

AST_Entry* Assembler::find_symbol(string name) {
    auto symbol = symbol_table.find(name);

    if (symbol == symbol_table.end()) {
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

    if (symbol == nullptr || symbol->shndx == UND_NDX || int_symbol == nullptr && int_symbol->ops != 0) {
        if (symbol == nullptr && int_symbol == nullptr) {
            create_symbol(name, 0, UND_NDX);
        }

        // Negative next instruction offset or zero.
        auto value = (rel_type == Elf16_Rel_Type::ERT_PC16 ? -next_ins_offs : 0);

        write_and_add_fw_ref(name, rel_type, (Elf16_Byte*)&value);
    } else {
        if (rel_type == Elf16_Rel_Type::ERT_PC16) {
            if (symbol && symbol->shndx == current_section) {
                    /* ToDo: check calculation */
                   auto value = symbol->value - next_ins_offs - section_headers[current_section].size;

                    write_to_cur_section((Elf16_Byte*)&value, sizeof(Elf16_Offs));
                } else {
                    // Negative next instruction offset.
                    auto value = -next_ins_offs;
                    write_and_add_rel_entry(name, rel_type, (Elf16_Byte*)&value);
                }
        } else {
            /* ToDo: Control symbol size? */
            if (symbol != nullptr) {
                write_and_add_rel_entry(section_headers[symbol->shndx].name, rel_type, (Elf16_Byte*)&symbol->value); 
            } else {
                if (rel_type == Elf16_Rel_Type::ERT_16) {
                    write_to_cur_section((Elf16_Byte*)&int_symbol->value, sizeof(Elf16_Word));
                } else {
                    write_to_cur_section((Elf16_Byte*)&int_symbol->value, sizeof(Elf16_Half));
                }
            }
        }
    }
}

void Assembler::add_rel_entry(string symbol, Elf16_UWord section, Elf16_Addr offs, Elf16_Rel_Type type) {
    if (section_headers[section].rel == UND_NDX) {
        auto id = create_section(section_headers[section].name + ".rel", Elf16_Section_Type::EST_REL);           
        section_headers[section].rel = id;
    }

    rel_tables[section].emplace_back(offs, type, symbol_table[symbol]);
}

void Assembler::write_and_add_rel_entry(string symbol, Elf16_Rel_Type type, const Elf16_Byte *data) {
    add_rel_entry(symbol, current_section, section_headers[current_section].size, type);
    
    if (type == Elf16_Rel_Type::ERT_8) {
        write_to_cur_section(data, sizeof(Elf16_Half));
    } else {
        write_to_cur_section(data, sizeof(Elf16_Word));    
    }
}

void Assembler::write_and_add_fw_ref(string symbol, Elf16_Rel_Type type, const Elf16_Byte *data) {
    forward_ref_tab[symbol].push_back({section_headers[current_section].size, type, current_section});
    
    if (type == Elf16_Rel_Type::ERT_8) {
        write_to_cur_section(data, sizeof(Elf16_Half));
    } else {
        write_to_cur_section(data, sizeof(Elf16_Word));    
    }
}

void Assembler::resolve_int_sym_ops(string name, Elf16_Word value) {
    for(auto& operation : int_sym_operations[name]) {
        if (operation.subtract) {
            operation.symbol->value -= value;
        } else {
            operation.symbol->value += value;
        }
        operation.symbol->ops--;

        if (operation.symbol->ops == 0) {
            resolve_forward_refs(operation.symbol->name);
            resolve_int_sym_ops(operation.symbol->name, operation.symbol->value);
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
    vector<AIS_Operation> operations;

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

            if (symbol->shndx != UND_NDX) {
               update_internal_symbol(int_symbols[name], subtract, symbol->value, symbol->shndx);
            } else {
                AIS_Operation operation;

                if (subtract) {
                    operation.subtract = true;
                }

                operation.symbol = int_symbols[name];
                int_symbols[name]->ops++;
                int_sym_operations[value.substr(1)].push_back(operation);
            }
        }
    }

    if (int_symbols[name]->ops == 0) {
        resolve_forward_refs(name);
        resolve_int_sym_ops(name, int_symbols[name]->value);
    }
}

// ToDo
void Assembler::resolve_forward_refs(string name) {
    auto symbol = find_symbol(name);
    auto int_symbol = find_internal_symbol(name);

    if (symbol != nullptr) {    
        for (auto entry : forward_ref_tab[name]) {
            auto section = binary_sections[entry.shndx].data();

            switch (entry.type) {
                case Elf16_Rel_Type::ERT_8: {
                    (Elf16_Half&)section[entry.addr] += (Elf16_Half)symbol->value;
                    add_rel_entry(section_headers[symbol->shndx].name, entry.shndx, entry.addr, entry.type);
                    break;
                }
                case Elf16_Rel_Type::ERT_16: {
                    (Elf16_Word&)(section[entry.addr]) += (Elf16_Word)symbol->value;
                    add_rel_entry(section_headers[symbol->shndx].name, entry.shndx, entry.addr, entry.type);

                    break;
                }
                case Elf16_Rel_Type::ERT_PC16: {
                    if (symbol->shndx == entry.shndx) {
                        (Elf16_Word&)(section[entry.addr]) += symbol->value - entry.addr;
                    } else {
                        add_rel_entry(name, entry.shndx, entry.addr, entry.type);
                    }

                    break;
                }
            }
        }
    } else {
        for (auto entry : forward_ref_tab[name]) {
            if (entry.type == Elf16_Rel_Type::ERT_8) {
                
            } else {

            }
        }
    }

    forward_ref_tab.erase(name);
}

void Assembler::check_global_symbols() {
    auto num = section_headers[SYM_TAB_NDX].size / sizeof(Elf16_ST_Entry);
    auto base = ((Elf16_ST_Entry*)sections[SYM_TAB_NDX].data());

    for(ulong i = 1; i < num; i++) {
        if (find(external_symbols.begin(), external_symbols.end(), string((char*)&sections[STR_TAB_NDX][(base + i)->name])) == external_symbols.end()) {
            if ((base + i)->shndx == UND_NDX) {
                /* ToDo: Error */
                cout << "Error2:" << string((char*)&sections[STR_TAB_NDX][(base + i)->name]) << endl;
            }
        }
    }
}

void Assembler::update_headers() {
    header.type = Elf16_File_Type::EFT_REL;
    header.pentry = 0;
    header.phoffs = 0;
    header.phentries = 0;
    header.shoffs = sizeof(Elf16_Header);
    for (auto &value : section_headers) {
        header.shoffs += value.size;
    }
    header.shentries = section_headers.size();
    header.shstrndx = SH_STR_TAB_NDX;

    auto sum = sizeof(header);
    for (auto &header : section_headers) {
        header.offs = sum;
        sum += header.size;
    }
}

void Assembler::finalize_assembling() {
    check_global_symbols();
    generate_rel_tables();
    update_headers();

    // Binary output
    ofstream out_file("out.o", ios::out | ios::trunc | ios::binary);

    if (out_file.is_open()) {
        out_file.write((char*)&header, sizeof(Elf16_Header));
        
        for (auto section : sections) {
            out_file.write((char*)section.data(), section.size());
        }

        out_file.write((char*)section_headers.data(), section_headers.size() * sizeof(Elf16_SH_Entry));
    } else {
        // Error
    }

    out_file.close();

    // Textual output
    Utility::print_file_header("out.o", header);
    Utility::print_section_headers(section_headers);

    for (uint i = 0; i<sections.size(); i++) {
        if (i == UND_NDX) {
            continue;
        } else if (i == SYM_TAB_NDX) {
            Utility::print_sym_tab(string((char*)&sections[SH_STR_TAB_NDX][section_headers[i].name]), (vector<Elf16_ST_Entry>&)sections[i]);
        } else if (i == STR_TAB_NDX || i == SH_STR_TAB_NDX) {
            Utility::print_str_tab(string((char*)&sections[SH_STR_TAB_NDX][section_headers[i].name]), sections[i]);
        } else if (section_headers[i].type == Elf16_Section_Type::EST_REL) {
            Utility::print_rel_section(string((char*)&sections[SH_STR_TAB_NDX][section_headers[i].name]), (vector<Elf16_RT_Entry>&)sections[i]);
        } 
        else {
            Utility::print_section(string((char*)&sections[SH_STR_TAB_NDX][section_headers[i].name]), sections[i]);
        }
    }

}

void Assembler::generate_rel_tables() {
    for (auto entry : sym_fw_ref_tab) {
        for(auto fw_ref : entry.second) {
            add_rel_entry(fw_ref.shndx, fw_ref.addr, fw_ref.type, entry.first);
        }
    }
}