#ifndef _ELF_H_
#define _ELF_H_

#include "includes.h"

enum Elf16_File_Type : Elf16_Byte {
    REL_F,
    EXEC_F
};

struct Elf16_Header {
    Elf16_File_Type type;
    //Elf16_Addr pentry; // defining this?
    Elf16_Offs phoffs;
    Elf16_UWord phentries;
    Elf16_Offs shoffs;
    Elf16_UWord shentries;
    Elf16_UWord shstrndx;
};

enum Elf16_Section_Type : Elf16_Byte {
    UND,
    PROGBITS,
    SYMTAB,
    STRTAB,
    REL
};

enum Elf16_Section_Flag : Elf16_Byte {
    WRITE,
    ALLOC,
    EXECINSTR
};

struct Elf16_SH_Entry {
    Elf16_UWord name;
    Elf16_Section_Type type;
    Elf16_Byte flags;
    Elf16_Offs offs;
    Elf16_UWord size;
    Elf16_UWord rel;
};

enum Elf16_Sym_Link : Elf16_Byte {
    LOCAL,
    GLOBAL
};

struct Elf16_ST_Entry {
    Elf16_UWord name;
    Elf16_UWord value;
    Elf16_Sym_Link link;
    Elf16_UWord shndx;
};

enum Elf16_Rel_Type : Elf16_Byte {
    R_16,
    R_PC16
};

struct Elf16_RT_Entry {
    Elf16_Addr offs;
    Elf16_Rel_Type type;
    Elf16_UWord stndx;
};

#endif