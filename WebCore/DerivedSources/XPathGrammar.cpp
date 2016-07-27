/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7.12-4996"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         xpathyyparse
#define yylex           xpathyylex
#define yyerror         xpathyyerror
#define yylval          xpathyylval
#define yychar          xpathyychar
#define yydebug         xpathyydebug
#define yynerrs         xpathyynerrs

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 28 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"


#include "config.h"

#include "XPathFunctions.h"
#include "XPathParser.h"
#include "XPathPath.h"
#include "XPathVariableReference.h"

#define YYMALLOC fastMalloc
#define YYFREE fastFree

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYDEBUG 0
#define YYMAXDEPTH 10000

using namespace WebCore;
using namespace XPath;


/* Line 371 of yacc.c  */
#line 97 "./XPathGrammar.cpp"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "XPathGrammar.hpp".  */
#ifndef YY_XPATHYY_XPATHGRAMMAR_HPP_INCLUDED
# define YY_XPATHYY_XPATHGRAMMAR_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int xpathyydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     MULOP = 258,
     RELOP = 259,
     EQOP = 260,
     MINUS = 261,
     PLUS = 262,
     AND = 263,
     OR = 264,
     FUNCTIONNAME = 265,
     LITERAL = 266,
     NAMETEST = 267,
     NUMBER = 268,
     NODETYPE = 269,
     VARIABLEREFERENCE = 270,
     AXISNAME = 271,
     COMMENT = 272,
     DOTDOT = 273,
     PI = 274,
     NODE = 275,
     SLASHSLASH = 276,
     TEXT = 277,
     XPATH_ERROR = 278
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 54 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 NumericOp::Opcode numericOpcode; 
/* Line 387 of yacc.c  */
#line 57 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 EqTestOp::Opcode equalityTestOpcode; 
/* Line 387 of yacc.c  */
#line 64 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 StringImpl* string; 
/* Line 387 of yacc.c  */
#line 68 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Step::Axis axis; 
/* Line 387 of yacc.c  */
#line 74 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 LocationPath* locationPath; 
/* Line 387 of yacc.c  */
#line 78 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Step::NodeTest* nodeTest; 
/* Line 387 of yacc.c  */
#line 82 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Vector<std::unique_ptr<Expression>>* expressionVector; 
/* Line 387 of yacc.c  */
#line 86 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Step* step; 
/* Line 387 of yacc.c  */
#line 90 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Expression* expression; 

/* Line 387 of yacc.c  */
#line 190 "./XPathGrammar.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int xpathyyparse (void *YYPARSE_PARAM);
#else
int xpathyyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int xpathyyparse (Parser& parser);
#else
int xpathyyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_XPATHYY_XPATHGRAMMAR_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */
/* Line 390 of yacc.c  */
#line 94 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"


static int xpathyylex(YYSTYPE* yylval, Parser& parser) { return parser.lex(*yylval); }
static void xpathyyerror(Parser&, const char*) { }


