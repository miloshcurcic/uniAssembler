#ifndef _ASM_EXCEPTION_H_
#define _ASM_EXCEPTION_H_

#include <iostream>
#include <string>
#include <memory>
#include <string>
#include <stdexcept>

using namespace std;

const string ERR_DEFINED_SYM_DECLARED_EXTERN = "Defined symbol '%s' declared as EXTERN.";
const string ERR_UNDEF_SYM_DECLARED_GLOBAL = "Undefined symbol '%s' declared as GLOBAL.";
const string ERR_UNKNOWN_SYMBOL_NOT_DECLARED_EXTERN = "Undefined symbol '%s' is not declared as EXTERN.";
const string ERR_SYM_DECLARED_EXT_AND_GLB = "Symbol '%s' declared as both GLOBAL and EXTERN.";
const string ERR_MULTIPLE_DEFINITIONS = "Found multiple definitions for symbol '%s'.";
const string ERR_SYMBOL_DEFINITION_LOOP = "Symbol definition loop found for symbol '%s'.";
const string ERR_EQU_DEFINITION_LOOP = "EQU definition loop has been detected.";
const string ERR_INVALID_SYM_CLASS_NDX = "Symbol '%s' has invalid classification index.";
const string ERR_OPEN_OUT_FILE_FAILED = "Error opening output file '%s'.";
const string ERR_OUT_FILE_NAME_ALREADY_SET = "Multiple output file name parameters supplied.";
const string ERR_IN_FILE_NAME_ALREADY_SET = "Multiple input files are not supported.";
const string ERR_PARSING = "Parsing error encountered.";

class Assembler_Exception {
    string message;
public:
    Assembler_Exception(string message) : message(message) {}
    friend ostream& operator<<(ostream& os, const Assembler_Exception& error);
};

ostream& operator<<(ostream& os, const Assembler_Exception& error);

string format_string(const string& format, const string& text);

#endif