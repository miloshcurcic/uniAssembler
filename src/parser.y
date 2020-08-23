%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.5"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include <sstream>
  #include <iostream>
  #include <list>
  #include "includes.h"
  #include "instruction.h"
  #include "directive.h"
  #include "asm_exception.h"
  #include "label.h"
  class Driver;

  using namespace std;
}

// The parsing context.
%param { Driver& drv }

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
  SP_REG "%sp"
  PSW_REG "%psw"
  NEW_LINE
  WHITESPACE
;


%token <string> NOOP_INS
%token <string> OOP_INS_JMP
%token <string> OOP_INS_DATA_SRC
%token <string> OOP_INS_DATA_DST
%token <string> TOP_INS_DATA_STD
%token <string> TOP_INS_DATA_NSTD_SWITCH
%token <string> TOP_INS_DATA_NSTD_SRC
%token <string> TOP_INS_DATA_NSTD_DST
%token <string> TOP_BYTE_INS_DATA_STD
%token <string> TOP_BYTE_INS_DATA_NSTD_SWITCH
%token <string> TOP_BYTE_INS_DATA_NSTD_SRC
%token <string> TOP_BYTE_INS_DATA_NSTD_DST

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
%nterm <list<string>*> mixed_list
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
%nterm <Directive*> content_directive

%nterm <LabeledContent*> labeled_content

%%
program:
  sections
  | assembler_directives sections
  | new_lines sections
  | new_lines assembler_directives sections
;

assembler_directives:
  assembler_directive new_lines { DirectiveHandler::handle_directive($1); }
  | assembler_directives assembler_directive new_lines { DirectiveHandler::handle_directive($2); }
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
  SECTION_DIRECTIVE_NAME SYMBOL { list<string> *temp = new list<string>(); temp->push_back($2); DirectiveHandler::handle_directive(DirectiveHandler::prep_directive($1, temp)); }
;

section_code:
  directive new_lines { DirectiveHandler::handle_directive($1); }
  | instruction new_lines { InstructionHandler::handle_instruction($1); }
  | labeled_content new_lines { LabelHandler::handle_labeled_content($1); }
  | section_code directive new_lines { DirectiveHandler::handle_directive($2); }
  | section_code instruction new_lines { InstructionHandler::handle_instruction($2); }
  | section_code labeled_content new_lines { LabelHandler::handle_labeled_content($2); }
;

labeled_content:
  label instruction { $$ = LabelHandler::prep_labeled_content($1, $2); }
  | label content_directive { $$ = LabelHandler::prep_labeled_content($1, $2); }
  | label new_lines instruction { $$ = LabelHandler::prep_labeled_content($1, $3); }
  | label new_lines content_directive { $$ = LabelHandler::prep_labeled_content($1, $3); }
;

directive: 
  alloc_directive { $$ = $1; }
  | skip_directive { $$ = $1; }
  | symbol_directive { $$ = $1; }
  | equ_directive { $$ = $1; }
;

content_directive:
  alloc_directive { $$ = $1; }
  | skip_directive { $$ = $1; }
;

new_lines:
  NEW_LINE
  | new_lines NEW_LINE
;

alloc_directive:
  ALLOC_DIRECTIVE_NAME mixed_list  { $$ = DirectiveHandler::prep_directive($1, $2); }
;

skip_directive:
  SKIP_DIRECTIVE_NAME literal  { list<string> *temp = new list<string>(); temp->push_back($2); $$ = DirectiveHandler::prep_directive($1, temp); }
;

equ_directive:
  EQU_DIRECTIVE_NAME SYMBOL "," expression  { $4->push_front($2); $$ = DirectiveHandler::prep_directive($1, $4); }
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
  SYMBOL_DIRECTIVE_NAME symbol_list  { $$ = DirectiveHandler::prep_directive($1, $2); }
;