/* Line 390 of yacc.c  */
#line 224 "./XPathGrammar.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  52
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   132

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  33
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  64
/* YYNRULES -- Number of states.  */
#define YYNSTATES  101

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   278

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      26,    27,     2,     2,    31,     2,    30,    24,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    25,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    28,     2,    29,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    32,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    16,    19,
      21,    25,    29,    32,    35,    39,    43,    45,    47,    49,
      53,    57,    61,    65,    70,    71,    73,    75,    78,    82,
      84,    86,    88,    90,    94,    96,    98,   100,   104,   109,
     111,   115,   117,   119,   123,   125,   127,   131,   135,   137,
     140,   142,   146,   148,   152,   154,   158,   160,   164,   166,
     170,   174,   176,   180,   182
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      34,     0,    -1,    35,    -1,    54,    -1,    37,    -1,    38,
      -1,    24,    -1,    24,    38,    -1,    45,    38,    -1,    39,
      -1,    38,    24,    39,    -1,    38,    45,    39,    -1,    41,
      42,    -1,    12,    42,    -1,    40,    41,    42,    -1,    40,
      12,    42,    -1,    46,    -1,    16,    -1,    25,    -1,    20,
      26,    27,    -1,    22,    26,    27,    -1,    17,    26,    27,
      -1,    19,    26,    27,    -1,    19,    26,    11,    27,    -1,
      -1,    43,    -1,    44,    -1,    43,    44,    -1,    28,    35,
      29,    -1,    21,    -1,    30,    -1,    18,    -1,    15,    -1,
      26,    35,    27,    -1,    11,    -1,    13,    -1,    48,    -1,
      10,    26,    27,    -1,    10,    26,    49,    27,    -1,    50,
      -1,    49,    31,    50,    -1,    35,    -1,    52,    -1,    51,
      32,    52,    -1,    36,    -1,    53,    -1,    53,    24,    38,
      -1,    53,    45,    38,    -1,    47,    -1,    47,    43,    -1,
      55,    -1,    54,     9,    55,    -1,    56,    -1,    55,     8,
      56,    -1,    57,    -1,    56,     5,    57,    -1,    58,    -1,
      57,     4,    58,    -1,    59,    -1,    58,     7,    59,    -1,
      58,     6,    59,    -1,    60,    -1,    59,     3,    60,    -1,
      51,    -1,     6,    60,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   104,   104,   111,   115,   121,   125,   130,   135,   143,
     149,   155,   164,   174,   192,   203,   221,   225,   227,   234,
     239,   244,   249,   254,   263,   267,   271,   277,   285,   292,
     299,   304,   311,   317,   322,   328,   334,   338,   346,   357,
     363,   371,   375,   377,   384,   389,   391,   397,   406,   408,
     416,   418,   425,   427,   434,   436,   443,   445,   452,   454,
     459,   466,   468,   475,   477
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "MULOP", "RELOP", "EQOP", "MINUS",
  "PLUS", "AND", "OR", "FUNCTIONNAME", "LITERAL", "NAMETEST", "NUMBER",
  "NODETYPE", "VARIABLEREFERENCE", "AXISNAME", "COMMENT", "DOTDOT", "PI",
  "NODE", "SLASHSLASH", "TEXT", "XPATH_ERROR", "'/'", "'@'", "'('", "')'",
  "'['", "']'", "'.'", "','", "'|'", "$accept", "Top", "Expr",
  "LocationPath", "AbsoluteLocationPath", "RelativeLocationPath", "Step",
  "AxisSpecifier", "NodeTest", "OptionalPredicateList", "PredicateList",
  "Predicate", "DescendantOrSelf", "AbbreviatedStep", "PrimaryExpr",
  "FunctionCall", "ArgumentList", "Argument", "UnionExpr", "PathExpr",
  "FilterExpr", "OrExpr", "AndExpr", "EqualityExpr", "RelationalExpr",
  "AdditiveExpr", "MultiplicativeExpr", "UnaryExpr", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,    47,    64,    40,    41,    91,    93,
      46,    44,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    33,    34,    35,    36,    36,    37,    37,    37,    38,
      38,    38,    39,    39,    39,    39,    39,    40,    40,    41,
      41,    41,    41,    41,    42,    42,    43,    43,    44,    45,
      46,    46,    47,    47,    47,    47,    47,    48,    48,    49,
      49,    50,    51,    51,    52,    52,    52,    52,    53,    53,
      54,    54,    55,    55,    56,    56,    57,    57,    58,    58,
      58,    59,    59,    60,    60
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     2,     2,     1,
       3,     3,     2,     2,     3,     3,     1,     1,     1,     3,
       3,     3,     3,     4,     0,     1,     1,     2,     3,     1,
       1,     1,     1,     3,     1,     1,     1,     3,     4,     1,
       3,     1,     1,     3,     1,     1,     3,     3,     1,     2,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       3,     1,     3,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,    34,    24,    35,    32,    17,     0,    31,
       0,     0,    29,     0,     6,    18,     0,    30,     0,     2,
      44,     4,     5,     9,     0,    24,     0,    16,    48,    36,
      63,    42,    45,     3,    50,    52,    54,    56,    58,    61,
      64,     0,     0,    13,    25,    26,     0,     0,     0,     0,
       7,     0,     1,     0,     0,    24,    24,    12,     8,    49,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      37,    41,     0,    39,     0,    27,    21,     0,    22,    19,
      20,    33,    10,    11,    15,    14,    43,    46,    47,    51,
      53,    55,    57,    60,    59,    62,    38,     0,    28,    23,
      40
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    18,    71,    20,    21,    22,    23,    24,    25,    43,
      44,    45,    26,    27,    28,    29,    72,    73,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -37
