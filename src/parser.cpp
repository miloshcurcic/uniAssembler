// A Bison parser, made by GNU Bison 3.6.1.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parser.h"


// Unqualified %code blocks.
#line 23 "src/parser.y"

  #include "driver.h"

#line 50 "src/parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 142 "src/parser.cpp"

  /// Build a parser object.
  parser::parser (driver& drv_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      drv (drv_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case 17: // NOOP_INS
      case 18: // OOP_INS_JMP
      case 19: // OOP_INS_DATA_SRC
      case 20: // OOP_INS_DATA_DST
      case 21: // TOP_INS_DATA_STD
      case 22: // TOP_INS_DATA_NSTD
      case 23: // SYMBOL
      case 24: // NUM_LITERAL
      case 25: // CHAR_LITERAL
      case 26: // REGISTER
      case 27: // ALLOC_DIRECTIVE_NAME
      case 28: // SYMBOL_DIRECTIVE_NAME
      case 29: // SECTION_DIRECTIVE
      case 33: // src_op_data_word
      case 34: // src_op_data_byte
      case 35: // dst_op_data_word
      case 36: // dst_op_data_byte
      case 37: // dst_op_jmp
      case 38: // reg_ind
      case 39: // reg_byte
      case 40: // label
      case 41: // literal
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case 17: // NOOP_INS
      case 18: // OOP_INS_JMP
      case 19: // OOP_INS_DATA_SRC
      case 20: // OOP_INS_DATA_DST
      case 21: // TOP_INS_DATA_STD
      case 22: // TOP_INS_DATA_NSTD
      case 23: // SYMBOL
      case 24: // NUM_LITERAL
      case 25: // CHAR_LITERAL
      case 26: // REGISTER
      case 27: // ALLOC_DIRECTIVE_NAME
      case 28: // SYMBOL_DIRECTIVE_NAME
      case 29: // SECTION_DIRECTIVE
      case 33: // src_op_data_word
      case 34: // src_op_data_byte
      case 35: // dst_op_data_word
      case 36: // dst_op_data_byte
      case 37: // dst_op_jmp
      case 38: // reg_ind
      case 39: // reg_byte
      case 40: // label
      case 41: // literal
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case 17: // NOOP_INS
      case 18: // OOP_INS_JMP
      case 19: // OOP_INS_DATA_SRC
      case 20: // OOP_INS_DATA_DST
      case 21: // TOP_INS_DATA_STD
      case 22: // TOP_INS_DATA_NSTD
      case 23: // SYMBOL
      case 24: // NUM_LITERAL
      case 25: // CHAR_LITERAL
      case 26: // REGISTER
      case 27: // ALLOC_DIRECTIVE_NAME
      case 28: // SYMBOL_DIRECTIVE_NAME
      case 29: // SECTION_DIRECTIVE
      case 33: // src_op_data_word
      case 34: // src_op_data_byte
      case 35: // dst_op_data_word
      case 36: // dst_op_data_byte
      case 37: // dst_op_jmp
      case 38: // reg_ind
      case 39: // reg_byte
      case 40: // label
      case 41: // literal
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case 17: // NOOP_INS
      case 18: // OOP_INS_JMP
      case 19: // OOP_INS_DATA_SRC
      case 20: // OOP_INS_DATA_DST
      case 21: // TOP_INS_DATA_STD
      case 22: // TOP_INS_DATA_NSTD
      case 23: // SYMBOL
      case 24: // NUM_LITERAL
      case 25: // CHAR_LITERAL
      case 26: // REGISTER
      case 27: // ALLOC_DIRECTIVE_NAME
      case 28: // SYMBOL_DIRECTIVE_NAME
      case 29: // SECTION_DIRECTIVE
      case 33: // src_op_data_word
      case 34: // src_op_data_byte
      case 35: // dst_op_data_word
      case 36: // dst_op_data_byte
      case 37: // dst_op_jmp
      case 38: // reg_ind
      case 39: // reg_byte
      case 40: // label
      case 41: // literal
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YYUSE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (drv));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case 17: // NOOP_INS
      case 18: // OOP_INS_JMP
      case 19: // OOP_INS_DATA_SRC
      case 20: // OOP_INS_DATA_DST
      case 21: // TOP_INS_DATA_STD
      case 22: // TOP_INS_DATA_NSTD
      case 23: // SYMBOL
      case 24: // NUM_LITERAL
      case 25: // CHAR_LITERAL
      case 26: // REGISTER
      case 27: // ALLOC_DIRECTIVE_NAME
      case 28: // SYMBOL_DIRECTIVE_NAME
      case 29: // SECTION_DIRECTIVE
      case 33: // src_op_data_word
      case 34: // src_op_data_byte
      case 35: // dst_op_data_word
      case 36: // dst_op_data_byte
      case 37: // dst_op_jmp
      case 38: // reg_ind
      case 39: // reg_byte
      case 40: // label
      case 41: // literal
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2:
#line 76 "src/parser.y"
                  { std::cout << "aaa";}
#line 661 "src/parser.cpp"
    break;

  case 28:
#line 124 "src/parser.y"
                  { std::cout << "bison found a labeled operation:" << yystack_[1].value.as < std::string > () << std::endl; }
#line 667 "src/parser.cpp"
    break;

  case 29:
#line 127 "src/parser.y"
           { std::cout << "bison found a NOOP_INS:" << yystack_[0].value.as < std::string > () << std::endl; }
#line 673 "src/parser.cpp"
    break;

  case 30:
#line 128 "src/parser.y"
                           { std::cout << "bison found a OOP_INS_JMP:" << yystack_[1].value.as < std::string > () << " " << yystack_[0].value.as < std::string > () << std::endl; }
#line 679 "src/parser.cpp"
    break;

  case 31:
#line 129 "src/parser.y"
                                      { std::cout << "bison found a 2B OOP_INS_DATA_SRC:" << yystack_[1].value.as < std::string > () << " " << yystack_[0].value.as < std::string > () << std::endl; }
#line 685 "src/parser.cpp"
    break;

  case 32:
#line 130 "src/parser.y"
                                         { std::cout << "bison found a 2B OOP_INS_DATA_SRC:" << yystack_[2].value.as < std::string > () << " " << yystack_[0].value.as < std::string > () << std::endl; }
#line 691 "src/parser.cpp"
    break;

  case 33:
#line 131 "src/parser.y"
                                         { std::cout << "bison found a 1B OOP_INS_DATA_SRC:" << yystack_[2].value.as < std::string > () << " " << yystack_[0].value.as < std::string > () << std::endl; }
#line 697 "src/parser.cpp"
    break;

  case 34:
#line 132 "src/parser.y"
                                      { std::cout << "bison found a 2B OOP_INS_DATA_DST:" << yystack_[1].value.as < std::string > () << " " << yystack_[0].value.as < std::string > () << std::endl; }
#line 703 "src/parser.cpp"
    break;

  case 35:
#line 133 "src/parser.y"
                                         { std::cout << "bison found a 2B OOP_INS_DATA_DST:" << yystack_[2].value.as < std::string > () << " " << yystack_[0].value.as < std::string > () << std::endl; }
#line 709 "src/parser.cpp"
    break;

  case 36:
#line 134 "src/parser.y"
                                         { std::cout << "bison found a 1B OOP_INS_DATA_DST:" << yystack_[2].value.as < std::string > () << " " << yystack_[0].value.as < std::string > () << std::endl; }
#line 715 "src/parser.cpp"
    break;

  case 37:
#line 135 "src/parser.y"
                                                           { std::cout << "bison found a 2B TOP_INS_DATA_STD:" << yystack_[3].value.as < std::string > () << " " << yystack_[2].value.as < std::string > () << ", " << yystack_[0].value.as < std::string > () << std::endl; }
#line 721 "src/parser.cpp"
    break;

  case 38:
#line 136 "src/parser.y"
                                                              { std::cout << "bison found a 2B TOP_INS_DATA_STD:" << yystack_[4].value.as < std::string > () << " " << yystack_[2].value.as < std::string > () << ", " << yystack_[0].value.as < std::string > () << std::endl; }
#line 727 "src/parser.cpp"
    break;

  case 39:
#line 137 "src/parser.y"
                                                              { std::cout << "bison found a 1B TOP_INS_DATA_STD:" << yystack_[4].value.as < std::string > () << " " << yystack_[2].value.as < std::string > () << ", " << yystack_[0].value.as < std::string > () << std::endl; }
#line 733 "src/parser.cpp"
    break;

  case 40:
#line 138 "src/parser.y"
                                                            { std::cout << "bison found a 2B TOP_INS_DATA_NSTD:" << yystack_[3].value.as < std::string > () << " " << yystack_[2].value.as < std::string > () << ", " << yystack_[0].value.as < std::string > () << std::endl; }
#line 739 "src/parser.cpp"
    break;

  case 41:
#line 139 "src/parser.y"
                                                               { std::cout << "bison found a 2B TOP_INS_DATA_NSTD:" << yystack_[4].value.as < std::string > () << " " << yystack_[2].value.as < std::string > () << ", " << yystack_[0].value.as < std::string > () << std::endl; }
#line 745 "src/parser.cpp"
    break;

  case 42:
#line 140 "src/parser.y"
                                                               { std::cout << "bison found a 1B TOP_INS_DATA_NSTD:" << yystack_[4].value.as < std::string > () << " " << yystack_[2].value.as < std::string > () << ", " << yystack_[0].value.as < std::string > () << std::endl; }
#line 751 "src/parser.cpp"
    break;

  case 43:
#line 143 "src/parser.y"
              { yylhs.value.as < std::string > () = "$" + yystack_[0].value.as < std::string > (); }
#line 757 "src/parser.cpp"
    break;

  case 44:
#line 144 "src/parser.y"
               { yylhs.value.as < std::string > () = "$" + yystack_[0].value.as < std::string > (); }
#line 763 "src/parser.cpp"
    break;

  case 45:
#line 145 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 769 "src/parser.cpp"
    break;

  case 46:
#line 146 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 775 "src/parser.cpp"
    break;

  case 47:
#line 147 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 781 "src/parser.cpp"
    break;

  case 48:
#line 148 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 787 "src/parser.cpp"
    break;

  case 49:
#line 151 "src/parser.y"
  { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 793 "src/parser.cpp"
    break;

  case 50:
#line 152 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 799 "src/parser.cpp"
    break;

  case 51:
#line 153 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 805 "src/parser.cpp"
    break;

  case 52:
#line 154 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 811 "src/parser.cpp"
    break;

  case 53:
#line 157 "src/parser.y"
              { yylhs.value.as < std::string > () = "$" + yystack_[0].value.as < std::string > (); }
#line 817 "src/parser.cpp"
    break;

  case 54:
#line 158 "src/parser.y"
               { yylhs.value.as < std::string > () = "$" + yystack_[0].value.as < std::string > (); }
#line 823 "src/parser.cpp"
    break;

  case 55:
#line 159 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 829 "src/parser.cpp"
    break;

  case 56:
#line 160 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 835 "src/parser.cpp"
    break;

  case 57:
#line 161 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 841 "src/parser.cpp"
    break;

  case 58:
#line 162 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 847 "src/parser.cpp"
    break;

  case 59:
#line 165 "src/parser.y"
  { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 853 "src/parser.cpp"
    break;

  case 60:
#line 166 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 859 "src/parser.cpp"
    break;

  case 61:
#line 167 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 865 "src/parser.cpp"
    break;

  case 62:
#line 168 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 871 "src/parser.cpp"
    break;

  case 63:
#line 171 "src/parser.y"
  { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 877 "src/parser.cpp"
    break;

  case 64:
#line 172 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 883 "src/parser.cpp"
    break;

  case 65:
#line 173 "src/parser.y"
                 { yylhs.value.as < std::string > () = "*" + yystack_[0].value.as < std::string > (); }
#line 889 "src/parser.cpp"
    break;

  case 66:
#line 174 "src/parser.y"
                { yylhs.value.as < std::string > () = "*" + yystack_[0].value.as < std::string > (); }
#line 895 "src/parser.cpp"
    break;

  case 67:
#line 175 "src/parser.y"
                { yylhs.value.as < std::string > () = "*" + yystack_[0].value.as < std::string > (); }
#line 901 "src/parser.cpp"
    break;

  case 68:
#line 176 "src/parser.y"
               { yylhs.value.as < std::string > () = "*" + yystack_[0].value.as < std::string > (); }
#line 907 "src/parser.cpp"
    break;

  case 69:
#line 179 "src/parser.y"
                   { yylhs.value.as < std::string > () = "(" + yystack_[1].value.as < std::string > () + ")"; }
#line 913 "src/parser.cpp"
    break;

  case 70:
#line 180 "src/parser.y"
                             { yylhs.value.as < std::string > () = yystack_[3].value.as < std::string > () + "(" + yystack_[1].value.as < std::string > () + ")"; }
#line 919 "src/parser.cpp"
    break;

  case 71:
#line 181 "src/parser.y"
                            { yylhs.value.as < std::string > () = yystack_[3].value.as < std::string > () + "(" + yystack_[1].value.as < std::string > () + ")"; }
#line 925 "src/parser.cpp"
    break;

  case 72:
#line 182 "src/parser.y"
                         { yylhs.value.as < std::string > () = yystack_[3].value.as < std::string > () + "(%pc)"; }
#line 931 "src/parser.cpp"
    break;

  case 73:
#line 185 "src/parser.y"
               { yylhs.value.as < std::string > () = yystack_[1].value.as < std::string > () + "l"; }
#line 937 "src/parser.cpp"
    break;

  case 74:
#line 186 "src/parser.y"
                 { yylhs.value.as < std::string > () = yystack_[1].value.as < std::string > () + "w"; }
#line 943 "src/parser.cpp"
    break;

  case 79:
#line 197 "src/parser.y"
            { yylhs.value.as < std::string > () = yystack_[1].value.as < std::string > () + ":"; }
#line 949 "src/parser.cpp"
    break;

  case 80:
#line 200 "src/parser.y"
  { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 955 "src/parser.cpp"
    break;

  case 81:
#line 201 "src/parser.y"
    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 961 "src/parser.cpp"
    break;


#line 965 "src/parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        int yychecklim = yylast_ - yyn + 1;
        int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }



  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char parser::yypact_ninf_ = -48;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
     -48,    15,    -6,   -48,    18,   -48,   169,   -48,   144,    79,
      38,   112,    53,    29,     6,    21,   -13,    26,   154,   -48,
      22,    36,   -48,   -48,   -48,   -48,    39,    55,    72,   -48,
     -48,   -48,   -48,   -48,    33,    44,   107,   136,    57,   -48,
     -48,   -48,    71,   131,   140,    57,   -48,   -48,   -48,    71,
     107,   136,    75,   131,   140,    77,   -48,    80,    81,   -48,
     -48,   -48,   -48,    83,   169,   -48,   -48,   169,   169,   169,
     169,    57,   -48,   -48,    71,    95,   -48,   -48,   170,    57,
       7,   -48,   -48,   -48,    71,   -48,    -2,    87,    57,   -48,
     -48,   -48,    71,   -48,   101,   108,   140,   110,   113,   136,
      21,   -13,   178,   -48,   -48,   -48,   -48,   -48,   -48,   -48,
     -48,   -48,   -48,   114,   145,   146,   131,   140,   -48,   107,
     136,   -48,   -48,   -48,     9,    16,   -48,   -48,   -48,   -48,
     -48,   -48,   -48,   -48,   178,   178,   178,   178,   -48,   -48,
     -48,   -48
  };

  const signed char
  parser::yydefact_[] =
  {
       3,     0,     2,     1,     0,     4,     6,    29,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       0,     0,    13,    14,    15,    16,     0,     0,     0,    64,
      80,    81,    30,    63,     0,     0,     0,     0,    48,    45,
      31,    46,    47,     0,     0,    52,    49,    34,    50,    51,
       0,     0,     0,     0,     0,     0,    79,    75,    77,    17,
      18,    27,    19,     0,     6,    12,    28,     6,     6,     6,
       6,    68,    65,    66,    67,     0,    44,    43,     0,    58,
       0,    33,    56,    55,    57,    32,     0,     0,    62,    36,
      60,    59,    61,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     8,     9,    10,    11,    69,    54,
      53,    73,    74,     0,     0,     0,     0,     0,    37,     0,
       0,    40,    76,    78,    21,    22,    20,    72,    71,    70,
      39,    38,    42,    41,     0,     0,     0,     0,    23,    25,
      24,    26
  };

  const signed char
  parser::yypgoto_[] =
  {
     -48,     2,   -47,   -11,   -43,   -48,     4,   -36,   -48,    -8,
     -48,   -48,   -48,    78,   -48,   122,   -48,   -48,   -48,    43,
     -48,   -48,   126,   -10,    51
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,    40,    81,    47,    89,    32,    41,    83,    18,    42,
       1,     2,     5,    19,    20,    21,    22,    23,    24,   126,
      25,    26,    27,    59,    60
  };

  const unsigned char
  parser::yytable_[] =
  {
      33,    55,    49,    94,    49,    61,    58,    91,    62,   113,
      97,    30,    31,    52,    48,     3,    48,    91,   134,   135,
      74,   111,   112,     4,   114,   136,   137,    77,    84,    57,
      30,    31,    73,    93,     6,    92,    49,    56,    67,    85,
      82,    34,    84,    98,    57,    92,    49,    90,    48,    63,
      43,    44,    68,    95,    82,    69,    34,    90,    48,    75,
      86,    45,    30,    31,    46,    53,    54,    76,    30,    31,
     110,    70,   132,   130,    87,    34,    45,    30,    31,    46,
      91,    96,    34,    99,    35,   118,   100,   101,    49,   102,
     122,    36,    37,    58,   125,    71,    30,    31,    72,   108,
      48,   121,    38,    30,    31,    39,   131,   116,    92,    49,
      34,    84,    78,   115,   117,    34,   119,    35,   127,   120,
      90,    48,   133,    82,    50,    51,   125,   125,   125,   125,
      79,    30,    31,    80,    34,    38,    30,    31,    39,    34,
      65,    35,   103,    34,    66,   104,   105,   106,   107,   128,
     129,    28,   123,     0,    88,    30,    31,    80,     0,    38,
      30,    31,    39,    45,    30,    31,    46,    29,    30,    31,
      64,     7,     8,     9,    10,    11,    12,   138,   139,   140,
     141,    14,    15,     0,    16,    17,     7,     8,     9,    10,
      11,    12,    13,   109,    30,    31,    14,    15,     0,    16,
      17,   124,    30,    31
  };

  const short
  parser::yycheck_[] =
  {
       8,    12,    10,    50,    12,    15,    14,    43,    16,    11,
      53,    24,    25,    11,    10,     0,    12,    53,     9,    10,
      28,    14,    15,    29,    26,     9,    10,    35,    36,    23,
      24,    25,    28,    44,    16,    43,    44,     8,    16,    37,
      36,     3,    50,    54,    23,    53,    54,    43,    44,    23,
      12,    13,    16,    51,    50,    16,     3,    53,    54,    26,
       3,    23,    24,    25,    26,    12,    13,    23,    24,    25,
      78,    16,   119,   116,     3,     3,    23,    24,    25,    26,
     116,     6,     3,     6,     5,    96,     6,     6,    96,     6,
     100,    12,    13,   101,   102,    23,    24,    25,    26,     4,
      96,    99,    23,    24,    25,    26,   117,     6,   116,   117,
       3,   119,     5,    26,     6,     3,     6,     5,     4,     6,
     116,   117,   120,   119,    12,    13,   134,   135,   136,   137,
      23,    24,    25,    26,     3,    23,    24,    25,    26,     3,
      18,     5,    64,     3,    18,    67,    68,    69,    70,     4,
       4,     7,   101,    -1,    23,    24,    25,    26,    -1,    23,
      24,    25,    26,    23,    24,    25,    26,    23,    24,    25,
      16,    17,    18,    19,    20,    21,    22,   134,   135,   136,
     137,    27,    28,    -1,    30,    31,    17,    18,    19,    20,
      21,    22,    23,    23,    24,    25,    27,    28,    -1,    30,
      31,    23,    24,    25
  };

  const signed char
  parser::yystos_[] =
  {
       0,    42,    43,     0,    29,    44,    16,    17,    18,    19,
      20,    21,    22,    23,    27,    28,    30,    31,    40,    45,
      46,    47,    48,    49,    50,    52,    53,    54,     7,    23,
      24,    25,    37,    41,     3,     5,    12,    13,    23,    26,
      33,    38,    41,    12,    13,    23,    26,    35,    38,    41,
      12,    13,    33,    12,    13,    35,     8,    23,    41,    55,
      56,    55,    41,    23,    16,    47,    54,    16,    16,    16,
      16,    23,    26,    38,    41,    26,    23,    41,     5,    23,
      26,    34,    38,    39,    41,    33,     3,     3,    23,    36,
      38,    39,    41,    35,    34,    33,     6,    36,    35,     6,
       6,     6,     6,    45,    45,    45,    45,    45,     4,    23,
      41,    14,    15,    11,    26,    26,     6,     6,    35,     6,
       6,    33,    55,    56,    23,    41,    51,     4,     4,     4,
      36,    35,    34,    33,     9,    10,     9,    10,    51,    51,
      51,    51
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    32,    42,    43,    43,    44,    45,    45,    45,    45,
      45,    45,    46,    47,    47,    47,    47,    48,    48,    49,
      50,    51,    51,    51,    51,    51,    51,    52,    53,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    33,    33,    33,    33,    33,    33,    35,
      35,    35,    35,    34,    34,    34,    34,    34,    34,    36,
      36,    36,    36,    37,    37,    37,    37,    37,    37,    38,
      38,    38,    38,    39,    39,    55,    55,    56,    56,    40,
      41,    41
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     3,     0,     3,     3,     3,
       3,     3,     2,     1,     1,     1,     1,     2,     2,     2,
       4,     1,     1,     3,     3,     3,     3,     2,     2,     1,
       2,     2,     3,     3,     2,     3,     3,     4,     5,     5,
       4,     5,     5,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     3,
       4,     4,     4,     2,     2,     1,     3,     1,     3,     2,
       1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end\"", "error", "\"invalid token\"", "\"(\"", "\")\"", "\"$\"",
  "\",\"", "\"*\"", "\":\"", "\"+\"", "\"-\"", "\"%pc\"", "\"b\"", "\"w\"",
  "\"l\"", "\"h\"", "NEW_LINE", "NOOP_INS", "OOP_INS_JMP",
  "OOP_INS_DATA_SRC", "OOP_INS_DATA_DST", "TOP_INS_DATA_STD",
  "TOP_INS_DATA_NSTD", "SYMBOL", "NUM_LITERAL", "CHAR_LITERAL", "REGISTER",
  "ALLOC_DIRECTIVE_NAME", "SYMBOL_DIRECTIVE_NAME", "SECTION_DIRECTIVE",
  "\".skip\"", "\".equ\"", "$accept", "src_op_data_word",
  "src_op_data_byte", "dst_op_data_word", "dst_op_data_byte", "dst_op_jmp",
  "reg_ind", "reg_byte", "label", "literal", "program", "sections",
  "section", "code", "labeled_directive", "directive", "alloc_directive",
  "skip_directive", "equ_directive", "exp", "symbol_directive",
  "labeled_operation", "operation", "symbol_list", "literal_list", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  parser::yyrline_[] =
  {
       0,    76,    76,    79,    80,    83,    86,    87,    88,    89,
      90,    91,    94,    97,    98,    99,   100,   103,   104,   107,
     110,   113,   114,   115,   116,   117,   118,   121,   124,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   143,   144,   145,   146,   147,   148,   151,
     152,   153,   154,   157,   158,   159,   160,   161,   162,   165,
     166,   167,   168,   171,   172,   173,   174,   175,   176,   179,
     180,   181,   182,   185,   186,   189,   190,   193,   194,   197,
     200,   201
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 1541 "src/parser.cpp"

#line 202 "src/parser.y"


void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << "\n" << l << "\n" << m << '\n';
}