instruction:
  NOOP_INS { $$ = InstructionHandler::prep_ins($1); }
  | OOP_INS_JMP dst_op_jmp { $$ = InstructionHandler::prep_ins($1, $2); }
  | OOP_INS_DATA_SRC src_op_data_word { $$ = InstructionHandler::prep_ins($1, $2); }
  | OOP_INS_DATA_DST dst_op_data_word { $$ = InstructionHandler::prep_ins($1, $2); }
  | TOP_INS_DATA_STD src_op_data_word "," dst_op_data_word { $$ = InstructionHandler::prep_ins($1, $2, $4); }
  | TOP_INS_DATA_NSTD_SWITCH dst_op_data_word "," src_op_data_word { $$ = InstructionHandler::prep_ins($1, $2, $4);}
  | TOP_INS_DATA_NSTD_SRC src_op_data_word "," src_op_data_word { $$ = InstructionHandler::prep_ins($1, $2, $4);}
  | TOP_INS_DATA_NSTD_DST dst_op_data_word "," dst_op_data_word { $$ = InstructionHandler::prep_ins($1, $2, $4);}
  | TOP_BYTE_INS_DATA_STD src_op_data_byte "," dst_op_data_byte { $$ = InstructionHandler::prep_ins($1, $2, $4); }
  | TOP_BYTE_INS_DATA_NSTD_SWITCH dst_op_data_byte "," src_op_data_byte { $$ = InstructionHandler::prep_ins($1, $2, $4); }
  | TOP_BYTE_INS_DATA_NSTD_SRC src_op_data_byte "," src_op_data_byte { $$ = InstructionHandler::prep_ins($1, $2, $4); }
  | TOP_BYTE_INS_DATA_NSTD_DST dst_op_data_byte "," dst_op_data_byte { $$ = InstructionHandler::prep_ins($1, $2, $4); }
;

src_op_data_byte:
  "$" literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_IMMED, $2, true); }
  | "$" SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_IMMED, $2, false); }
  | BYTE_REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, $1.first, $1.second); }
  | reg_ind { $$ = $1; }
  | literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $1, false); }
;

src_op_data_word:
  "$" literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_IMMED, $2, true); }
  | "$" SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_IMMED, $2, false); }
  | REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, $1); }
  | "%pc" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, Register::R_7); }
  | "%sp" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, Register::R_6); }
  | "%psw" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, Register::R_PSW); }
  | reg_ind { $$ = $1; }
  | literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $1, false); }
;

dst_op_data_byte:
  BYTE_REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, $1.first, $1.second); }
  | reg_ind { $$ = $1; }
  | literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $1, false); }
;

dst_op_data_word:
  REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, $1); }
  | "%pc" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, Register::R_7); }
  | "%sp" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, Register::R_6); }
  | "%psw" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, Register::R_PSW); }
  | reg_ind { $$ = $1; }
  | literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $1, false); }
;

dst_op_jmp:
  literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_IMMED, $1, true); }
  | SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_IMMED, $1, false); }
  | "*" REGISTER { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGDIR, "", false, $2); }
  | "*" reg_ind { $$ = $2; }
  | "*" literal { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $2, true); }
  | "*" SYMBOL { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_MEMDIR, $2, false); }
;

reg_ind:
  "(" REGISTER ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGIND, "", false, $2); }
  | "(" "%pc" ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGIND, "", false, Register::R_6); }
  | "(" "%sp" ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_REGIND, "", false, Register::R_6); }
  | literal "(" REGISTER ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_BASEREG, $1, true, $3); }
  | SYMBOL "(" REGISTER ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_BASEREG, $1, false, $3);  }
  | SYMBOL "(" "%pc" ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_BASEREG, $1, false, Register::R_7);  }
  | SYMBOL "(" "%sp" ")" { $$ = InstructionHandler::prep_ins_op(AddressingMode::AM_BASEREG, $1, false, Register::R_6); }
;

mixed_list:
  SYMBOL { $$ = new list<string>(); $$->push_back($1); }
  | literal { $$ = new list<string>(); $$->push_back($1); }
  | mixed_list "," SYMBOL { $$ = $1; $$->push_back($3); }
  | mixed_list "," literal { $$ = $1; $$->push_back($3); }
;

symbol_list:
  SYMBOL { $$ = new list<string>(); $$->push_back($1); }
  | symbol_list "," SYMBOL { $$ = $1; $$->push_back($3); }
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
yy::parser::error (const location_type& loc, const std::string& msg)
{
  location::counter_type end_col = 0 < loc.end.column ? loc.end.column - 1 : 0;
  stringstream loc_msg;

  loc_msg << loc.begin;
  
  if (loc.end.filename
      && (!loc.begin.filename
          || *loc.begin.filename != *loc.end.filename)) {
    loc_msg << '-' << loc.end.filename << ':' << loc.end.line << '.' << end_col;
  } else if (loc.begin.line < loc.end.line)
    loc_msg << '-' << loc.end.line << '.' << end_col;
  else if (loc.begin.column < end_col)
    loc_msg << '-' << end_col;
  
  string message = format_string(ERR_PARSING, loc_msg.str(), msg);
  throw AssemblerException(message);
}