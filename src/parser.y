%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.5"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include <iostream>
  class driver;
}

// The parsing context.
%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
  #include "driver.h"
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
  BYTE_INS "b"
  WORD_INS "w"
  LOW_REG "l"
  HIGH_REG "h"
  NEW_LINE
;


%token <std::string> NOOP_INS
%token <std::string> OOP_INS_SRC
%token <std::string> OOP_INS_DST
%token <std::string> TOP_INS_DATA_STD
%token <std::string> TOP_INS_DATA_NSTD
%token <std::string> SYMBOL
%token <std::string> NUM_LITERAL
%token <std::string> CHAR_LITERAL
%token <std::string> REGISTER

%token <std::string> ALLOC_DIRECTIVE_NAME
%token <std::string> SYMBOL_DIRECTIVE_NAME
// ToDo: check names:
%token <std::string> SECTION_DIRECTIVE
%token SKIP_DIRECTIVE_NAME ".skip"
%token EQU_DIRECTIVE_NAME ".equ"

%nterm <std::string> src_op_data_word
%nterm <std::string> src_op_data_byte
%nterm <std::string> dst_op_data_word
%nterm <std::string> dst_op_data_byte
%nterm <std::string> dst_op_jmp
%nterm <std::string> reg_ind
%nterm <std::string> reg_byte
%nterm <std::string> label
%nterm <std::string> literal

%%
program: sections { std::cout << "aaa";};

sections:
  %empty
  | sections section

section:
  SECTION_DIRECTIVE NEW_LINE code;

code:
  %empty
  | label code
  | directive code
  | label NEW_LINE code
  | directive NEW_LINE code
  | instruction NEW_LINE code;

directive:
  alloc_directive
  | skip_directive
  | equ_directive
  | symbol_directive;

alloc_directive:
  ALLOC_DIRECTIVE_NAME symbol_list
  | ALLOC_DIRECTIVE_NAME literal_list;

skip_directive:
  ".skip" literal;

equ_directive:
  ".equ" SYMBOL "," exp 

exp:
  SYMBOL
  | literal
  | SYMBOL "+" exp
  | literal "+" exp
  | SYMBOL "-" exp
  | literal "-" exp

symbol_directive:
  SYMBOL_DIRECTIVE_NAME symbol_list;

labeled_operation:
  label instruction { std::cout << "bison found a labeled operation:" << $1 << std::endl; };

instruction:
  NOOP_INS { std::cout << "bison found a NOOP_INS:" << $1 << std::endl; }
  | OOP_INS_JMP dst_op_jmp { std::cout << "bison found a OOP_INS_JMP:" << $1 << " " << $2 << std::endl; }
  | OOP_INS_DATA_SRC src_op_data_word { std::cout << "bison found a 2B OOP_INS_DATA_SRC:" << $1 << " " << $2 << std::endl; }
  | OOP_INS_DATA_DST dst_op_data_word { std::cout << "bison found a 2B OOP_INS_DATA_DST:" << $1 << " " << $2 << std::endl; }
  | TOP_INS_DATA_STD src_op_data_word "," dst_op_data_word { std::cout << "bison found a 2B TOP_INS_DATA_STD:" << $1 << " " << $2 << ", " << $4 << std::endl; }
  | TOP_INS_DATA_STD"w" src_op_data_word "," dst_op_data_word { std::cout << "bison found a 2B TOP_INS_DATA_STD:" << $1 << " " << $3 << ", " << $5 << std::endl; }
  | TOP_INS_DATA_STD"b" src_op_data_byte "," dst_op_data_byte { std::cout << "bison found a 1B TOP_INS_DATA_STD:" << $1 << " " << $3 << ", " << $5 << std::endl; }
  | TOP_INS_DATA_NSTD dst_op_data_word "," src_op_data_word { std::cout << "bison found a 2B TOP_INS_DATA_NSTD:" << $1 << " " << $2 << ", " << $4 << std::endl; }
  | TOP_INS_DATA_NSTD"w" dst_op_data_word "," src_op_data_word { std::cout << "bison found a 2B TOP_INS_DATA_NSTD:" << $1 << " " << $3 << ", " << $5 << std::endl; }
  | TOP_INS_DATA_NSTD"b" dst_op_data_byte "," src_op_data_byte { std::cout << "bison found a 1B TOP_INS_DATA_NSTD:" << $1 << " " << $3 << ", " << $5 << std::endl; };

src_op_data_word:
  "$" literal { $$ = "$" + $2; }
  | "$" SYMBOL { $$ = "$" + $2; }
  | REGISTER
  | reg_ind
  | literal
  | SYMBOL;

dst_op_data_word:
  REGISTER
  | reg_ind
  | literal
  | SYMBOL;

src_op_data_byte:
  "$" literal { $$ = "$" + $2; }
  | "$" SYMBOL { $$ = "$" + $2; }
  | reg_byte
  | reg_ind
  | literal
  | SYMBOL;

dst_op_data_byte:
  reg_byte
  | reg_ind
  | literal
  | SYMBOL;

dst_op_jmp:
  literal
  | SYMBOL
  | "*" REGISTER { $$ = "*" + $2; }
  | "*" reg_ind { $$ = "*" + $2; }
  | "*" literal { $$ = "*" + $2; }
  | "*" SYMBOL { $$ = "*" + $2; };

reg_ind:
  "(" REGISTER ")" { $$ = "(" + $2 + ")"; }
  | literal "(" REGISTER ")" { $$ = $1 + "(" + $3 + ")"; }
  | SYMBOL "(" REGISTER ")" { $$ = $1 + "(" + $3 + ")"; }
  | SYMBOL "(" "%pc" ")" { $$ = $1 + "(%r7)"; };

reg_byte:
  REGISTER "l" { $$ = $1 + "l"; }
  | REGISTER "h" { $$ = $1 + "h"; };

symbol_list:
  SYMBOL
  | SYMBOL "," symbol_list;

literal_list:
  literal
  | literal "," literal_list;

label:
 SYMBOL ":" { $$ = $1 + ":"; };

literal:
  NUM_LITERAL
  | CHAR_LITERAL;
%%

void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << "\n" << l << "\n" << m << '\n';
}