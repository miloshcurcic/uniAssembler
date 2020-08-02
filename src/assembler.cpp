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
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), Elf16_Section_Type::EST_UND, 0, 0, 0, 0});
    write_to_section("", UND_NDX);
   /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), Elf16_Section_Type::EST_STRTAB, 0, 0, 0, 0});
    write_to_section(".shstrtab", SH_STR_TAB_NDX);
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), Elf16_Section_Type::EST_SYMTAB, 0, 0, 0, 0});
    write_to_section(".symtab", SYM_TAB_NDX);
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), Elf16_Section_Type::EST_SYMTAB, 0, 0, 0, 0});
    write_to_section(".strtab", STR_TAB_NDX);
}

void Assembler::write_fw_ref_cur(string name, const Elf16_Byte *data, Elf16_UWord num) {
    add_forward_ref(name, section_headers[current_section].size);
    write_to_cur_section(data, num);
}

void Assembler::add_forward_ref(string name, Elf16_Addr addr) {
    sym_fw_ref_tab[symbol_ndxs[name]].push_back(addr);
}

void Assembler::resolve_forward_refs(string name, Elf16_Word value) {
    auto section_ndx = find_symbol(name)->shndx;
    auto section = sections[section_ndx].data();
    for (auto addr : sym_fw_ref_tab[symbol_ndxs[name]]) {
        section[addr] += value;
    }
    sym_fw_ref_tab.erase(symbol_ndxs[name]);
}

void Assembler::add_rel_entry(Elf16_UWord section, Elf16_Addr offs, Elf16_Rel_Type type, Elf16_UWord stndx) {
    auto rel_section_ndx = section_headers[section].rel;
    Elf16_RT_Entry entry{offs, type, stndx};
    write_to_section((Elf16_Byte*)&entry, sizeof(Elf16_ST_Entry), rel_section_ndx);
}

void Assembler::resolve_forward_refs(Elf16_Word ndx, Elf16_Word value) {
    auto section_ndx = find_symbol(ndx)->shndx;
    auto section = sections[section_ndx].data();
    for (auto addr : sym_fw_ref_tab[ndx]) {
        section[addr] += value;
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

Elf16_UWord Assembler::create_section(string name, Elf16_Section_Type type) {
    section_ndxs[name] = sections.size();
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NDX].size(), type, 0, 0, 0, 0});
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