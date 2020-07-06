%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstring> // strerror
# include <string>
# include "driver.h"
# include "parser.h"
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

[ \t\r]+   loc.step ();
\n        loc.lines(1); loc.step (); return yy::parser::make_NEW_LINE(loc);

"(" return yy::parser::make_LPAREN(loc);
")" return yy::parser::make_RPAREN(loc);
"," return yy::parser::make_COMMA(loc);
"$" return yy::parser::make_DOLLAR(loc);
"*" return yy::parser::make_STAR(loc);
":" return yy::parser::make_COLON(loc);
"+" return yy::parser::make_PLUS(loc);
"-" return yy::parser::make_MINUS(loc);

"b" return yy::parser::make_BYTE_INS(loc);
"w" return yy::parser::make_WORD_INS(loc);
"l" return yy::parser::make_LOW_REG(loc);
"h" return yy::parser::make_HIGH_REG(loc);

"%pc" return yy::parser::make_PC_REG(loc);

"%"r[0-7] return yy::parser::make_REGISTER(yytext, loc);
\'.\' return yy::parser::make_CHAR_LITERAL(yytext, loc);

("halt"|"iret"|"ret") return yy::parser::make_NOOP_INS(yytext, loc);
("call"|"jmp"|"jeq"|"jne"|"jgt") return yy::parser::make_OOP_INS_JMP(yytext, loc);

("int"|"pop") return yy::parser::make_OOP_INS_DATA_DST(yytext, loc);
("push") return yy::parser::make_OOP_INS_DATA_SRC(yytext, loc);

("xchg"|"mov"|"add"|"sub"|"mul"|"div"|"cmp"|"not"|"and"|"or"|"xor"|"test"|"shl")/([wb ]{0,1}) return yy::parser::make_TOP_INS_DATA_STD(yytext, loc);
("shr")/([wb ]{0,1}) return yy::parser::make_TOP_INS_DATA_NSTD(yytext, loc);

("\.byte"|"\.word") return yy::parser::make_ALLOC_DIRECTIVE_NAME(yytext, loc);
("\.global"|"\.extern") return yy::parser::make_SYMBOL_DIRECTIVE_NAME(yytext, loc);
"\.section "[a-z]+":" std::cout << "Current section is: " << yytext << std::endl; return yy::parser::make_SECTION_DIRECTIVE(yytext, loc);
"\.skip" return yy::p)arser::make_SKIP_DIRECTIVE_NAME(loc);
"\.equ" return yy::parser::make_EQU_DIRECTIVE_NAME(loc);
"\.end" return yy::parser::make_END(loc);

([1-9]{1}[0-9]*|"0"[0-7]+|"0"[xX]{1}[0-9abcdefABCDEF]+|"0"[bB]{1}[0-1]+) return yy::parser::make_NUM_LITERAL(yytext, loc);

([a-zA-Z_.$]{1}[a-zA-Z0-9_.$]*) return yy::parser::make_SYMBOL(yytext, loc);

<<EOF>>    return yy::parser::make_END (loc);
%%

void
driver::scan_begin ()
{
  yy_flex_debug = trace_scanning;
  if (file.empty () || file == "-")
    yyin = stdin;
  else if (!(yyin = fopen (file.c_str (), "r")))
    {
      std::cerr << "cannot open " << file << ": " << strerror(errno) << '\n';
      exit (EXIT_FAILURE);
    }
}

void
driver::scan_end ()
{
  fclose (yyin);
}