static const yytype_int8 yypact[] =
{
      81,    81,    -8,   -37,    -7,   -37,   -37,   -37,    14,   -37,
      17,    20,   -37,    21,     8,   -37,    81,   -37,    48,   -37,
     -37,   -37,   -17,   -37,    22,    -7,     8,   -37,    -7,   -37,
      23,   -37,    -9,     1,    45,    49,    52,    10,    54,   -37,
     -37,    59,    81,   -37,    -7,   -37,    31,    -5,    32,    33,
     -17,    34,   -37,     8,     8,    -7,    -7,   -37,   -17,    -7,
     102,     8,     8,    81,    81,    81,    81,    81,    81,    81,
     -37,   -37,   -18,   -37,    35,   -37,   -37,    36,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -17,   -17,    45,
      49,    52,    10,    54,    54,   -37,   -37,    81,   -37,   -37,
     -37
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -37,   -37,     3,   -37,   -37,   -12,   -22,   -37,    38,   -20,
      60,   -36,   -21,   -37,   -37,   -37,   -37,   -24,   -37,    30,
     -37,   -37,    19,    40,    43,    29,   -16,    -1
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      40,    54,    50,    19,    12,    57,    77,    53,    75,    96,
      63,    62,    12,    97,    58,    61,    67,    68,    41,    51,
       4,    42,    78,    75,     7,     8,     9,    10,    11,    54,
      13,    82,    83,    15,    55,    84,    85,    54,    17,     8,
      46,    10,    11,    47,    13,    74,    48,    49,    52,    87,
      88,    93,    94,    64,    65,    60,    66,    69,    76,    79,
      80,    81,    56,    99,    98,     1,    54,    54,    95,     2,
       3,     4,     5,   100,     6,     7,     8,     9,    10,    11,
      12,    13,    89,    14,    15,    16,    70,     1,    59,    17,
      86,     2,     3,     4,     5,    92,     6,     7,     8,     9,
      10,    11,    12,    13,    90,    14,    15,    16,    91,     0,
       0,    17,     2,     3,     4,     5,     0,     6,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,    16,     0,
       0,     0,    17
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-37)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
       1,    22,    14,     0,    21,    25,    11,    24,    44,    27,
       9,    32,    21,    31,    26,    24,     6,     7,    26,    16,
      12,    28,    27,    59,    16,    17,    18,    19,    20,    50,
      22,    53,    54,    25,    12,    55,    56,    58,    30,    17,
      26,    19,    20,    26,    22,    42,    26,    26,     0,    61,
      62,    67,    68,     8,     5,    32,     4,     3,    27,    27,
      27,    27,    24,    27,    29,     6,    87,    88,    69,    10,
      11,    12,    13,    97,    15,    16,    17,    18,    19,    20,
      21,    22,    63,    24,    25,    26,    27,     6,    28,    30,
      60,    10,    11,    12,    13,    66,    15,    16,    17,    18,
      19,    20,    21,    22,    64,    24,    25,    26,    65,    -1,
      -1,    30,    10,    11,    12,    13,    -1,    15,    16,    17,
      18,    19,    20,    21,    22,    -1,    24,    25,    26,    -1,
      -1,    -1,    30
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     6,    10,    11,    12,    13,    15,    16,    17,    18,
      19,    20,    21,    22,    24,    25,    26,    30,    34,    35,
      36,    37,    38,    39,    40,    41,    45,    46,    47,    48,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      60,    26,    28,    42,    43,    44,    26,    26,    26,    26,
      38,    35,     0,    24,    45,    12,    41,    42,    38,    43,
      32,    24,    45,     9,     8,     5,     4,     6,     7,     3,
      27,    35,    49,    50,    35,    44,    27,    11,    27,    27,
      27,    27,    39,    39,    42,    42,    52,    38,    38,    55,
      56,    57,    58,    59,    59,    60,    27,    31,    29,    27,
      50
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (parser, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, parser)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, parser); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, Parser& parser)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    Parser& parser;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (parser);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, Parser& parser)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    Parser& parser;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, Parser& parser)
#else
static void
yy_reduce_print (yyvsp, yyrule, parser)
    YYSTYPE *yyvsp;
    int yyrule;
    Parser& parser;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , parser);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, parser); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, Parser& parser)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, parser)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    Parser& parser;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (parser);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 10: /* FUNCTIONNAME */
