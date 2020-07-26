#include "assembler.h"
#include "utility.h"

Assembler::Assembler() {
    section_nxs[".shstrtab"] = SH_STR_TAB_NX;
    section_nxs[".symtab"] = SYM_TAB_NX;
    section_nxs[".strtab"] = STR_TAB_NX;
    sections.emplace_back();
    sections.emplace_back();
    sections.emplace_back();
    sections.emplace_back();
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NX].size(), Elf16_Section_Type::UND, 0, 0, 0, 0});
    write_to_section("", UND_NX);
   /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NX].size(), Elf16_Section_Type::STRTAB, 0, 0, 0, 0});
    write_to_section(".shstrtab", SH_STR_TAB_NX);
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NX].size(), Elf16_Section_Type::SYMTAB, 0, 0, 0, 0});
    write_to_section(".symtab", SYM_TAB_NX);
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NX].size(), Elf16_Section_Type::SYMTAB, 0, 0, 0, 0});
    write_to_section(".strtab", STR_TAB_NX);
}

Elf16_ST_Entry* Assembler::find_symbol(string name) {
    auto symbol_nx = symbol_nxs.find(name);
    
    if (symbol_nx == symbol_nxs.end()) {
        return nullptr;
    }

    return ((Elf16_ST_Entry*)sections[SYM_TAB_NX].data()) + symbol_nx->second;
}

void Assembler::add_symbol(string name, Elf16_UWord value, Elf16_Sym_Link link, Elf16_UWord section) {
    Elf16_ST_Entry entry{(Elf16_UWord)sections[STR_TAB_NX].size(), value, link, section};
    section_nxs[name] = section_headers[SYM_TAB_NX].size / sizeof(Elf16_ST_Entry);
    write_to_section((Elf16_Byte*)&entry, sizeof(Elf16_ST_Entry), SYM_TAB_NX);
    write_to_section(name, STR_TAB_NX);
}

void Assembler::create_section(string name, Elf16_Section_Type type) {
    section_nxs[name] = sections.size();
    /* ToDo: 0's */
    section_headers.push_back({(Elf16_UWord)sections[SH_STR_TAB_NX].size(), type, 0, 0, 0, 0});
    sections.emplace_back();
    write_to_section(name, SH_STR_TAB_NX);
}

void Assembler::write_to_section(string data, Elf16_UWord section) {
    write_to_section((Elf16_Byte*)data.c_str(), data.length() + 1, section);
}
void Assembler::write_to_section(const Elf16_Byte *data, Elf16_UWord num, Elf16_UWord section) {
    sections[section].insert(sections[section].end(), data, data + num);
    section_headers[section].size += num;
}