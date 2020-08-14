%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstring> // strerror
# include <string>
# include "driver.h"
# include "parser.h"
# include "instruction.h"
# include "assembler.h"

using namespace std;
%}

%option noyywrap nounput noinput batch debug

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}
%%
%{
  // A handy shortcut to the location held by the driver.
  yy::location& loc = drv.location;
  // Code run each time yylex is called.
  loc.step ();
%}

[ \t]   { loc.step (); }
\n    { loc.lines(1); loc.step (); return yy::parser::make_NEW_LINE(loc); }

"(" { return yy::parser::make_LPAREN(loc); }
")" { return yy::parser::make_RPAREN(loc); }
"," { return yy::parser::make_COMMA(loc); }
"$" { return yy::parser::make_DOLLAR(loc); }
"*" { return yy::parser::make_STAR(loc); }
":" { return yy::parser::make_COLON(loc); }
"+" { return yy::parser::make_PLUS(loc); }
"-" { return yy::parser::make_MINUS(loc); }

"%pc" { return yy::parser::make_PC_REG(loc); }

%r[0-7] { return yy::parser::make_REGISTER((Register)(yytext[2] - '0'), loc); }
%r[0-7][lh] { return yy::parser::make_BYTE_REGISTER(pair<Register, RegisterByte>((Register)(yytext[2] - '0'), (RegisterByte)(yytext[3] == 'h')), loc); }

\'.\' { return yy::parser::make_CHAR_LITERAL(yytext, loc); }

("halt"|"iret"|"ret") { return yy::parser::make_NOOP_INS(yytext, loc); }
("call"|"jmp"|"jeq"|"jne"|"jgt") { return yy::parser::make_OOP_INS_JMP(yytext, loc); }
("int"|"pop") { return yy::parser::make_OOP_INS_DATA_DST(yytext, loc); }
("push") { return yy::parser::make_OOP_INS_DATA_SRC(yytext, loc); }
("mov"|"add"|"sub"|"mul"|"div"|"not"|"and"|"or"|"xor"|"shl")([w]?) { return yy::parser::make_TOP_INS_DATA_STD(yytext, loc); }
("cmp"|"test")([w]?) { return yy::parser::make_TOP_INS_DATA_NSTD_SRC(yytext, loc); }
("xchg")([w]?) { return yy::parser::make_TOP_INS_DATA_NSTD_DST(yytext, loc); }
("shr")([w]?) { return yy::parser::make_TOP_INS_DATA_NSTD_SWITCH(yytext, loc); }
("mov"|"add"|"sub"|"mul"|"div"|"not"|"and"|"or"|"xor"|"shl")("b") { return yy::parser::make_TOP_BYTE_INS_DATA_STD(yytext, loc); }
("cmp"|"test")("b") { return yy::parser::make_TOP_BYTE_INS_DATA_NSTD_SRC(yytext, loc); }
("xchg")("b") { return yy::parser::make_TOP_BYTE_INS_DATA_NSTD_DST(yytext, loc); }
("shr")("b") { return yy::parser::make_TOP_BYTE_INS_DATA_NSTD_SWITCH(yytext, loc); }

("\.byte"|"\.word") { return yy::parser::make_ALLOC_DIRECTIVE_NAME(yytext, loc); }
("\.global"|"\.extern") { return yy::parser::make_SYMBOL_DIRECTIVE_NAME(yytext, loc); }
"\.section" { return yy::parser::make_SECTION_DIRECTIVE_NAME(yytext, loc); }
"\.skip" { return yy::parser::make_SKIP_DIRECTIVE_NAME(yytext, loc); }
"\.equ" { return yy::parser::make_EQU_DIRECTIVE_NAME(yytext, loc); }
"\.end" { return yy::parser::make_END(loc); }

([1-9]{1}[0-9]*|"0"[0-7]+|"0"[xX]{1}[0-9abcdefABCDEF]+|"0"[bB]{1}[0-1]+|"0") { return yy::parser::make_NUM_LITERAL(yytext, loc); }
([a-zA-Z_.]{1}[a-zA-Z0-9_.$]*) { return yy::parser::make_SYMBOL(yytext, loc); }

<<EOF>> { return yy::parser::make_END(loc); }
%%

void
Driver::scan_begin ()
{
  yy_flex_debug = 0;
  if (!(yyin = fopen (file.c_str (), "r")))
    {
      std::cerr << "cannot open " << file << ": " << strerror(errno) << '\n';
      exit (EXIT_FAILURE);
    }
}

void
Driver::scan_end ()
{
  fclose (yyin);
}