/* Line 1393 of yacc.c  */
#line 66 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { if (((*yyvaluep).string)) ((*yyvaluep).string)->deref(); };
/* Line 1393 of yacc.c  */
#line 1243 "./XPathGrammar.cpp"
        break;
      case 11: /* LITERAL */
/* Line 1393 of yacc.c  */
#line 66 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { if (((*yyvaluep).string)) ((*yyvaluep).string)->deref(); };
/* Line 1393 of yacc.c  */
#line 1250 "./XPathGrammar.cpp"
        break;
      case 12: /* NAMETEST */
/* Line 1393 of yacc.c  */
#line 66 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { if (((*yyvaluep).string)) ((*yyvaluep).string)->deref(); };
/* Line 1393 of yacc.c  */
#line 1257 "./XPathGrammar.cpp"
        break;
      case 13: /* NUMBER */
/* Line 1393 of yacc.c  */
#line 66 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { if (((*yyvaluep).string)) ((*yyvaluep).string)->deref(); };
/* Line 1393 of yacc.c  */
#line 1264 "./XPathGrammar.cpp"
        break;
      case 14: /* NODETYPE */
/* Line 1393 of yacc.c  */
#line 66 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { if (((*yyvaluep).string)) ((*yyvaluep).string)->deref(); };
/* Line 1393 of yacc.c  */
#line 1271 "./XPathGrammar.cpp"
        break;
      case 15: /* VARIABLEREFERENCE */
/* Line 1393 of yacc.c  */
#line 66 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { if (((*yyvaluep).string)) ((*yyvaluep).string)->deref(); };
/* Line 1393 of yacc.c  */
#line 1278 "./XPathGrammar.cpp"
        break;
      case 35: /* Expr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1285 "./XPathGrammar.cpp"
        break;
      case 36: /* LocationPath */
/* Line 1393 of yacc.c  */
#line 76 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).locationPath); };
/* Line 1393 of yacc.c  */
#line 1292 "./XPathGrammar.cpp"
        break;
      case 37: /* AbsoluteLocationPath */
/* Line 1393 of yacc.c  */
#line 76 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).locationPath); };
/* Line 1393 of yacc.c  */
#line 1299 "./XPathGrammar.cpp"
        break;
      case 38: /* RelativeLocationPath */
/* Line 1393 of yacc.c  */
#line 76 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).locationPath); };
/* Line 1393 of yacc.c  */
#line 1306 "./XPathGrammar.cpp"
        break;
      case 39: /* Step */
