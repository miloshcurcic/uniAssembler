#include "assembler.h"
#include "utility.h"
#include "logger.h"

Assembler::Assembler() {
    section_ndxs[".shstrtab"] = SH_STR_TAB_NDX;
    section_ndxs[".symtab"] = SYM_TAB_NDX;
    section_ndxs[".strtab"] = STR_TAB_NDX;
    
    sections.emplace_back();
    sections.emplace_back();
    sections.emplace_back();
    sections.emplace_back();
    
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), Elf16_Section_Type::EST_UND, 0, 0, 0, 0});
    write_to_section("", UND_NDX);
   /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), Elf16_Section_Type::EST_STRTAB, 0, 0, 0, 0});
    write_to_section(".shstrtab", SH_STR_TAB_NDX);
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), Elf16_Section_Type::EST_STRTAB, 0, 0, 0, 0});
    write_to_section(".strtab", SH_STR_TAB_NDX);
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), Elf16_Section_Type::EST_SYMTAB, 0, 0, 0, 0});
    write_to_section(".symtab", SH_STR_TAB_NDX);

}

void Assembler::write_fw_ref_cur(string name, const Elf16_Byte *data, Elf16_Rel_Type type) {
    add_forward_ref(name, section_headers[current_section].size, type);
    if (type == Elf16_Rel_Type::ERT_8) {
        write_to_cur_section(data, sizeof(Elf16_Byte));
    } else {
        write_to_cur_section(data, sizeof(Elf16_Word));    
    }
}

void Assembler::add_forward_ref(string name, Elf16_Addr addr, Elf16_Rel_Type type) {
    sym_fw_ref_tab[symbol_ndxs[name]].push_back({addr, type, current_section});
}

void Assembler::resolve_forward_refs(string name) {
    auto symbol = find_symbol(name);
    
    if (symbol != nullptr) {
        for (auto entry : sym_fw_ref_tab[symbol_ndxs[name]]) {
            auto section = sections[entry.shndx].data();
            if (entry.type == Elf16_Rel_Type::ERT_8) {
                section[entry.addr] += (Elf16_Byte)symbol->value;
            } else {
                (Elf16_Word&)(section[entry.addr]) += symbol->value;
            }
        }
        sym_fw_ref_tab.erase(symbol_ndxs[name]);
    }
}

void Assembler::write_rel_entry(Elf16_UWord section, Elf16_Addr offs, Elf16_Rel_Type type, Elf16_UWord stndx) {
    Elf16_RT_Entry entry{offs, type, stndx};
    write_to_section((Elf16_Byte*)&entry, sizeof(Elf16_RT_Entry), section);
}

void Assembler::resolve_forward_refs(Elf16_Word ndx) {
    auto symbol = find_symbol(ndx);
    auto section_ndx = symbol->shndx;
    auto section = sections[section_ndx].data();
    for (auto entry : sym_fw_ref_tab[ndx]) {
        if (entry.type == Elf16_Rel_Type::ERT_8) {
            section[entry.addr] += symbol->value;
        } else {
            (Elf16_Word&)(section[entry.addr]) += symbol->value;
        }
    }
    sym_fw_ref_tab.erase(ndx);
}

Elf16_ST_Entry* Assembler::find_symbol(string name) {
    auto symbol_ndx = symbol_ndxs.find(name);
    
    if (symbol_ndx == symbol_ndxs.end()) {
        return nullptr;
    }

    return ((Elf16_ST_Entry*)sections[SYM_TAB_NDX].data()) + symbol_ndx->second;
}

Elf16_ST_Entry* Assembler::find_symbol(Elf16_Word ndx) {
    return ((Elf16_ST_Entry*)sections[SYM_TAB_NDX].data()) + ndx;
}

void Assembler::add_symbol(string name, Elf16_UWord value, Elf16_Sym_Link link, Elf16_UWord section) {
    Elf16_ST_Entry entry{(Elf16_UWord)sections[STR_TAB_NDX].size(), value, link, section};
    symbol_ndxs[name] = section_headers[SYM_TAB_NDX].size / sizeof(Elf16_ST_Entry);
    write_to_section((Elf16_Byte*)&entry, sizeof(Elf16_ST_Entry), SYM_TAB_NDX);
    write_to_section(name, STR_TAB_NDX);
}

