%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.5"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include <iostream>
  #include <list>
  #include "types.h"
  #include "instruction.h"
  #include "directive.h"
  class driver;

  using namespace std;
}

// The parsing context.
%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
  #include "driver.h"
  #include "utility.h"
}

%define api.token.prefix {TOK_}
%token
  END 0 "end"
  LPAREN "("
  RPAREN ")"
  DOLLAR "$"
  COMMA ","
  STAR "*"
  COLON ":"
  PLUS "+"
  MINUS "-"
  PC_REG "%pc"
  NEW_LINE
  WHITESPACE
;


%token <string> NOOP_INS
%token <string> OOP_INS_JMP
%token <string> OOP_INS_DATA_SRC
%token <string> OOP_INS_DATA_DST
%token <string> TOP_INS_DATA_STD
%token <string> TOP_INS_DATA_NSTD
%token <string> TOP_BYTE_INS_DATA_STD
%token <string> TOP_BYTE_INS_DATA_NSTD

%token <string> SYMBOL
%token <string> NUM_LITERAL
%token <string> CHAR_LITERAL
%token <Register> REGISTER
%token <pair<Register, RegisterByte>> BYTE_REGISTER

%token <string> ALLOC_DIRECTIVE_NAME
%token <string> SYMBOL_DIRECTIVE_NAME
%token <string> SECTION_DIRECTIVE
%token <string> EQU_DIRECTIVE_NAME
%token <string> SKIP_DIRECTIVE_NAME
%token <string> SECTION_DIRECTIVE_NAME

%nterm <string> literal
%nterm <string> label

%nterm <list<string>*> symbol_list
%nterm <list<string>*> literal_list
%nterm <list<string>*> expression

%nterm <InstructionOperand*> src_op_data_byte
%nterm <InstructionOperand*> dst_op_data_byte
%nterm <InstructionOperand*> src_op_data_word
%nterm <InstructionOperand*> dst_op_data_word
%nterm <InstructionOperand*> dst_op_jmp
%nterm <InstructionOperand*> reg_ind
%nterm <Instruction*> instruction

%nterm <Directive*> alloc_directive
%nterm <Directive*> equ_directive
%nterm <Directive*> skip_directive
%nterm <Directive*> symbol_directive
%nterm <Directive*> directive
%nterm <Directive*> assembler_directive

%%
program:
  sections
  | assembler_directives sections
  | new_lines sections
  | new_lines assembler_directives sections
;

assembler_directives:
  assembler_directive new_lines { cout << "assembler directive" << endl; }
  | assembler_directives assembler_directive new_lines { cout << "assembler directive" << endl; }
;

assembler_directive:
  equ_directive { $$ = $1; }
  | symbol_directive { $$ = $1; }
;

sections:
  section
  | sections section
;

section:
  change_section_directive new_lines section_code
;

change_section_directive:
  SECTION_DIRECTIVE_NAME SYMBOL { cout << "changing section" << endl; }
;

section_code:
  directive new_lines { cout << "directive" << endl; }
  | instruction new_lines { cout << "instruction" << endl; }
  | labeled_content new_lines { cout << "labeled_content" << endl; }
  | section_code directive new_lines { cout << "directive" << endl; }
  | section_code instruction new_lines { cout << "instruction" << endl; }
  | section_code labeled_content new_lines { cout << "labeled_content" << endl; }
;

labeled_content:
  label instruction
  | label content_directive
  | label new_lines instruction
  | label new_lines content_directive
;

directive: 
  alloc_directive { $$ = $1; }
  | skip_directive { $$ = $1; }
  | symbol_directive { $$ = $1; }
  | equ_directive { $$ = $1; }
;

content_directive:
  alloc_directive
  | skip_directive
;

new_lines:
  NEW_LINE
  | new_lines NEW_LINE
;

alloc_directive:
  ALLOC_DIRECTIVE_NAME symbol_list  { $$ = new Directive($1, $2); }
  | ALLOC_DIRECTIVE_NAME literal_list { $$ = new Directive($1, $2); }
;

skip_directive:
  SKIP_DIRECTIVE_NAME literal  { list<string> *temp = new list<string>(); temp->push_back($2); $$ = new Directive($1, temp); }