/* Line 1393 of yacc.c  */
#line 88 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).step); };
/* Line 1393 of yacc.c  */
#line 1313 "./XPathGrammar.cpp"
        break;
      case 41: /* NodeTest */
/* Line 1393 of yacc.c  */
#line 80 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).nodeTest); };
/* Line 1393 of yacc.c  */
#line 1320 "./XPathGrammar.cpp"
        break;
      case 42: /* OptionalPredicateList */
/* Line 1393 of yacc.c  */
#line 84 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expressionVector); };
/* Line 1393 of yacc.c  */
#line 1327 "./XPathGrammar.cpp"
        break;
      case 43: /* PredicateList */
/* Line 1393 of yacc.c  */
#line 84 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expressionVector); };
/* Line 1393 of yacc.c  */
#line 1334 "./XPathGrammar.cpp"
        break;
      case 44: /* Predicate */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1341 "./XPathGrammar.cpp"
        break;
      case 45: /* DescendantOrSelf */
/* Line 1393 of yacc.c  */
#line 88 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).step); };
/* Line 1393 of yacc.c  */
#line 1348 "./XPathGrammar.cpp"
        break;
      case 46: /* AbbreviatedStep */
/* Line 1393 of yacc.c  */
#line 88 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).step); };
/* Line 1393 of yacc.c  */
#line 1355 "./XPathGrammar.cpp"
        break;
      case 47: /* PrimaryExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1362 "./XPathGrammar.cpp"
        break;
      case 48: /* FunctionCall */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1369 "./XPathGrammar.cpp"
        break;
      case 49: /* ArgumentList */
/* Line 1393 of yacc.c  */
#line 84 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expressionVector); };
/* Line 1393 of yacc.c  */
#line 1376 "./XPathGrammar.cpp"
        break;
      case 50: /* Argument */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1383 "./XPathGrammar.cpp"
        break;
      case 51: /* UnionExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1390 "./XPathGrammar.cpp"
        break;
      case 52: /* PathExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1397 "./XPathGrammar.cpp"
        break;
      case 53: /* FilterExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1404 "./XPathGrammar.cpp"
        break;
      case 54: /* OrExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1411 "./XPathGrammar.cpp"
        break;
      case 55: /* AndExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1418 "./XPathGrammar.cpp"
        break;
      case 56: /* EqualityExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1425 "./XPathGrammar.cpp"
        break;
      case 57: /* RelationalExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1432 "./XPathGrammar.cpp"
        break;
      case 58: /* AdditiveExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1439 "./XPathGrammar.cpp"
        break;
      case 59: /* MultiplicativeExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1446 "./XPathGrammar.cpp"
        break;
      case 60: /* UnaryExpr */
/* Line 1393 of yacc.c  */
#line 92 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
        { delete ((*yyvaluep).expression); };
/* Line 1393 of yacc.c  */
#line 1453 "./XPathGrammar.cpp"
        break;

      default:
        break;
    }
}




/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (Parser& parser)
#else
int
yyparse (parser)
    Parser& parser;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;


#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
static YYSTYPE yyval_default;
# define YY_INITIAL_VALUE(Value) = Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1787 of yacc.c  */
#line 105 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        parser.setParseResult(std::unique_ptr<Expression>((yyvsp[(1) - (1)].expression)));
    }
    break;

  case 4:
/* Line 1787 of yacc.c  */
#line 116 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.locationPath) = (yyvsp[(1) - (1)].locationPath);
        (yyval.locationPath)->setAbsolute();
    }
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 126 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.locationPath) = new LocationPath;
    }
    break;

  case 7:
/* Line 1787 of yacc.c  */
#line 131 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.locationPath) = (yyvsp[(2) - (2)].locationPath);
    }
    break;

  case 8:
/* Line 1787 of yacc.c  */
#line 136 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.locationPath) = (yyvsp[(2) - (2)].locationPath);
        (yyval.locationPath)->prependStep(std::unique_ptr<Step>((yyvsp[(1) - (2)].step)));
    }
    break;

  case 9:
/* Line 1787 of yacc.c  */
#line 144 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.locationPath) = new LocationPath;
        (yyval.locationPath)->appendStep(std::unique_ptr<Step>((yyvsp[(1) - (1)].step)));
    }
    break;

  case 10:
/* Line 1787 of yacc.c  */
#line 150 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.locationPath) = (yyvsp[(1) - (3)].locationPath);
        (yyval.locationPath)->appendStep(std::unique_ptr<Step>((yyvsp[(3) - (3)].step)));
    }
    break;

  case 11:
/* Line 1787 of yacc.c  */
#line 156 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.locationPath) = (yyvsp[(1) - (3)].locationPath);
        (yyval.locationPath)->appendStep(std::unique_ptr<Step>((yyvsp[(2) - (3)].step)));
        (yyval.locationPath)->appendStep(std::unique_ptr<Step>((yyvsp[(3) - (3)].step)));
    }
    break;

  case 12:
/* Line 1787 of yacc.c  */
#line 165 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        std::unique_ptr<Step::NodeTest> nodeTest((yyvsp[(1) - (2)].nodeTest));
        std::unique_ptr<Vector<std::unique_ptr<Expression>>> predicateList((yyvsp[(2) - (2)].expressionVector));
        if (predicateList)
            (yyval.step) = new Step(Step::ChildAxis, std::move(*nodeTest), std::move(*predicateList));
        else
            (yyval.step) = new Step(Step::ChildAxis, std::move(*nodeTest));
    }
    break;

  case 13:
/* Line 1787 of yacc.c  */
#line 175 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        String nametest = adoptRef((yyvsp[(1) - (2)].string));
        std::unique_ptr<Vector<std::unique_ptr<Expression>>> predicateList((yyvsp[(2) - (2)].expressionVector));

        String localName;
        String namespaceURI;
        if (!parser.expandQualifiedName(nametest, localName, namespaceURI)) {
            (yyval.step) = nullptr;
            YYABORT;
        }

        if (predicateList)
            (yyval.step) = new Step(Step::ChildAxis, Step::NodeTest(Step::NodeTest::NameTest, localName, namespaceURI), std::move(*predicateList));
        else
            (yyval.step) = new Step(Step::ChildAxis, Step::NodeTest(Step::NodeTest::NameTest, localName, namespaceURI));
    }
    break;

  case 14:
/* Line 1787 of yacc.c  */
#line 193 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        std::unique_ptr<Step::NodeTest> nodeTest((yyvsp[(2) - (3)].nodeTest));
        std::unique_ptr<Vector<std::unique_ptr<Expression>>> predicateList((yyvsp[(3) - (3)].expressionVector));

        if (predicateList)
            (yyval.step) = new Step((yyvsp[(1) - (3)].axis), std::move(*nodeTest), std::move(*predicateList));
        else
            (yyval.step) = new Step((yyvsp[(1) - (3)].axis), std::move(*nodeTest));
    }
    break;

  case 15:
/* Line 1787 of yacc.c  */
#line 204 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        String nametest = adoptRef((yyvsp[(2) - (3)].string));
        std::unique_ptr<Vector<std::unique_ptr<Expression>>> predicateList((yyvsp[(3) - (3)].expressionVector));

        String localName;
        String namespaceURI;
        if (!parser.expandQualifiedName(nametest, localName, namespaceURI)) {
            (yyval.step) = nullptr;
            YYABORT;
        }

        if (predicateList)
            (yyval.step) = new Step((yyvsp[(1) - (3)].axis), Step::NodeTest(Step::NodeTest::NameTest, localName, namespaceURI), std::move(*predicateList));
        else
            (yyval.step) = new Step((yyvsp[(1) - (3)].axis), Step::NodeTest(Step::NodeTest::NameTest, localName, namespaceURI));
    }
    break;

  case 18:
/* Line 1787 of yacc.c  */
#line 228 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.axis) = Step::AttributeAxis;
    }
    break;

  case 19:
/* Line 1787 of yacc.c  */
#line 235 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.nodeTest) = new Step::NodeTest(Step::NodeTest::AnyNodeTest);
    }
    break;

  case 20:
/* Line 1787 of yacc.c  */
#line 240 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.nodeTest) = new Step::NodeTest(Step::NodeTest::TextNodeTest);
    }
    break;

  case 21:
/* Line 1787 of yacc.c  */
#line 245 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.nodeTest) = new Step::NodeTest(Step::NodeTest::CommentNodeTest);
    }
    break;

  case 22:
/* Line 1787 of yacc.c  */
#line 250 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.nodeTest) = new Step::NodeTest(Step::NodeTest::ProcessingInstructionNodeTest);
    }
    break;

  case 23:
/* Line 1787 of yacc.c  */
#line 255 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        String literal = adoptRef((yyvsp[(3) - (4)].string));
        (yyval.nodeTest) = new Step::NodeTest(Step::NodeTest::ProcessingInstructionNodeTest, literal.stripWhiteSpace());
    }
    break;

  case 24:
/* Line 1787 of yacc.c  */
#line 263 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expressionVector) = nullptr;
    }
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 272 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expressionVector) = new Vector<std::unique_ptr<Expression>>;
        (yyval.expressionVector)->append(std::unique_ptr<Expression>((yyvsp[(1) - (1)].expression)));
    }
    break;

  case 27:
/* Line 1787 of yacc.c  */
#line 278 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expressionVector) = (yyvsp[(1) - (2)].expressionVector);
        (yyval.expressionVector)->append(std::unique_ptr<Expression>((yyvsp[(2) - (2)].expression)));
    }
    break;

  case 28:
/* Line 1787 of yacc.c  */
#line 286 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = (yyvsp[(2) - (3)].expression);
    }
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 293 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.step) = new Step(Step::DescendantOrSelfAxis, Step::NodeTest(Step::NodeTest::AnyNodeTest));
    }
    break;

  case 30:
/* Line 1787 of yacc.c  */
#line 300 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.step) = new Step(Step::SelfAxis, Step::NodeTest(Step::NodeTest::AnyNodeTest));
    }
    break;

  case 31:
/* Line 1787 of yacc.c  */
#line 305 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.step) = new Step(Step::ParentAxis, Step::NodeTest(Step::NodeTest::AnyNodeTest));
    }
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 312 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        String name = adoptRef((yyvsp[(1) - (1)].string));
        (yyval.expression) = new VariableReference(name);
    }
    break;

  case 33:
/* Line 1787 of yacc.c  */
#line 318 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = (yyvsp[(2) - (3)].expression);
    }
    break;

  case 34:
/* Line 1787 of yacc.c  */
#line 323 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        String literal = adoptRef((yyvsp[(1) - (1)].string));
        (yyval.expression) = new StringExpression(std::move(literal));
    }
    break;

  case 35:
/* Line 1787 of yacc.c  */
#line 329 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        String numeral = adoptRef((yyvsp[(1) - (1)].string));
        (yyval.expression) = new Number(numeral.toDouble());
    }
    break;

  case 37:
/* Line 1787 of yacc.c  */
#line 339 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        String name = adoptRef((yyvsp[(1) - (3)].string));
        (yyval.expression) = XPath::Function::create(name).release();
        if (!(yyval.expression))
            YYABORT;
    }
    break;

  case 38:
/* Line 1787 of yacc.c  */
#line 347 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        String name = adoptRef((yyvsp[(1) - (4)].string));
        std::unique_ptr<Vector<std::unique_ptr<Expression>>> argumentList((yyvsp[(3) - (4)].expressionVector));
        (yyval.expression) = XPath::Function::create(name, std::move(*argumentList)).release();
        if (!(yyval.expression))
            YYABORT;
    }
    break;

  case 39:
/* Line 1787 of yacc.c  */
#line 358 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expressionVector) = new Vector<std::unique_ptr<Expression>>;
        (yyval.expressionVector)->append(std::unique_ptr<Expression>((yyvsp[(1) - (1)].expression)));
    }
    break;

  case 40:
/* Line 1787 of yacc.c  */
#line 364 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expressionVector) = (yyvsp[(1) - (3)].expressionVector);
        (yyval.expressionVector)->append(std::unique_ptr<Expression>((yyvsp[(3) - (3)].expression)));
    }
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 378 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = new Union(std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<Expression>((yyvsp[(3) - (3)].expression)));
    }
    break;

  case 44:
/* Line 1787 of yacc.c  */
#line 385 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = (yyvsp[(1) - (1)].locationPath);
    }
    break;

  case 46:
/* Line 1787 of yacc.c  */
#line 392 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyvsp[(3) - (3)].locationPath)->setAbsolute();
        (yyval.expression) = new Path(std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<LocationPath>((yyvsp[(3) - (3)].locationPath)));
    }
    break;

  case 47:
/* Line 1787 of yacc.c  */
#line 398 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyvsp[(3) - (3)].locationPath)->prependStep(std::unique_ptr<Step>((yyvsp[(2) - (3)].step)));
        (yyvsp[(3) - (3)].locationPath)->setAbsolute();
        (yyval.expression) = new Path(std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<LocationPath>((yyvsp[(3) - (3)].locationPath)));
    }
    break;

  case 49:
/* Line 1787 of yacc.c  */
#line 409 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        std::unique_ptr<Vector<std::unique_ptr<Expression>>> predicateList((yyvsp[(2) - (2)].expressionVector));
        (yyval.expression) = new Filter(std::unique_ptr<Expression>((yyvsp[(1) - (2)].expression)), std::move(*predicateList));
    }
    break;

  case 51:
/* Line 1787 of yacc.c  */
#line 419 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = new LogicalOp(LogicalOp::OP_Or, std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<Expression>((yyvsp[(3) - (3)].expression)));
    }
    break;

  case 53:
/* Line 1787 of yacc.c  */
#line 428 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = new LogicalOp(LogicalOp::OP_And, std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<Expression>((yyvsp[(3) - (3)].expression)));
    }
    break;

  case 55:
/* Line 1787 of yacc.c  */
#line 437 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = new EqTestOp((yyvsp[(2) - (3)].equalityTestOpcode), std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<Expression>((yyvsp[(3) - (3)].expression)));
    }
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 446 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = new EqTestOp((yyvsp[(2) - (3)].equalityTestOpcode), std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<Expression>((yyvsp[(3) - (3)].expression)));
    }
    break;

  case 59:
/* Line 1787 of yacc.c  */
#line 455 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = new NumericOp(NumericOp::OP_Add, std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<Expression>((yyvsp[(3) - (3)].expression)));
    }
    break;

  case 60:
/* Line 1787 of yacc.c  */
#line 460 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = new NumericOp(NumericOp::OP_Sub, std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<Expression>((yyvsp[(3) - (3)].expression)));
    }
    break;

  case 62:
/* Line 1787 of yacc.c  */
#line 469 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = new NumericOp((yyvsp[(2) - (3)].numericOpcode), std::unique_ptr<Expression>((yyvsp[(1) - (3)].expression)), std::unique_ptr<Expression>((yyvsp[(3) - (3)].expression)));
    }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 478 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
    {
        (yyval.expression) = new Negative(std::unique_ptr<Expression>((yyvsp[(2) - (2)].expression)));
    }
    break;


/* Line 1787 of yacc.c  */
#line 2186 "./XPathGrammar.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (parser, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (parser, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, parser);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, parser);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (parser, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, parser);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, parser);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2050 of yacc.c  */
#line 483 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"