void Assembler::add_or_set_symbol_cur(string name, Elf16_Sym_Link link) {
    auto symbol = find_symbol(name);

    if (symbol == nullptr) {
        add_symbol(name, section_headers[current_section].size, link, current_section);
    } else {
        if (find(external_symbols.begin(), external_symbols.end(), name) != external_symbols.end()) {
            /* ToDo: Error */
            cout << "Error" << endl;
        }

        symbol->value = section_headers[current_section].size;
        symbol->shndx = current_section;
    }
}

void Assembler::add_or_set_extern_symbol(string name) {
    auto symbol = find_symbol(name);
    
    if (symbol == nullptr) {
        external_symbols.push_back(name);
        add_symbol(name, 0, Elf16_Sym_Link::ESL_GLOBAL, UND_NDX);
    } else {
        if (symbol->shndx != UND_NDX) {
            /* ToDo: Error */
            cout << "Error" << endl;
        } else {
            if (find(external_symbols.begin(), external_symbols.end(), name) != external_symbols.end()) {
                return;
            }

            symbol->link = Elf16_Sym_Link::ESL_GLOBAL;
        }
    }
}

void Assembler::add_or_set_global_symbol(string name) {
    auto symbol = find_symbol(name);
    
    if (symbol == nullptr) {
        external_symbols.push_back(name);
        add_symbol(name, 0, Elf16_Sym_Link::ESL_GLOBAL, UND_NDX);
    } else {
        symbol->link = Elf16_Sym_Link::ESL_GLOBAL;
    }
}

Elf16_UWord Assembler::create_section(string name, Elf16_Section_Type type) {
    section_ndxs[name] = sections.size();
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), type, 0, 0, 0, UND_NDX});
    sections.emplace_back();
    write_to_section(name, SH_STR_TAB_NDX);
    return section_ndxs[name];
}

void Assembler::write_to_section(string data, Elf16_UWord section) {
    write_to_section((Elf16_Byte*)data.c_str(), data.length() + 1, section);
}

void Assembler::write_to_section(const Elf16_Byte *data, Elf16_UWord num, Elf16_UWord section) {
    sections[section].insert(sections[section].end(), data, data + num);
    section_headers[section].size += num;
}

void Assembler::switch_to_section(string name) {
    auto section = section_ndxs.find(name);

    Elf16_UWord result;
    if (section == section_ndxs.end()) {
        result = create_section(name, Elf16_Section_Type::EST_PROGBITS);
    } else {
        result = section->second;
    }
    current_section = result;
}

void Assembler::write_to_cur_section(const Elf16_Byte *data, Elf16_UWord num) {
    write_to_section(data, num, current_section);
}

void Assembler::check_global_symbols() {
    auto num = section_headers[SYM_TAB_NDX].size / sizeof(Elf16_ST_Entry);
    auto base = ((Elf16_ST_Entry*)sections[SYM_TAB_NDX].data());

    for(ulong i = 0; i < num; i++) {
        if (find(external_symbols.begin(), external_symbols.end(), string((char*)&sections[STR_TAB_NDX][(base + i)->name])) == external_symbols.end()) {
            if ((base + i)->shndx == UND_NDX) {
                /* ToDo: Error */
                cout << "Error" << endl;
            }
        }
    }
}

void Assembler::generate_rel_tables() {
    for (auto entry : sym_fw_ref_tab) {
        for(auto fw_ref : entry.second) {
            if (section_headers[fw_ref.shndx].rel == UND_NDX) {
                auto id = create_section(string((char*)&sections[SH_STR_TAB_NDX][section_headers[fw_ref.shndx].name]) + ".rel", Elf16_Section_Type::EST_REL);           
                section_headers[fw_ref.shndx].rel = id;
            }

            write_rel_entry(section_headers[fw_ref.shndx].rel, fw_ref.addr, fw_ref.type, entry.first);
        }
    }
}

void Assembler::finalize_assembling() {
    check_global_symbols();
    generate_rel_tables();

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

    Logger::write_log("Done.");
}