;

equ_directive:
  EQU_DIRECTIVE_NAME SYMBOL "," expression  { $4->push_front($2); $$ = new Directive($1, $4); }
; 

expression:
  SYMBOL { $$ = new list<string>(); $$->push_back("+" + $1); }
  | literal { $$ = new list<string>(); $$->push_back("+" + $1); }
  | expression "+" SYMBOL { $$ = $1; $$->push_back("+" + $3); }
  | expression "+" literal { $$ = $1; $$->push_back("+" + $3); }
  | expression "-" SYMBOL { $$ = $1; $$->push_back("-" + $3); }
  | expression "-" literal { $$ = $1; $$->push_back("-" + $3); }
;

symbol_directive:
  SYMBOL_DIRECTIVE_NAME symbol_list  { $$ = new Directive($1, $2); }
;

instruction:
  NOOP_INS { $$ = InstructionHandler::prep_ins($1); }
  | OOP_INS_JMP dst_op_jmp { $$ = InstructionHandler::prep_ins($1, $2); }
  | OOP_INS_DATA_SRC src_op_data_word { $$ = InstructionHandler::prep_ins($1, $2); }
  | OOP_INS_DATA_DST dst_op_data_word { $$ = InstructionHandler::prep_ins($1, $2); }
  | TOP_INS_DATA_STD src_op_data_word "," dst_op_data_word { $$ = InstructionHandler::prep_ins($1, $2); }
  | TOP_BYTE_INS_DATA_STD src_op_data_byte "," dst_op_data_byte { $$ = InstructionHandler::prep_ins($1, $2, $4); }
  | TOP_INS_DATA_NSTD dst_op_data_word "," src_op_data_word { $$ = InstructionHandler::prep_ins($1, $2, $4);}
  | TOP_BYTE_INS_DATA_NSTD dst_op_data_byte "," src_op_data_byte { $$ = InstructionHandler::prep_ins($1, $2, $4); }
;

src_op_data_byte:
  "$" literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::IMMED, $2, true); }
  | "$" SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::IMMED, $2, false); }
  | BYTE_REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::REGDIR, "", false, $1.first, $1.second); }
  | reg_ind { $$ = $1; }
  | literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $1, false); }
;

src_op_data_word:
  "$" literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::IMMED, $2, true); }
  | "$" SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::IMMED, $2, false); }
  | REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::REGDIR, "", false, $1); }
  | reg_ind { $$ = $1; }
  | literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $1, true); }
;

dst_op_data_byte:
  BYTE_REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::REGDIR, "", false, $1.first, $1.second); }
  | reg_ind { $$ = $1; }
  | literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $1, false); }
;

dst_op_data_word:
  REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::REGDIR, "", false, $1); }
  | reg_ind { $$ = $1; }
  | literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $1, false); }
;

dst_op_jmp:
  literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::IMMED, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::IMMED, $1, false); }
  | "*" REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::REGDIR, "", false, $2); }
  | "*" reg_ind { $$ = $2; }
  | "*" literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $2, true); }
  | "*" SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::MEMDIR, $2, false); }
;

reg_ind:
  "(" REGISTER ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::REGIND, "", false, $2); }
  | literal "(" REGISTER ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::REGIND, $1, true, $3); }
  | SYMBOL "(" REGISTER ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::REGIND, $1, false, $3);  }
  | SYMBOL "(" "%pc" ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::REGIND, $1, false, Register::R7);  }
;

symbol_list:
  SYMBOL { $$ = new list<string>(); $$->push_back($1); }
  | symbol_list "," SYMBOL { $$ = $1; $$->push_back($3); }
;

literal_list:
  literal { $$ = new list<string>(); $$->push_back($1); }
  | literal_list "," literal { $$ = $1; $$->push_back($3); }
;

label:
 SYMBOL ":" { $$ = $1; }
;

literal:
  NUM_LITERAL { $$ = $1; }
  | CHAR_LITERAL { $$ = $1; }
;
%%

void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << "\n" << l << "\n" << m << '\n';
}