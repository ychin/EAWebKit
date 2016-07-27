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
#define yyparse         cssyyparse
#define yylex           cssyylex
#define yyerror         cssyyerror
#define yylval          cssyylval
#define yychar          cssyychar
#define yydebug         cssyydebug
#define yynerrs         cssyynerrs

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 1 "./CSSGrammar.y"


/*
 *  Copyright (C) 2002-2003 Lars Knoll (knoll@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 *  Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"

#include "CSSParser.h"
#include "CSSParserMode.h"
#include "CSSPrimitiveValue.h"
#include "CSSPropertyNames.h"
#include "CSSSelector.h"
#include "CSSSelectorList.h"
#include "Document.h"
#include "HTMLNames.h"
#include "MediaList.h"
#include "MediaQueryExp.h"
#include "StyleRule.h"
#include "StyleSheetContents.h"
#include "WebKitCSSKeyframeRule.h"
#include "WebKitCSSKeyframesRule.h"
#include <wtf/FastMalloc.h>
#include <stdlib.h>
#include <string.h>

using namespace WebCore;
using namespace HTMLNames;

#define YYMALLOC fastMalloc
#define YYFREE fastFree

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 10000
#define YYDEBUG 0

#if YYDEBUG > 0
#include <wtf/text/CString.h>
#define YYPRINT(File,Type,Value) if (isCSSTokenAString(Type)) YYFPRINTF(File, "%s", String((Value).string).utf8().data())
#endif


/* Line 371 of yacc.c  */
#line 137 "./CSSGrammar.cpp"

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
   by #include "CSSGrammar.hpp".  */
#ifndef YY_CSSYY_CSSGRAMMAR_HPP_INCLUDED
# define YY_CSSYY_CSSGRAMMAR_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int cssyydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_EOF = 0,
     LOWEST_PREC = 258,
     UNIMPORTANT_TOK = 259,
     WHITESPACE = 260,
     SGML_CD = 261,
     INCLUDES = 262,
     DASHMATCH = 263,
     BEGINSWITH = 264,
     ENDSWITH = 265,
     CONTAINS = 266,
     STRING = 267,
     IDENT = 268,
     NTH = 269,
     HEX = 270,
     IDSEL = 271,
     IMPORT_SYM = 272,
     PAGE_SYM = 273,
     MEDIA_SYM = 274,
     FONT_FACE_SYM = 275,
     CHARSET_SYM = 276,
     NAMESPACE_SYM = 277,
     VARFUNCTION = 278,
     WEBKIT_RULE_SYM = 279,
     WEBKIT_DECLS_SYM = 280,
     WEBKIT_KEYFRAME_RULE_SYM = 281,
     WEBKIT_KEYFRAMES_SYM = 282,
     WEBKIT_VALUE_SYM = 283,
     WEBKIT_MEDIAQUERY_SYM = 284,
     WEBKIT_SELECTOR_SYM = 285,
     WEBKIT_REGION_RULE_SYM = 286,
     WEBKIT_VIEWPORT_RULE_SYM = 287,
     TOPLEFTCORNER_SYM = 288,
     TOPLEFT_SYM = 289,
     TOPCENTER_SYM = 290,
     TOPRIGHT_SYM = 291,
     TOPRIGHTCORNER_SYM = 292,
     BOTTOMLEFTCORNER_SYM = 293,
     BOTTOMLEFT_SYM = 294,
     BOTTOMCENTER_SYM = 295,
     BOTTOMRIGHT_SYM = 296,
     BOTTOMRIGHTCORNER_SYM = 297,
     LEFTTOP_SYM = 298,
     LEFTMIDDLE_SYM = 299,
     LEFTBOTTOM_SYM = 300,
     RIGHTTOP_SYM = 301,
     RIGHTMIDDLE_SYM = 302,
     RIGHTBOTTOM_SYM = 303,
     ATKEYWORD = 304,
     IMPORTANT_SYM = 305,
     MEDIA_ONLY = 306,
     MEDIA_NOT = 307,
     MEDIA_AND = 308,
     REMS = 309,
     CHS = 310,
     QEMS = 311,
     EMS = 312,
     EXS = 313,
     PXS = 314,
     CMS = 315,
     MMS = 316,
     INS = 317,
     PTS = 318,
     PCS = 319,
     DEGS = 320,
     RADS = 321,
     GRADS = 322,
     TURNS = 323,
     MSECS = 324,
     SECS = 325,
     HERTZ = 326,
     KHERTZ = 327,
     DIMEN = 328,
     INVALIDDIMEN = 329,
     PERCENTAGE = 330,
     FLOATTOKEN = 331,
     INTEGER = 332,
     VW = 333,
     VH = 334,
     VMIN = 335,
     VMAX = 336,
     DPPX = 337,
     DPI = 338,
     DPCM = 339,
     FR = 340,
     URI = 341,
     FUNCTION = 342,
     ANYFUNCTION = 343,
     NOTFUNCTION = 344,
     CALCFUNCTION = 345,
     MINFUNCTION = 346,
     MAXFUNCTION = 347,
     VAR_DEFINITION = 348,
     UNICODERANGE = 349
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 65 "./CSSGrammar.y"

    double number;
    CSSParserString string;
    CSSSelector::MarginBoxType marginBox;
    CSSParserValue value;

/* Line 387 of yacc.c  */
#line 176 "./CSSGrammar.y"
 CSSSelector::Relation relation; 
/* Line 387 of yacc.c  */
#line 178 "./CSSGrammar.y"
 StyleRuleBase* rule; 
/* Line 387 of yacc.c  */
#line 181 "./CSSGrammar.y"
 Vector<RefPtr<StyleRuleBase>>* ruleList; 
/* Line 387 of yacc.c  */
#line 186 "./CSSGrammar.y"
 MediaQuerySet* mediaList; 
/* Line 387 of yacc.c  */
#line 189 "./CSSGrammar.y"
 MediaQuery* mediaQuery; 
/* Line 387 of yacc.c  */
#line 192 "./CSSGrammar.y"
 MediaQuery::Restrictor mediaQueryRestrictor; 
/* Line 387 of yacc.c  */
#line 194 "./CSSGrammar.y"
 MediaQueryExp* mediaQueryExp; 
/* Line 387 of yacc.c  */
#line 197 "./CSSGrammar.y"
 Vector<OwnPtr<MediaQueryExp>>* mediaQueryExpList; 
/* Line 387 of yacc.c  */
#line 201 "./CSSGrammar.y"
 StyleKeyframe* keyframe; 
/* Line 387 of yacc.c  */
#line 204 "./CSSGrammar.y"
 Vector<RefPtr<StyleKeyframe>>* keyframeRuleList; 
/* Line 387 of yacc.c  */
#line 210 "./CSSGrammar.y"
 CSSPropertyID id; 
/* Line 387 of yacc.c  */
#line 212 "./CSSGrammar.y"
 CSSParserSelector* selector; 
/* Line 387 of yacc.c  */
#line 215 "./CSSGrammar.y"
 Vector<OwnPtr<CSSParserSelector>>* selectorList; 
/* Line 387 of yacc.c  */
#line 218 "./CSSGrammar.y"
 bool boolean; 
/* Line 387 of yacc.c  */
#line 220 "./CSSGrammar.y"
 CSSSelector::Match match; 
/* Line 387 of yacc.c  */
#line 222 "./CSSGrammar.y"
 int integer; 
/* Line 387 of yacc.c  */
#line 224 "./CSSGrammar.y"
 char character; 
/* Line 387 of yacc.c  */
#line 226 "./CSSGrammar.y"
 CSSParserValueList* valueList; 
/* Line 387 of yacc.c  */
#line 231 "./CSSGrammar.y"
 CSSParser::Location location; 

/* Line 387 of yacc.c  */
#line 340 "./CSSGrammar.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int cssyyparse (void *YYPARSE_PARAM);
#else
int cssyyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int cssyyparse (CSSParser* parser);
#else
int cssyyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_CSSYY_CSSGRAMMAR_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */
/* Line 390 of yacc.c  */
#line 71 "./CSSGrammar.y"

static inline int cssyyerror(void*, const char*)
{
    return 1;
}

/* Line 390 of yacc.c  */
#line 374 "./CSSGrammar.cpp"

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
#define YYFINAL  21
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1719

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  115
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  331
/* YYNRULES -- Number of states.  */
#define YYNSTATES  595

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   349

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,   113,     2,   114,     2,     2,
     104,   102,    20,   106,   105,   109,    18,   112,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    17,   103,
       2,   111,   108,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    19,     2,   110,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   100,    21,   101,   107,     2,     2,     2,
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
      15,    16,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     8,    11,    14,    17,    20,    23,    26,
      33,    40,    46,    52,    58,    64,    65,    68,    69,    72,
      75,    76,    78,    80,    82,    84,    86,    92,    96,   100,
     106,   110,   111,   115,   117,   119,   121,   123,   125,   127,
     129,   131,   133,   135,   137,   139,   140,   144,   145,   149,
     151,   153,   155,   157,   159,   161,   163,   165,   167,   169,
     171,   173,   174,   182,   190,   198,   203,   208,   215,   222,
     226,   230,   231,   234,   236,   238,   239,   244,   254,   256,
     262,   263,   267,   268,   270,   272,   274,   280,   281,   283,
     285,   290,   293,   294,   295,   297,   308,   317,   322,   323,
     334,   336,   338,   339,   343,   350,   352,   358,   361,   363,
     365,   366,   377,   382,   387,   389,   392,   394,   395,   397,
     402,   403,   411,   413,   415,   417,   419,   421,   423,   425,
     427,   429,   431,   433,   435,   437,   439,   441,   443,   444,
     453,   458,   463,   464,   475,   478,   481,   484,   486,   487,
     489,   491,   493,   494,   495,   496,   506,   507,   509,   516,
     519,   521,   524,   528,   532,   535,   537,   540,   543,   545,
     548,   550,   553,   557,   560,   562,   568,   571,   573,   575,
     577,   580,   583,   585,   587,   589,   591,   593,   596,   602,
     612,   619,   630,   632,   634,   636,   638,   640,   642,   644,
     646,   649,   652,   656,   663,   670,   678,   685,   692,   693,
     695,   698,   700,   702,   705,   709,   713,   718,   723,   727,
     734,   740,   743,   750,   754,   759,   763,   766,   769,   770,
     772,   775,   777,   781,   785,   788,   791,   792,   795,   799,
     802,   805,   808,   812,   815,   818,   821,   824,   830,   833,
     836,   839,   842,   844,   846,   848,   850,   852,   854,   856,
     858,   860,   862,   864,   866,   868,   870,   872,   874,   876,
     878,   880,   882,   884,   886,   888,   890,   892,   894,   896,
     898,   900,   902,   907,   911,   916,   918,   923,   926,   930,
     934,   938,   942,   943,   945,   951,   953,   956,   958,   962,
     966,   968,   971,   977,   983,   988,   990,   992,   997,  1002,
    1004,  1007,  1011,  1015,  1018,  1022,  1026,  1030,  1034,  1036,
    1038,  1040,  1042,  1044,  1046,  1048,  1050,  1051,  1052,  1055,
    1058,  1061
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     116,     0,    -1,   123,   125,   124,   130,    -1,   117,   123,
      -1,   119,   123,    -1,   120,   123,    -1,   121,   123,    -1,
     122,   123,    -1,   118,   123,    -1,    29,   100,   123,   131,
     123,   101,    -1,    31,   100,   123,   159,   123,   101,    -1,
      30,   100,   176,   196,   101,    -1,    33,   100,   123,   203,
     101,    -1,    34,     5,   123,   148,   101,    -1,    35,   100,
     123,   182,   101,    -1,    -1,   123,     5,    -1,    -1,   124,
       6,    -1,   124,     5,    -1,    -1,   128,    -1,   101,    -1,
       0,    -1,   102,    -1,     0,    -1,    26,   123,    12,   123,
     103,    -1,    26,     1,   223,    -1,    26,     1,   103,    -1,
      26,   123,    12,   123,   103,    -1,    26,   123,   103,    -1,
      -1,   130,   132,   124,    -1,   180,    -1,   154,    -1,   163,
      -1,   170,    -1,   156,    -1,   140,    -1,   139,    -1,   172,
      -1,   131,    -1,   129,    -1,   222,    -1,   221,    -1,    -1,
     133,   136,   124,    -1,    -1,   134,   135,   124,    -1,   180,
      -1,   163,    -1,   170,    -1,   154,    -1,   156,    -1,   135,
      -1,   222,    -1,   221,    -1,   140,    -1,   139,    -1,   172,
      -1,   123,    -1,    -1,   138,    22,   137,   142,   123,   149,
     103,    -1,   138,    22,   137,   142,   123,   149,     0,    -1,
     138,    22,   137,   142,   123,   149,   223,    -1,   138,    22,
       1,   103,    -1,   138,    22,     1,   223,    -1,    27,   123,
     141,   142,   123,   103,    -1,    27,   123,   141,   142,   123,
     223,    -1,    27,     1,   223,    -1,    27,     1,   103,    -1,
      -1,    13,   123,    -1,    12,    -1,    91,    -1,    -1,    17,
     123,   203,   123,    -1,   147,   123,   104,   123,    13,   123,
     143,   102,   123,    -1,   144,    -1,   145,   123,    58,   123,
     144,    -1,    -1,    58,   123,   145,    -1,    -1,    56,    -1,
      57,    -1,   145,    -1,   147,   123,    13,   123,   146,    -1,
      -1,   150,    -1,   148,    -1,   150,   105,   123,   148,    -1,
     150,     1,    -1,    -1,    -1,   123,    -1,   152,    24,   123,
     150,   178,   100,   151,   123,   133,   220,    -1,   152,    24,
     153,   100,   151,   123,   133,   220,    -1,   152,    24,   153,
     103,    -1,    -1,   155,    32,   123,   157,   153,   100,   151,
     123,   158,   126,    -1,    13,    -1,    12,    -1,    -1,   158,
     159,   123,    -1,   160,   123,   100,   123,   196,   126,    -1,
     161,    -1,   160,   123,   105,   123,   161,    -1,   174,    80,
      -1,    13,    -1,     1,    -1,    -1,   162,    23,   123,   164,
     153,   100,   151,   176,   165,   126,    -1,   162,    23,     1,
     223,    -1,   162,    23,     1,   103,    -1,    13,    -1,    13,
     194,    -1,   194,    -1,    -1,   196,    -1,   165,   166,   123,
     196,    -1,    -1,   168,   167,   123,   100,   123,   196,   126,
      -1,    38,    -1,    39,    -1,    40,    -1,    41,    -1,    42,
      -1,    43,    -1,    44,    -1,    45,    -1,    46,    -1,    47,
      -1,    48,    -1,    49,    -1,    50,    -1,    51,    -1,    52,
      -1,    53,    -1,    -1,   169,    25,   153,   100,   151,   176,
     196,   126,    -1,   169,    25,     1,   223,    -1,   169,    25,
       1,   103,    -1,    -1,   171,    36,     5,   182,   178,   100,
     151,   123,   134,   220,    -1,   106,   123,    -1,   107,   123,
      -1,   108,   123,    -1,   175,    -1,    -1,   109,    -1,   106,
      -1,   123,    -1,    -1,    -1,    -1,   177,   182,   179,   178,
     100,   151,   176,   196,   126,    -1,    -1,   183,    -1,   182,
     179,   105,   123,   181,   183,    -1,   182,     1,    -1,   185,
      -1,   183,     5,    -1,   183,     5,   185,    -1,   183,   173,
     185,    -1,   183,     1,    -1,    21,    -1,    20,    21,    -1,
      13,    21,    -1,   187,    -1,   187,   188,    -1,   188,    -1,
     184,   187,    -1,   184,   187,   188,    -1,   184,   188,    -1,
     185,    -1,   186,   123,   105,   123,   185,    -1,   186,     1,
      -1,    13,    -1,    20,    -1,   189,    -1,   188,   189,    -1,
     188,     1,    -1,    16,    -1,    15,    -1,   190,    -1,   191,
      -1,   195,    -1,    18,    13,    -1,    19,   123,    13,   123,
     110,    -1,    19,   123,    13,   123,   192,   123,   193,   123,
     110,    -1,    19,   123,   184,    13,   123,   110,    -1,    19,
     123,   184,    13,   123,   192,   123,   193,   123,   110,    -1,
     111,    -1,     7,    -1,     8,    -1,     9,    -1,    10,    -1,
      11,    -1,    13,    -1,    12,    -1,    17,    13,    -1,    17,
      13,    -1,    17,    17,    13,    -1,    17,    93,   123,   186,
     123,   102,    -1,    17,    92,   123,    14,   123,   102,    -1,
      17,    92,   123,   174,    82,   123,   102,    -1,    17,    92,
     123,    13,   123,   102,    -1,    17,    94,   123,   185,   123,
     102,    -1,    -1,   199,    -1,   197,   199,    -1,   197,    -1,
     198,    -1,   197,   198,    -1,   199,   103,   123,    -1,   198,
     103,   123,    -1,   197,   199,   103,   123,    -1,   197,   198,
     103,   123,    -1,     1,   227,   228,    -1,    98,   123,    17,
     123,   203,   202,    -1,   201,    17,   123,   203,   202,    -1,
     201,   200,    -1,   201,    17,   123,   203,   202,   200,    -1,
      55,   123,   200,    -1,   201,    17,   123,   200,    -1,     1,
     227,   228,    -1,    13,   123,    -1,    55,   123,    -1,    -1,
     204,    -1,   204,   205,    -1,   207,    -1,   204,   206,   207,
      -1,     1,   227,   228,    -1,   112,   123,    -1,   105,   123,
      -1,    -1,   208,   123,    -1,   175,   208,   123,    -1,    12,
     123,    -1,    13,   123,    -1,    78,   123,    -1,   175,    78,
     123,    -1,    91,   123,    -1,    99,   123,    -1,    15,   123,
      -1,   113,   123,    -1,    28,   123,    13,   127,   123,    -1,
     209,   123,    -1,   217,   123,    -1,   219,   123,    -1,   114,
     123,    -1,    82,    -1,    81,    -1,    80,    -1,    64,    -1,
      65,    -1,    66,    -1,    67,    -1,    68,    -1,    69,    -1,
      70,    -1,    71,    -1,    72,    -1,    73,    -1,    74,    -1,
      75,    -1,    76,    -1,    77,    -1,    62,    -1,    61,    -1,
      63,    -1,    59,    -1,    60,    -1,    83,    -1,    84,    -1,
      85,    -1,    86,    -1,    87,    -1,    88,    -1,    89,    -1,
      90,    -1,    92,   123,   203,   127,    -1,    92,   123,   127,
      -1,    92,   123,   205,   127,    -1,   208,    -1,    28,   123,
      13,   127,    -1,   175,   208,    -1,     5,   106,     5,    -1,
       5,   109,     5,    -1,   212,    20,   123,    -1,   212,   112,
     123,    -1,    -1,     5,    -1,   104,   123,   214,   212,   127,
      -1,   215,    -1,   215,   205,    -1,   210,    -1,   214,   211,
     210,    -1,   214,   211,   213,    -1,   213,    -1,   214,   212,
      -1,   216,   105,   123,   214,   212,    -1,    95,   123,   214,
     212,   127,    -1,    95,   123,   205,   127,    -1,    96,    -1,
      97,    -1,   218,   123,   216,   127,    -1,   218,   123,   205,
     127,    -1,   126,    -1,     1,   126,    -1,    54,     1,   223,
      -1,    54,     1,   103,    -1,     1,   223,    -1,   100,   228,
     126,    -1,    19,   228,   110,    -1,    19,   228,     0,    -1,
     226,   228,   127,    -1,   104,    -1,    92,    -1,    95,    -1,
      28,    -1,    96,    -1,    97,    -1,    93,    -1,    94,    -1,
      -1,    -1,   228,     1,    -1,   228,   223,    -1,   228,   224,
      -1,   228,   225,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   235,   235,   236,   237,   238,   239,   240,   241,   243,
     244,   245,   247,   257,   259,   267,   267,   268,   268,   268,
     269,   269,   270,   270,   271,   271,   273,   279,   280,   282,
     282,   283,   284,   292,   293,   294,   295,   296,   297,   298,
     299,   302,   306,   307,   308,   311,   312,   322,   323,   333,
     334,   335,   336,   337,   339,   339,   339,   339,   339,   339,
     341,   347,   352,   355,   358,   364,   368,   374,   375,   376,
     377,   379,   379,   380,   380,   381,   381,   383,   394,   398,
     404,   407,   412,   415,   418,   423,   427,   432,   432,   434,
     439,   447,   454,   459,   464,   469,   472,   475,   481,   486,
     490,   490,   492,   493,   499,   501,   505,   513,   514,   526,
     531,   536,   546,   550,   556,   560,   567,   572,   577,   577,
     579,   579,   586,   589,   592,   595,   598,   601,   604,   607,
     610,   613,   616,   619,   622,   625,   628,   631,   636,   641,
     644,   648,   654,   659,   670,   671,   672,   674,   674,   675,
     675,   676,   678,   683,   684,   686,   691,   693,   701,   711,
     717,   718,   719,   728,   737,   743,   744,   745,   748,   751,
     756,   761,   764,   769,   776,   783,   792,   798,   803,   809,
     810,   817,   823,   830,   841,   842,   843,   846,   855,   860,
     866,   871,   879,   882,   885,   888,   891,   894,   898,   898,
     900,   910,   919,   928,   940,   949,   958,   976,   991,   992,
     993,   994,   995,   996,   999,  1003,  1007,  1013,  1019,  1024,
    1028,  1044,  1045,  1050,  1053,  1058,  1059,  1060,  1060,  1061,
    1061,  1063,  1067,  1083,  1084,  1084,  1084,  1086,  1087,  1088,
    1089,  1094,  1095,  1096,  1097,  1098,  1099,  1100,  1104,  1105,
    1106,  1107,  1112,  1113,  1114,  1115,  1116,  1117,  1118,  1119,
    1120,  1121,  1122,  1123,  1124,  1125,  1126,  1127,  1128,  1129,
    1130,  1131,  1132,  1139,  1140,  1141,  1142,  1143,  1144,  1145,
    1146,  1147,  1150,  1158,  1166,  1176,  1177,  1181,  1184,  1187,
    1190,  1193,  1197,  1197,  1199,  1213,  1213,  1215,  1219,  1234,
    1248,  1251,  1252,  1268,  1276,  1282,  1282,  1284,  1292,  1298,
    1298,  1299,  1299,  1300,  1301,  1302,  1302,  1303,  1305,  1305,
    1305,  1305,  1305,  1305,  1305,  1305,  1307,  1308,  1309,  1310,
    1311,  1312
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "TOKEN_EOF", "error", "$undefined", "LOWEST_PREC", "UNIMPORTANT_TOK",
  "WHITESPACE", "SGML_CD", "INCLUDES", "DASHMATCH", "BEGINSWITH",
  "ENDSWITH", "CONTAINS", "STRING", "IDENT", "NTH", "HEX", "IDSEL", "':'",
  "'.'", "'['", "'*'", "'|'", "IMPORT_SYM", "PAGE_SYM", "MEDIA_SYM",
  "FONT_FACE_SYM", "CHARSET_SYM", "NAMESPACE_SYM", "VARFUNCTION",
  "WEBKIT_RULE_SYM", "WEBKIT_DECLS_SYM", "WEBKIT_KEYFRAME_RULE_SYM",
  "WEBKIT_KEYFRAMES_SYM", "WEBKIT_VALUE_SYM", "WEBKIT_MEDIAQUERY_SYM",
  "WEBKIT_SELECTOR_SYM", "WEBKIT_REGION_RULE_SYM",
  "WEBKIT_VIEWPORT_RULE_SYM", "TOPLEFTCORNER_SYM", "TOPLEFT_SYM",
  "TOPCENTER_SYM", "TOPRIGHT_SYM", "TOPRIGHTCORNER_SYM",
  "BOTTOMLEFTCORNER_SYM", "BOTTOMLEFT_SYM", "BOTTOMCENTER_SYM",
  "BOTTOMRIGHT_SYM", "BOTTOMRIGHTCORNER_SYM", "LEFTTOP_SYM",
  "LEFTMIDDLE_SYM", "LEFTBOTTOM_SYM", "RIGHTTOP_SYM", "RIGHTMIDDLE_SYM",
  "RIGHTBOTTOM_SYM", "ATKEYWORD", "IMPORTANT_SYM", "MEDIA_ONLY",
  "MEDIA_NOT", "MEDIA_AND", "REMS", "CHS", "QEMS", "EMS", "EXS", "PXS",
  "CMS", "MMS", "INS", "PTS", "PCS", "DEGS", "RADS", "GRADS", "TURNS",
  "MSECS", "SECS", "HERTZ", "KHERTZ", "DIMEN", "INVALIDDIMEN",
  "PERCENTAGE", "FLOATTOKEN", "INTEGER", "VW", "VH", "VMIN", "VMAX",
  "DPPX", "DPI", "DPCM", "FR", "URI", "FUNCTION", "ANYFUNCTION",
  "NOTFUNCTION", "CALCFUNCTION", "MINFUNCTION", "MAXFUNCTION",
  "VAR_DEFINITION", "UNICODERANGE", "'{'", "'}'", "')'", "';'", "'('",
  "','", "'+'", "'~'", "'>'", "'-'", "']'", "'='", "'/'", "'#'", "'%'",
  "$accept", "stylesheet", "webkit_rule", "webkit_keyframe_rule",
  "webkit_decls", "webkit_value", "webkit_mediaquery", "webkit_selector",
  "maybe_space", "maybe_sgml", "maybe_charset", "closing_brace",
  "closing_parenthesis", "charset", "ignored_charset", "rule_list",
  "valid_rule", "rule", "block_rule_list", "block_valid_rule_list",
  "block_valid_rule", "block_rule", "at_import_header_end_maybe_space",
  "before_import_rule", "import", "namespace", "maybe_ns_prefix",
  "string_or_uri", "maybe_media_value", "media_query_exp",
  "media_query_exp_list", "maybe_and_media_query_exp_list",
  "maybe_media_restrictor", "media_query", "maybe_media_list",
  "media_list", "at_rule_body_start", "before_media_rule",
  "at_rule_header_end_maybe_space", "media", "before_keyframes_rule",
  "keyframes", "keyframe_name", "keyframes_rule", "keyframe_rule",
  "key_list", "key", "before_page_rule", "page", "page_selector",
  "declarations_and_margins", "margin_box", "$@1", "margin_sym",
  "before_font_face_rule", "font_face", "before_region_rule", "region",
  "combinator", "maybe_unary_operator", "unary_operator",
  "maybe_space_before_declaration", "before_selector_list",
  "at_rule_header_end", "at_selector_end", "ruleset",
  "before_selector_group_item", "selector_list", "selector",
  "namespace_selector", "simple_selector", "simple_selector_list",
  "element_name", "specifier_list", "specifier", "class", "attrib",
  "match", "ident_or_string", "pseudo_page", "pseudo", "declaration_list",
  "decl_list", "decl_list_recovery", "declaration", "declaration_recovery",
  "property", "priority", "expr", "valid_expr", "expr_recovery",
  "operator", "term", "unary_term", "function", "calc_func_term",
  "calc_func_operator", "calc_maybe_space", "calc_func_paren_expr",
  "calc_func_expr", "valid_calc_func_expr", "calc_func_expr_list",
  "calc_function", "min_or_max", "min_or_max_function", "save_block",
  "invalid_at", "invalid_rule", "invalid_block",
  "invalid_square_brackets_block", "invalid_parentheses_block",
  "opening_parenthesis", "error_location", "error_recovery", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,    58,    46,    91,
      42,   124,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     123,   125,    41,    59,    40,    44,    43,   126,    62,    45,
      93,    61,    47,    35,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   115,   116,   116,   116,   116,   116,   116,   116,   117,
     118,   119,   120,   121,   122,   123,   123,   124,   124,   124,
     125,   125,   126,   126,   127,   127,   128,   128,   128,   129,
     129,   130,   130,   131,   131,   131,   131,   131,   131,   131,
     131,   132,   132,   132,   132,   133,   133,   134,   134,   135,
     135,   135,   135,   135,   136,   136,   136,   136,   136,   136,
     137,   138,   139,   139,   139,   139,   139,   140,   140,   140,
     140,   141,   141,   142,   142,   143,   143,   144,   145,   145,
     146,   146,   147,   147,   147,   148,   148,   149,   149,   150,
     150,   150,   151,   152,   153,   154,   154,   154,   155,   156,
     157,   157,   158,   158,   159,   160,   160,   161,   161,   161,
     162,   163,   163,   163,   164,   164,   164,   164,   165,   165,
     167,   166,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   169,   170,
     170,   170,   171,   172,   173,   173,   173,   174,   174,   175,
     175,   176,   177,   178,   179,   180,   181,   182,   182,   182,
     183,   183,   183,   183,   183,   184,   184,   184,   185,   185,
     185,   185,   185,   185,   186,   186,   186,   187,   187,   188,
     188,   188,   189,   189,   189,   189,   189,   190,   191,   191,
     191,   191,   192,   192,   192,   192,   192,   192,   193,   193,
     194,   195,   195,   195,   195,   195,   195,   195,   196,   196,
     196,   196,   196,   196,   197,   197,   197,   197,   198,   199,
     199,   199,   199,   199,   199,   200,   201,   202,   202,   203,
     203,   204,   204,   205,   206,   206,   206,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   209,   209,   209,   210,   210,   210,   211,   211,
     211,   211,   212,   212,   213,   214,   214,   215,   215,   215,
     215,   216,   216,   217,   217,   218,   218,   219,   219,   220,
     220,   221,   221,   222,   223,   224,   224,   225,   226,   226,
     226,   226,   226,   226,   226,   226,   227,   228,   228,   228,
     228,   228
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     2,     2,     2,     2,     2,     2,     6,
       6,     5,     5,     5,     5,     0,     2,     0,     2,     2,
       0,     1,     1,     1,     1,     1,     5,     3,     3,     5,
       3,     0,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     3,     0,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     7,     7,     7,     4,     4,     6,     6,     3,
       3,     0,     2,     1,     1,     0,     4,     9,     1,     5,
       0,     3,     0,     1,     1,     1,     5,     0,     1,     1,
       4,     2,     0,     0,     1,    10,     8,     4,     0,    10,
       1,     1,     0,     3,     6,     1,     5,     2,     1,     1,
       0,    10,     4,     4,     1,     2,     1,     0,     1,     4,
       0,     7,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     8,
       4,     4,     0,    10,     2,     2,     2,     1,     0,     1,
       1,     1,     0,     0,     0,     9,     0,     1,     6,     2,
       1,     2,     3,     3,     2,     1,     2,     2,     1,     2,
       1,     2,     3,     2,     1,     5,     2,     1,     1,     1,
       2,     2,     1,     1,     1,     1,     1,     2,     5,     9,
       6,    10,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     3,     6,     6,     7,     6,     6,     0,     1,
       2,     1,     1,     2,     3,     3,     4,     4,     3,     6,
       5,     2,     6,     3,     4,     3,     2,     2,     0,     1,
       2,     1,     3,     3,     2,     2,     0,     2,     3,     2,
       2,     2,     3,     2,     2,     2,     2,     5,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     3,     4,     1,     4,     2,     3,     3,
       3,     3,     0,     1,     5,     1,     2,     1,     3,     3,
       1,     2,     5,     5,     4,     1,     1,     4,     4,     1,
       2,     3,     3,     2,     3,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     0,     2,     2,
       2,     2
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      15,     0,     0,     0,     0,     0,     0,     0,    15,    15,
      15,    15,    15,    15,    20,    15,    15,    15,    15,    15,
      15,     1,     3,     8,     4,     5,     6,     7,    16,     0,
      17,    21,   152,   151,     0,     0,     0,    82,     0,     0,
       0,    31,     0,    15,     0,    39,    38,     0,    34,     0,
      37,     0,    35,     0,    36,     0,    40,     0,    33,   326,
      15,    15,    15,     0,     0,   212,   209,     0,   109,   108,
     150,   149,    15,    15,   105,     0,   147,    15,    15,    15,
      15,   272,   273,   270,   269,   271,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
      15,   254,   253,   252,   274,   275,   276,   277,   278,   279,
     280,   281,    15,    15,    15,   305,   306,    15,    15,    15,
       0,     0,     0,   231,    15,    15,    15,    15,    15,    83,
      84,    78,    85,    15,     0,   177,   183,   182,     0,     0,
      15,   178,   165,     0,     0,     0,   160,   168,     0,   179,
     184,   185,   186,   327,    28,    27,    15,    19,    18,     0,
       0,    71,     0,     0,    15,    15,     0,     0,     0,     0,
     327,   226,     0,     0,    11,   213,   210,    15,    15,   326,
      15,   221,     0,     0,   107,   239,   240,   245,     0,   241,
     243,     0,     0,   244,   246,   251,    15,    15,    12,   326,
      15,    15,   230,     0,   237,   248,   249,     0,   250,     0,
       0,    13,   167,   201,     0,    15,    15,    15,   187,     0,
     166,   159,    14,     0,   164,   161,    15,    15,    15,     0,
     177,   178,   171,     0,     0,   181,   180,     0,     0,     0,
      15,     0,    42,    41,    17,    44,    43,    70,    69,    15,
       0,     9,     0,    60,     0,    82,     0,     0,     0,   117,
       0,    94,     0,     0,   153,     0,   223,    15,    15,    15,
     215,   214,   327,     0,    10,    15,    15,     0,    25,    24,
     283,     0,     0,    15,    15,     0,     0,   285,   297,   300,
     292,     0,   242,   238,   327,   235,   234,   232,     0,   292,
       0,    15,    15,    15,   202,   148,     0,     0,    15,     0,
       0,    15,   162,   144,   145,   146,   163,     0,    23,   328,
     327,   321,   319,   324,   325,   320,   322,   323,    22,   318,
     314,   329,   330,   331,   327,    26,   313,     0,     0,    32,
      72,    73,    74,    15,    65,    66,    15,    89,     0,    92,
      97,   101,   100,    15,   113,   112,   114,     0,    15,   116,
     141,   140,    92,     0,     0,     0,   217,   216,     0,   224,
     228,     0,     0,    15,   282,   284,     0,     0,   287,   304,
     293,     0,     0,   296,   233,   308,   301,    15,   307,    82,
      80,     0,    15,    15,     0,   174,     0,    15,     0,    15,
     156,     0,     0,    15,    30,   312,   311,     0,    87,    91,
      15,     0,    15,     0,   115,   200,     0,    15,     0,    92,
     228,    15,     0,     0,   106,   247,     0,   292,     0,     0,
     298,   299,    15,    15,   303,     0,    79,    15,    15,    86,
      15,     0,     0,    15,   176,     0,     0,   193,   194,   195,
     196,   197,   188,   192,    15,     0,     0,   316,   315,   317,
       0,    67,    68,     0,     0,    82,    92,    45,    92,    92,
       0,    92,    15,   219,   227,   222,   104,   286,     0,   288,
     289,   290,   291,   292,     0,    82,    75,   206,   204,     0,
     203,    15,   207,     0,   190,    15,     0,    29,    63,    62,
      64,    90,    15,     0,    15,    15,     0,    15,     0,   294,
     302,    81,    15,     0,   205,     0,   199,   198,    15,     0,
      45,     0,   309,    54,    17,    58,    57,    52,    53,    50,
      51,    59,    49,    96,    56,    55,   102,     0,   139,    47,
       0,     0,    15,   175,     0,    15,     0,   310,    46,     0,
       0,   118,     0,   155,    15,    77,   189,     0,    95,    99,
      15,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   111,    15,   120,
       0,    17,   143,    76,   191,   103,     0,    15,    48,   119,
       0,    15,     0,     0,   121
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     7,     8,     9,    10,    11,    12,    13,    33,    41,
      30,   522,   280,    31,   242,   159,    43,   244,   503,   552,
     523,   524,   254,    44,    45,    46,   250,   343,   513,   131,
     132,   439,   133,   347,   463,   348,   412,    47,   256,   527,
      49,   528,   353,   549,    72,    73,    74,    51,   529,   358,
     550,   578,   587,   579,    53,   530,    55,    56,   229,    75,
     120,    34,    57,   364,   223,   532,   456,   143,   144,   145,
     146,   396,   147,   148,   149,   150,   151,   454,   518,   359,
     152,    63,    64,    65,    66,   181,    67,   422,   121,   122,
     202,   203,   123,   124,   125,   288,   381,   382,   289,   290,
     291,   300,   126,   127,   128,   533,   534,   535,   331,   332,
     333,   334,   170,   237
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -456
static const yytype_int16 yypact[] =
{
     654,   -43,   -11,   120,   124,   242,   187,   300,  -456,  -456,
    -456,  -456,  -456,  -456,    58,  -456,  -456,  -456,  -456,  -456,
    -456,  -456,   305,   305,   305,   305,   305,   305,  -456,   496,
    -456,  -456,   514,   305,   244,   129,   857,   385,   974,    -1,
     491,   460,   280,  -456,   319,  -456,  -456,   326,  -456,   368,
    -456,   383,  -456,   402,  -456,   425,  -456,   802,  -456,  -456,
    -456,  -456,  -456,   380,  1305,   382,   387,    64,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,   419,  -456,  -456,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
    1597,   399,   556,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
    -456,  -456,    35,  -456,   447,   494,  -456,  -456,   794,   536,
    -456,   532,  -456,   138,   220,   887,  -456,   658,   318,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  1601,
     410,    63,   258,   393,  -456,  -456,   475,   249,   550,    70,
    -456,   305,    65,   364,  -456,   478,   479,  -456,  -456,  -456,
    -456,  -456,   260,   224,  -456,   305,   305,   305,   261,   305,
     305,   666,   950,   305,   305,   305,  -456,  -456,  -456,  -456,
    -456,  -456,  -456,  1185,   305,   305,   305,   950,   305,    51,
      46,  -456,  -456,  -456,   560,  -456,  -456,  -456,  -456,   473,
    -456,  -456,  -456,   481,  -456,   802,  -456,  -456,  -456,   802,
    -456,  -456,   658,   367,   685,  -456,  -456,   880,   227,   488,
    -456,   588,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
      19,  -456,   414,   305,    19,   406,   440,   292,   444,   417,
     476,   305,   490,   802,   481,  1092,  -456,  -456,  -456,  -456,
     305,   305,  -456,   769,  -456,  -456,  -456,    47,  -456,  -456,
    -456,    47,    47,  -456,  -456,  1629,    47,  -456,  -456,  -456,
     587,    49,   305,   305,  -456,   305,   305,  -456,    47,   587,
      97,  -456,  -456,  -456,  -456,   248,   974,   974,   494,   532,
     580,  -456,  -456,   305,   305,   305,  -456,   694,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,  -456,   111,   477,   460,
     305,  -456,  -456,  -456,  -456,  -456,  -456,  -456,   146,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,   593,   599,  -456,  -456,
    -456,  -456,  -456,   186,   513,   857,   305,   305,  1127,  -456,
     559,   277,   129,  -456,  -456,  -456,   301,  1001,  -456,  -456,
     295,  1494,    74,  -456,   797,  -456,   228,  -456,  -456,   385,
      53,   474,  -456,  -456,   553,  -456,    87,  -456,   441,  -456,
     305,   270,   470,  -456,  -456,  -456,  -456,   103,   667,  -456,
    -456,   549,  -456,   554,  -456,  -456,   563,  -456,   564,  -456,
     559,  -456,   482,    28,  -456,   305,    47,   587,   645,   651,
    -456,  -456,  -456,  -456,  -456,  1001,  -456,  -456,  -456,  -456,
    -456,   213,   229,  -456,  -456,   117,   236,  -456,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,   448,   802,  -456,  -456,  -456,
     253,  -456,  -456,    38,   105,   385,  -456,   305,  -456,  -456,
     705,  -456,  -456,  -456,   305,  -456,  -456,  -456,    74,  -456,
    -456,   305,   305,   587,   126,   385,   390,  -456,  -456,   250,
    -456,  -456,  -456,   423,  -456,  -456,   308,  -456,  -456,  -456,
    -456,  -456,  -456,  1425,  -456,  -456,    28,  -456,   705,  -456,
     228,    35,  -456,   558,  -456,   974,  -456,  -456,  -456,   423,
     305,   195,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,   305,  1369,  -456,   305,
      28,   857,  -456,  -456,    25,  -456,  1425,  -456,   460,    82,
    1451,  -456,  1512,  -456,  -456,   305,  -456,    59,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
    -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,
      28,  -456,  -456,   305,  -456,   305,  1287,  -456,   460,  -456,
      93,  -456,   277,    28,  -456
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -456,  -456,  -456,  -456,  -456,  -456,  -456,  -456,     0,  -230,
    -456,  -233,  -245,  -456,  -456,  -456,   523,  -456,   171,  -456,
     140,  -456,  -456,  -456,  -455,  -115,  -456,   439,  -456,   307,
     212,  -456,  -109,   -36,  -456,   290,  -323,  -456,  -146,    -7,
    -456,     1,  -456,  -456,   158,  -456,   336,  -456,    12,  -456,
    -456,  -456,  -456,  -456,  -456,    13,  -456,   -86,  -456,   409,
     -33,   -28,  -456,  -296,   546,    14,  -456,   -50,   263,   497,
    -203,  -456,   572,   232,  -113,  -456,  -456,   265,   202,   366,
    -456,  -355,  -456,   681,   695,  -166,  -456,   344,  -186,  -456,
    -106,  -456,   566,  -117,  -456,   386,  -456,  -272,   392,  -183,
    -456,  -456,  -456,  -456,  -456,  -107,   607,   617,   -16,  -456,
    -456,  -456,    80,  -136
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -296
static const yytype_int16 yytable[] =
{
      14,   134,    76,   197,   330,   281,   266,   169,    22,    23,
      24,    25,    26,    27,   339,    32,   423,    35,    36,    37,
      38,   262,   312,   155,   299,    48,   316,   386,   318,    40,
      28,   341,   373,    50,   265,   236,   374,   375,   498,   417,
     -15,   379,   161,   162,    52,    54,    58,   278,   525,  -295,
     199,    28,   411,   385,  -295,   388,    28,    15,    28,   302,
     171,   172,   173,    28,    28,   179,   179,   418,    28,  -295,
      28,   221,   182,   183,   278,   287,   249,   185,   186,   187,
     188,   180,   318,    68,    29,   282,   286,   370,   444,    16,
     287,   525,   -15,   -15,   432,    69,   472,   278,    28,   153,
     189,   298,   154,   395,   397,   -88,   409,   369,    28,   301,
     342,   438,   190,   191,   192,   506,    28,   193,   194,   195,
     236,   236,    28,   403,   204,   205,   206,   207,   208,   328,
      68,    28,   209,   210,    28,   556,   368,   434,   153,   221,
     219,   499,    69,   502,   248,   504,   505,   409,   507,   279,
     303,  -295,    48,   540,  -295,   478,   238,   459,   384,   285,
      50,  -295,  -148,   253,   255,   257,   259,   261,   378,   584,
    -154,    52,    54,    58,   285,  -154,   279,   270,   271,   420,
     273,   477,   551,   328,   401,   383,   433,   221,    70,   -15,
     476,    71,   -15,   591,   427,   318,   292,   293,   402,   279,
     295,   296,   387,   153,   236,   -88,   461,   413,   -88,  -148,
     410,   510,   416,   363,   404,   305,   306,   307,    28,   490,
      17,   224,   491,   336,    18,   225,   313,   314,   315,    28,
     303,   589,    28,   509,    28,    70,   345,   593,    71,   222,
     337,    28,   355,  -154,   361,    59,  -153,    19,   432,   340,
     260,   410,   483,    28,   -15,    28,   475,    60,    28,   272,
     287,   392,   393,    28,   287,    28,    28,   365,   366,   367,
     457,   319,    76,   538,   277,   371,   372,  -208,    59,   294,
     437,   160,    28,   376,   377,   -15,  -153,    20,   547,   320,
      60,  -154,   -15,   -15,   548,   153,   328,    28,   321,    61,
      21,   389,   390,   391,   351,   352,    28,   553,   398,   224,
      28,   400,   543,   225,   426,   487,   559,   577,   287,   235,
    -157,  -157,   406,  -170,   275,  -157,   226,   227,   228,   276,
     335,   488,    61,   136,   137,   138,   139,   140,   492,    76,
     433,   163,    62,   407,   285,  -208,   408,   547,   285,   -15,
     164,   588,   514,   261,    70,   554,   497,    71,   261,   251,
     594,   274,   322,   323,   324,   325,   326,   327,   235,    28,
     153,   -15,  -173,   425,   329,    62,   437,   233,  -208,   234,
     458,   267,   136,   137,   138,   139,   140,   435,   526,   470,
      28,   462,   441,   442,   252,    28,   445,   446,   -15,   455,
     165,   428,   285,   460,   429,   -15,   166,   512,  -158,  -158,
     465,    28,   467,  -158,   226,   227,   228,   531,  -170,  -170,
    -170,   474,    28,  -170,  -170,  -170,  -170,   167,    28,   501,
     356,   526,   481,   482,   357,   516,   517,   484,   485,   558,
     486,   129,   130,   489,   508,   582,    28,   500,   447,   448,
     449,   450,   451,    28,   493,   447,   448,   449,   450,   451,
     531,   168,   129,   130,   317,   157,   158,  -173,  -173,  -173,
     278,   319,  -173,  -173,  -173,  -173,   258,   537,    28,    28,
     -15,   174,  -220,   179,   -15,   177,   308,   440,   -15,   320,
     178,   515,   -15,   309,   142,   519,    28,    39,   321,   184,
     198,   -15,   520,   156,   536,   336,   -94,   539,   -15,   -94,
     153,   209,   541,   247,   153,   212,    76,   344,   544,    28,
    -220,  -220,  -220,  -220,  -220,  -220,  -220,  -220,  -220,  -220,
    -220,  -220,  -220,  -220,  -220,  -220,   -61,  -110,   -93,  -138,
     349,    42,   555,   350,   153,   557,   -98,   354,   211,   218,
    -142,   452,   453,   220,   583,   263,  -229,   199,   494,   453,
     585,  -229,   322,   323,   324,   325,   326,   327,  -236,  -236,
     153,  -236,   279,   304,   329,   -15,   153,   153,   586,   360,
     405,   268,   269,  -220,  -236,  -220,   311,   590,   153,   338,
     362,   592,   380,   399,  -229,  -229,  -229,  -229,  -229,  -229,
    -229,  -229,  -229,  -229,  -229,  -229,  -229,  -229,  -229,  -229,
     357,  -229,   415,   419,   421,  -236,  -236,  -236,  -236,  -236,
    -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,
    -236,  -236,  -236,  -236,  -236,   443,  -236,  -236,  -236,  -236,
    -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,   466,
     479,  -236,  -236,  -236,   468,  -236,   480,  -229,  -229,  -229,
     542,   200,  -236,   469,   471,  -236,   278,   199,   201,  -236,
    -236,    28,    28,   136,   137,   138,   139,   140,    77,    78,
     -82,    79,   243,     1,     2,     3,   235,     4,     5,     6,
    -169,   546,   581,   346,    80,   235,   436,   511,   464,  -172,
     136,   137,   138,   139,   140,  -208,    59,   560,   424,   136,
     137,   138,   139,   140,   394,   264,   310,   232,    60,   496,
     495,   545,   414,   129,   130,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   175,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   176,
      61,   114,   115,   116,   473,   117,   245,   430,   279,   297,
     179,   -82,    70,   431,    28,    71,   246,     0,     0,   118,
     119,    77,    78,     0,    79,  -169,  -169,  -169,     0,     0,
    -169,  -169,  -169,  -169,  -172,  -172,  -172,    80,  -296,  -172,
    -172,  -172,  -172,    62,     0,     0,  -208,   213,     0,     0,
       0,   214,     0,     0,     0,   135,   320,   136,   137,   138,
     139,   140,   141,   142,     0,   321,     0,     0,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,     0,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    28,     0,   114,   115,   116,     0,   117,    77,
      78,     0,    79,     0,     0,    70,     0,     0,    71,     0,
     318,   319,   118,   119,     0,    80,   215,   216,   217,   322,
     323,   324,   325,   326,   327,     0,     0,   153,     0,   320,
     230,   329,   136,   137,   138,   139,   140,   231,   321,     0,
       0,     0,     0,     0,     0,     0,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,     0,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
       0,   199,   114,   115,   116,    28,   117,     0,     0,     0,
       0,     0,     0,    70,     0,     0,    71,     0,     0,     0,
     118,   119,   322,   323,   324,   325,   326,   327,   283,    28,
     153,   328,     0,     0,   329,     0,     0,   135,     0,   136,
     137,   138,   139,   140,   141,   142,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    28,     0,     0,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,     0,   283,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   284,     0,    70,     0,     0,    71,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,     0,
       0,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,  -218,   319,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   284,     0,    70,     0,     0,
      71,   320,     0,     0,     0,     0,     0,     0,     0,     0,
     321,     0,     0,     0,     0,     0,     0,  -225,   319,     0,
    -218,  -218,  -218,  -218,  -218,  -218,  -218,  -218,  -218,  -218,
    -218,  -218,  -218,  -218,  -218,  -218,   320,     0,     0,     0,
       0,     0,     0,     0,     0,   321,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -225,  -225,  -225,  -225,  -225,
    -225,  -225,  -225,  -225,  -225,  -225,  -225,  -225,  -225,  -225,
    -225,     0,     0,     0,   322,   323,   324,   325,   326,   327,
       0,     0,   153,  -218,     0,  -218,   329,    77,    78,     0,
      79,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    80,     0,     0,     0,     0,     0,   322,
     323,   324,   325,   326,   327,     0,     0,   153,  -225,     0,
    -225,   329,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,     0,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,     0,     0,
     114,   115,   116,     0,   117,     0,     0,  -208,    59,     0,
       0,    70,    28,     0,    71,     0,     0,     0,   118,   119,
      60,     0,     0,     0,     0,  -211,    59,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    60,     0,
       0,     0,     0,     0,     0,  -208,  -208,  -208,  -208,  -208,
    -208,  -208,  -208,  -208,  -208,  -208,  -208,  -208,  -208,  -208,
    -208,     0,    61,  -211,  -211,  -211,  -211,  -211,  -211,  -211,
    -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,     0,
      61,     0,     0,     0,     0,     0,     0,     0,     0,  -208,
      59,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    60,     0,     0,    62,     0,     0,  -208,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    62,     0,     0,  -211,  -208,  -208,  -208,
    -208,  -208,  -208,  -208,  -208,  -208,  -208,  -208,  -208,  -208,
    -208,  -208,  -208,     0,    61,   318,   521,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -152,     0,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,   -61,  -110,   -93,
    -138,   318,    42,     0,     0,     0,     0,   -98,     0,     0,
       0,  -142,     0,     0,     0,     0,     0,    62,     0,     0,
    -208,     0,     0,     0,     0,     0,     0,     0,     0,   241,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   561,
     562,   563,   564,   565,   566,   567,   568,   569,   570,   571,
     572,   573,   574,   575,   576,     0,     0,     0,     0,     0,
       0,     0,   318,   580,     0,     0,     0,     0,     0,     0,
       0,     0,   283,     0,     0,  -152,   328,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,     0,  -110,   -93,  -138,     0,     0,
       0,     0,     0,     0,   -98,     0,     0,     0,     0,     0,
       0,     0,   328,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,     0,     0,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   284,     0,
      70,    -2,   239,    71,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   328,  -152,     0,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,   -61,  -110,   -93,  -138,   240,    42,     0,
       0,     0,     0,   -98,     0,     0,     0,  -142,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   241,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   196,     0,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,     0,     0,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-456)))

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-296)))

static const yytype_int16 yycheck[] =
{
       0,    37,    35,   120,   237,   191,   172,    57,     8,     9,
      10,    11,    12,    13,   244,    15,   371,    17,    18,    19,
      20,   167,   225,    39,   207,    32,   229,   299,     0,    29,
       5,    12,   277,    32,   170,   148,   281,   282,     0,   362,
       5,   286,    42,    43,    32,    32,    32,     0,   503,     0,
       1,     5,   348,   298,     5,   300,     5,   100,     5,    13,
      60,    61,    62,     5,     5,     1,     1,   363,     5,    20,
       5,     1,    72,    73,     0,   192,    13,    77,    78,    79,
      80,    17,     0,     1,    26,   191,   192,   273,     1,   100,
     207,   546,     5,    58,    20,    13,   419,     0,     5,   100,
     100,   207,   103,   306,   307,     0,     1,   273,     5,    58,
      91,    58,   112,   113,   114,   470,     5,   117,   118,   119,
     233,   234,     5,    12,   124,   125,   126,   127,   128,   101,
       1,     5,   132,   133,     5,   110,   272,   382,   100,     1,
     140,   103,    13,   466,   160,   468,   469,     1,   471,   102,
     104,   102,   159,   508,   105,   427,   156,   402,   294,   192,
     159,   112,    80,   163,   164,   165,   166,   167,   285,   110,
     100,   159,   159,   159,   207,   105,   102,   177,   178,   365,
     180,   426,   537,   101,   320,   291,   112,     1,   106,   102,
     423,   109,   105,   100,   377,     0,   196,   197,   334,   102,
     200,   201,   105,   100,   317,   100,   103,   353,   103,    80,
     105,   483,   358,   263,   103,   215,   216,   217,     5,   102,
     100,     1,   105,   239,   100,     5,   226,   227,   228,     5,
     104,   586,     5,   478,     5,   106,   252,   592,   109,   101,
     240,     5,   258,   105,   260,     1,   100,     5,    20,   249,
       1,   105,   435,     5,     5,     5,   422,    13,     5,   179,
     377,    13,    14,     5,   381,     5,     5,   267,   268,   269,
       0,     1,   305,   506,    13,   275,   276,     0,     1,   199,
     389,     1,     5,   283,   284,     5,   100,   100,   521,    19,
      13,   105,    12,    13,   524,   100,   101,     5,    28,    55,
       0,   301,   302,   303,    12,    13,     5,   540,   308,     1,
       5,   311,   515,     5,    13,   102,   549,   550,   435,     1,
     100,   101,   338,     5,   100,   105,   106,   107,   108,   105,
     103,   102,    55,    15,    16,    17,    18,    19,   102,   372,
     112,    22,    98,   343,   377,   101,   346,   580,   381,   100,
      24,   581,   102,   353,   106,   541,   103,   109,   358,   101,
     593,   101,    92,    93,    94,    95,    96,    97,     1,     5,
     100,    91,     5,   373,   104,    98,   485,   145,   101,   147,
     110,    17,    15,    16,    17,    18,    19,   387,   503,   417,
       5,   407,   392,   393,     1,     5,   396,   397,     5,   399,
      32,   106,   435,   403,   109,    12,    23,    17,   100,   101,
     410,     5,   412,   105,   106,   107,   108,   503,   100,   101,
     102,   421,     5,   105,   106,   107,   108,    25,     5,   465,
      13,   546,   432,   433,    17,    12,    13,   437,   438,   546,
     440,    56,    57,   443,   472,   552,     5,   463,     7,     8,
       9,    10,    11,     5,   454,     7,     8,     9,    10,    11,
     546,    36,    56,    57,   232,     5,     6,   100,   101,   102,
       0,     1,   105,   106,   107,   108,     1,   505,     5,     5,
       5,   101,     0,     1,    91,   103,    13,    13,    13,    19,
     103,   491,    17,    20,    21,   495,     5,     1,    28,    80,
     101,     5,   502,    12,   504,   521,   100,   507,    12,   103,
     100,   511,   512,   103,   100,    21,   549,   103,   518,     5,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    22,    23,    24,    25,
     100,    27,   542,   103,   100,   545,    32,   103,   101,    13,
      36,   110,   111,    21,   554,     5,     0,     1,   110,   111,
     560,     5,    92,    93,    94,    95,    96,    97,    12,    13,
     100,    15,   102,    13,   104,   100,   100,   100,   578,   103,
     103,   103,   103,   101,    28,   103,   105,   587,   100,     1,
     100,   591,     5,    13,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      17,    55,    13,   100,    55,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    82,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,   100,
       5,    95,    96,    97,   100,    99,     5,   101,   102,   103,
     102,   105,   106,   100,   100,   109,     0,     1,   112,   113,
     114,     5,     5,    15,    16,    17,    18,    19,    12,    13,
      13,    15,   159,    29,    30,    31,     1,    33,    34,    35,
       5,   520,   552,   254,    28,     1,   389,   485,   408,     5,
      15,    16,    17,    18,    19,     0,     1,   549,   372,    15,
      16,    17,    18,    19,   305,   169,   219,   145,    13,   456,
     455,   519,   356,    56,    57,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    64,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    64,
      55,    95,    96,    97,   420,    99,   159,   381,   102,   203,
       1,   104,   106,   381,     5,   109,   159,    -1,    -1,   113,
     114,    12,    13,    -1,    15,   100,   101,   102,    -1,    -1,
     105,   106,   107,   108,   100,   101,   102,    28,     1,   105,
     106,   107,   108,    98,    -1,    -1,   101,    13,    -1,    -1,
      -1,    17,    -1,    -1,    -1,    13,    19,    15,    16,    17,
      18,    19,    20,    21,    -1,    28,    -1,    -1,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    -1,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,     5,    -1,    95,    96,    97,    -1,    99,    12,
      13,    -1,    15,    -1,    -1,   106,    -1,    -1,   109,    -1,
       0,     1,   113,   114,    -1,    28,    92,    93,    94,    92,
      93,    94,    95,    96,    97,    -1,    -1,   100,    -1,    19,
      13,   104,    15,    16,    17,    18,    19,    20,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    -1,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      -1,     1,    95,    96,    97,     5,    99,    -1,    -1,    -1,
      -1,    -1,    -1,   106,    -1,    -1,   109,    -1,    -1,    -1,
     113,   114,    92,    93,    94,    95,    96,    97,    28,     5,
     100,   101,    -1,    -1,   104,    -1,    -1,    13,    -1,    15,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     5,    -1,    -1,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    -1,    28,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   104,    -1,   106,    -1,    -1,   109,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    -1,
      -1,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,     0,     1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   104,    -1,   106,    -1,    -1,
     109,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,     0,     1,    -1,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    92,    93,    94,    95,    96,    97,
      -1,    -1,   100,   101,    -1,   103,   104,    12,    13,    -1,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,    -1,    92,
      93,    94,    95,    96,    97,    -1,    -1,   100,   101,    -1,
     103,   104,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    -1,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    -1,    -1,
      95,    96,    97,    -1,    99,    -1,    -1,     0,     1,    -1,
      -1,   106,     5,    -1,   109,    -1,    -1,    -1,   113,   114,
      13,    -1,    -1,    -1,    -1,     0,     1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    55,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,
       1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    13,    -1,    -1,    98,    -1,    -1,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    -1,    -1,   101,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    55,     0,     1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    -1,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,     0,    27,    -1,    -1,    -1,    -1,    32,    -1,    -1,
      -1,    36,    -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,
     101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     0,     1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    28,    -1,    -1,    13,   101,    15,    16,    17,
      18,    19,    20,    21,    -1,    23,    24,    25,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    -1,    -1,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,
     106,     0,     1,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   101,    13,    -1,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    54,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    -1,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    -1,    -1,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    29,    30,    31,    33,    34,    35,   116,   117,   118,
     119,   120,   121,   122,   123,   100,   100,   100,   100,     5,
     100,     0,   123,   123,   123,   123,   123,   123,     5,    26,
     125,   128,   123,   123,   176,   123,   123,   123,   123,     1,
     123,   124,    27,   131,   138,   139,   140,   152,   154,   155,
     156,   162,   163,   169,   170,   171,   172,   177,   180,     1,
      13,    55,    98,   196,   197,   198,   199,   201,     1,    13,
     106,   109,   159,   160,   161,   174,   175,    12,    13,    15,
      28,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    95,    96,    97,    99,   113,   114,
     175,   203,   204,   207,   208,   209,   217,   218,   219,    56,
      57,   144,   145,   147,   148,    13,    15,    16,    17,    18,
      19,    20,    21,   182,   183,   184,   185,   187,   188,   189,
     190,   191,   195,   100,   103,   223,    12,     5,     6,   130,
       1,   123,   123,    22,    24,    32,    23,    25,    36,   182,
     227,   123,   123,   123,   101,   198,   199,   103,   103,     1,
      17,   200,   123,   123,    80,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,    78,   208,   101,     1,
     105,   112,   205,   206,   123,   123,   123,   123,   123,   123,
     123,   101,    21,    13,    17,    92,    93,    94,    13,   123,
      21,     1,   101,   179,     1,     5,   106,   107,   108,   173,
      13,    20,   187,   188,   188,     1,   189,   228,   123,     1,
      26,    54,   129,   131,   132,   221,   222,   103,   223,    13,
     141,   101,     1,   123,   137,   123,   153,   123,     1,   123,
       1,   123,   153,     5,   179,   228,   200,    17,   103,   103,
     123,   123,   227,   123,   101,   100,   105,    13,     0,   102,
     127,   203,   205,    28,   104,   175,   205,   208,   210,   213,
     214,   215,   123,   123,   227,   123,   123,   207,   205,   214,
     216,    58,    13,   104,    13,   123,   123,   123,    13,    20,
     184,   105,   185,   123,   123,   123,   185,   188,     0,     1,
      19,    28,    92,    93,    94,    95,    96,    97,   101,   104,
     126,   223,   224,   225,   226,   103,   223,   123,     1,   124,
     123,    12,    91,   142,   103,   223,   142,   148,   150,   100,
     103,    12,    13,   157,   103,   223,    13,    17,   164,   194,
     103,   223,   100,   182,   178,   123,   123,   123,   228,   200,
     203,   123,   123,   127,   127,   127,   123,   123,   208,   127,
       5,   211,   212,   205,   228,   127,   212,   105,   127,   123,
     123,   123,    13,    14,   174,   185,   186,   185,   123,    13,
     123,   228,   228,    12,   103,   103,   223,   123,   123,     1,
     105,   178,   151,   153,   194,    13,   153,   151,   178,   100,
     203,    55,   202,   196,   161,   123,    13,   214,   106,   109,
     210,   213,    20,   112,   127,   123,   144,   147,    58,   146,
      13,   123,   123,    82,     1,   123,   123,     7,     8,     9,
      10,    11,   110,   111,   192,   123,   181,     0,   110,   127,
     123,   103,   223,   149,   150,   123,   100,   123,   100,   100,
     176,   100,   151,   202,   123,   200,   126,   127,   212,     5,
       5,   123,   123,   214,   123,   123,   123,   102,   102,   123,
     102,   105,   102,   123,   110,   192,   183,   103,     0,   103,
     223,   148,   151,   133,   151,   151,   196,   151,   176,   127,
     212,   145,    17,   143,   102,   123,    12,    13,   193,   123,
     123,     1,   126,   135,   136,   139,   140,   154,   156,   163,
     170,   172,   180,   220,   221,   222,   123,   176,   126,   123,
     196,   123,   102,   185,   123,   193,   133,   126,   124,   158,
     165,   196,   134,   126,   203,   123,   110,   123,   220,   126,
     159,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,   126,   166,   168,
       1,   135,   220,   123,   110,   123,   123,   167,   124,   196,
     123,   100,   123,   196,   126
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, CSSParser* parser)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    CSSParser* parser;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, CSSParser* parser)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    CSSParser* parser;
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, CSSParser* parser)
#else
static void
yy_reduce_print (yyvsp, yyrule, parser)
    YYSTYPE *yyvsp;
    int yyrule;
    CSSParser* parser;
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, CSSParser* parser)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, parser)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    CSSParser* parser;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (parser);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 131: /* valid_rule */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2128 "./CSSGrammar.cpp"
        break;
      case 132: /* rule */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2135 "./CSSGrammar.cpp"
        break;
      case 133: /* block_rule_list */
/* Line 1393 of yacc.c  */
#line 183 "./CSSGrammar.y"
        { delete ((*yyvaluep).ruleList); };
/* Line 1393 of yacc.c  */
#line 2142 "./CSSGrammar.cpp"
        break;
      case 134: /* block_valid_rule_list */
/* Line 1393 of yacc.c  */
#line 183 "./CSSGrammar.y"
        { delete ((*yyvaluep).ruleList); };
/* Line 1393 of yacc.c  */
#line 2149 "./CSSGrammar.cpp"
        break;
      case 135: /* block_valid_rule */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2156 "./CSSGrammar.cpp"
        break;
      case 136: /* block_rule */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2163 "./CSSGrammar.cpp"
        break;
      case 139: /* import */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2170 "./CSSGrammar.cpp"
        break;
      case 143: /* maybe_media_value */
/* Line 1393 of yacc.c  */
#line 228 "./CSSGrammar.y"
        { delete ((*yyvaluep).valueList); };
/* Line 1393 of yacc.c  */
#line 2177 "./CSSGrammar.cpp"
        break;
      case 144: /* media_query_exp */
/* Line 1393 of yacc.c  */
#line 196 "./CSSGrammar.y"
        { delete ((*yyvaluep).mediaQueryExp); };
/* Line 1393 of yacc.c  */
#line 2184 "./CSSGrammar.cpp"
        break;
      case 145: /* media_query_exp_list */
/* Line 1393 of yacc.c  */
#line 199 "./CSSGrammar.y"
        { delete ((*yyvaluep).mediaQueryExpList); };
/* Line 1393 of yacc.c  */
#line 2191 "./CSSGrammar.cpp"
        break;
      case 146: /* maybe_and_media_query_exp_list */
/* Line 1393 of yacc.c  */
#line 199 "./CSSGrammar.y"
        { delete ((*yyvaluep).mediaQueryExpList); };
/* Line 1393 of yacc.c  */
#line 2198 "./CSSGrammar.cpp"
        break;
      case 148: /* media_query */
/* Line 1393 of yacc.c  */
#line 191 "./CSSGrammar.y"
        { delete ((*yyvaluep).mediaQuery); };
/* Line 1393 of yacc.c  */
#line 2205 "./CSSGrammar.cpp"
        break;
      case 149: /* maybe_media_list */
/* Line 1393 of yacc.c  */
#line 188 "./CSSGrammar.y"
        { if (((*yyvaluep).mediaList)) ((*yyvaluep).mediaList)->deref(); };
/* Line 1393 of yacc.c  */
#line 2212 "./CSSGrammar.cpp"
        break;
      case 150: /* media_list */
/* Line 1393 of yacc.c  */
#line 188 "./CSSGrammar.y"
        { if (((*yyvaluep).mediaList)) ((*yyvaluep).mediaList)->deref(); };
/* Line 1393 of yacc.c  */
#line 2219 "./CSSGrammar.cpp"
        break;
      case 154: /* media */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2226 "./CSSGrammar.cpp"
        break;
      case 156: /* keyframes */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2233 "./CSSGrammar.cpp"
        break;
      case 158: /* keyframes_rule */
/* Line 1393 of yacc.c  */
#line 206 "./CSSGrammar.y"
        { delete ((*yyvaluep).keyframeRuleList); };
/* Line 1393 of yacc.c  */
#line 2240 "./CSSGrammar.cpp"
        break;
      case 159: /* keyframe_rule */
/* Line 1393 of yacc.c  */
#line 203 "./CSSGrammar.y"
        { if (((*yyvaluep).keyframe)) ((*yyvaluep).keyframe)->deref(); };
/* Line 1393 of yacc.c  */
#line 2247 "./CSSGrammar.cpp"
        break;
      case 160: /* key_list */
/* Line 1393 of yacc.c  */
#line 228 "./CSSGrammar.y"
        { delete ((*yyvaluep).valueList); };
/* Line 1393 of yacc.c  */
#line 2254 "./CSSGrammar.cpp"
        break;
      case 163: /* page */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2261 "./CSSGrammar.cpp"
        break;
      case 164: /* page_selector */
/* Line 1393 of yacc.c  */
#line 214 "./CSSGrammar.y"
        { delete ((*yyvaluep).selector); };
/* Line 1393 of yacc.c  */
#line 2268 "./CSSGrammar.cpp"
        break;
      case 170: /* font_face */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2275 "./CSSGrammar.cpp"
        break;
      case 172: /* region */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2282 "./CSSGrammar.cpp"
        break;
      case 180: /* ruleset */
/* Line 1393 of yacc.c  */
#line 180 "./CSSGrammar.y"
        { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); };
/* Line 1393 of yacc.c  */
#line 2289 "./CSSGrammar.cpp"
        break;
      case 182: /* selector_list */
/* Line 1393 of yacc.c  */
#line 217 "./CSSGrammar.y"
        { delete ((*yyvaluep).selectorList); };
/* Line 1393 of yacc.c  */
#line 2296 "./CSSGrammar.cpp"
        break;
      case 183: /* selector */
/* Line 1393 of yacc.c  */
#line 214 "./CSSGrammar.y"
        { delete ((*yyvaluep).selector); };
/* Line 1393 of yacc.c  */
#line 2303 "./CSSGrammar.cpp"
        break;
      case 185: /* simple_selector */
/* Line 1393 of yacc.c  */
#line 214 "./CSSGrammar.y"
        { delete ((*yyvaluep).selector); };
/* Line 1393 of yacc.c  */
#line 2310 "./CSSGrammar.cpp"
        break;
      case 186: /* simple_selector_list */
/* Line 1393 of yacc.c  */
#line 217 "./CSSGrammar.y"
        { delete ((*yyvaluep).selectorList); };
/* Line 1393 of yacc.c  */
#line 2317 "./CSSGrammar.cpp"
        break;
      case 188: /* specifier_list */
/* Line 1393 of yacc.c  */
#line 214 "./CSSGrammar.y"
        { delete ((*yyvaluep).selector); };
/* Line 1393 of yacc.c  */
#line 2324 "./CSSGrammar.cpp"
        break;
      case 189: /* specifier */
/* Line 1393 of yacc.c  */
#line 214 "./CSSGrammar.y"
        { delete ((*yyvaluep).selector); };
/* Line 1393 of yacc.c  */
#line 2331 "./CSSGrammar.cpp"
        break;
      case 190: /* class */
/* Line 1393 of yacc.c  */
#line 214 "./CSSGrammar.y"
        { delete ((*yyvaluep).selector); };
/* Line 1393 of yacc.c  */
#line 2338 "./CSSGrammar.cpp"
        break;
      case 191: /* attrib */
/* Line 1393 of yacc.c  */
#line 214 "./CSSGrammar.y"
        { delete ((*yyvaluep).selector); };
/* Line 1393 of yacc.c  */
#line 2345 "./CSSGrammar.cpp"
        break;
      case 194: /* pseudo_page */
/* Line 1393 of yacc.c  */
#line 214 "./CSSGrammar.y"
        { delete ((*yyvaluep).selector); };
/* Line 1393 of yacc.c  */
#line 2352 "./CSSGrammar.cpp"
        break;
      case 195: /* pseudo */
/* Line 1393 of yacc.c  */
#line 214 "./CSSGrammar.y"
        { delete ((*yyvaluep).selector); };
/* Line 1393 of yacc.c  */
#line 2359 "./CSSGrammar.cpp"
        break;
      case 203: /* expr */
/* Line 1393 of yacc.c  */
#line 228 "./CSSGrammar.y"
        { delete ((*yyvaluep).valueList); };
/* Line 1393 of yacc.c  */
#line 2366 "./CSSGrammar.cpp"
        break;
      case 204: /* valid_expr */
/* Line 1393 of yacc.c  */
#line 228 "./CSSGrammar.y"
        { delete ((*yyvaluep).valueList); };
/* Line 1393 of yacc.c  */
#line 2373 "./CSSGrammar.cpp"
        break;
      case 207: /* term */
/* Line 1393 of yacc.c  */
#line 209 "./CSSGrammar.y"
        { destroy(((*yyvaluep).value)); };
/* Line 1393 of yacc.c  */
#line 2380 "./CSSGrammar.cpp"
        break;
      case 209: /* function */
/* Line 1393 of yacc.c  */
#line 209 "./CSSGrammar.y"
        { destroy(((*yyvaluep).value)); };
/* Line 1393 of yacc.c  */
#line 2387 "./CSSGrammar.cpp"
        break;
      case 210: /* calc_func_term */
/* Line 1393 of yacc.c  */
#line 209 "./CSSGrammar.y"
        { destroy(((*yyvaluep).value)); };
/* Line 1393 of yacc.c  */
#line 2394 "./CSSGrammar.cpp"
        break;
      case 213: /* calc_func_paren_expr */
/* Line 1393 of yacc.c  */
#line 228 "./CSSGrammar.y"
        { delete ((*yyvaluep).valueList); };
/* Line 1393 of yacc.c  */
#line 2401 "./CSSGrammar.cpp"
        break;
      case 214: /* calc_func_expr */
/* Line 1393 of yacc.c  */
#line 228 "./CSSGrammar.y"
        { delete ((*yyvaluep).valueList); };
/* Line 1393 of yacc.c  */
#line 2408 "./CSSGrammar.cpp"
        break;
      case 215: /* valid_calc_func_expr */
/* Line 1393 of yacc.c  */
#line 228 "./CSSGrammar.y"
        { delete ((*yyvaluep).valueList); };
/* Line 1393 of yacc.c  */
#line 2415 "./CSSGrammar.cpp"
        break;
      case 216: /* calc_func_expr_list */
/* Line 1393 of yacc.c  */
#line 228 "./CSSGrammar.y"
        { delete ((*yyvaluep).valueList); };
/* Line 1393 of yacc.c  */
#line 2422 "./CSSGrammar.cpp"
        break;
      case 217: /* calc_function */
/* Line 1393 of yacc.c  */
#line 209 "./CSSGrammar.y"
        { destroy(((*yyvaluep).value)); };
/* Line 1393 of yacc.c  */
#line 2429 "./CSSGrammar.cpp"
        break;
      case 219: /* min_or_max_function */
/* Line 1393 of yacc.c  */
#line 209 "./CSSGrammar.y"
        { destroy(((*yyvaluep).value)); };
/* Line 1393 of yacc.c  */
#line 2436 "./CSSGrammar.cpp"
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
yyparse (CSSParser* parser)
#else
int
yyparse (parser)
    CSSParser* parser;
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
        case 9:
/* Line 1787 of yacc.c  */
#line 243 "./CSSGrammar.y"
    { parser->m_rule = adoptRef((yyvsp[(4) - (6)].rule)); }
    break;

  case 10:
/* Line 1787 of yacc.c  */
#line 244 "./CSSGrammar.y"
    { parser->m_keyframe = adoptRef((yyvsp[(4) - (6)].keyframe)); }
    break;

  case 12:
/* Line 1787 of yacc.c  */
#line 247 "./CSSGrammar.y"
    {
        if ((yyvsp[(4) - (5)].valueList)) {
            parser->m_valueList = adoptPtr((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = parser->m_parsedProperties.size();
            if (!parser->parseValue(parser->m_id, parser->m_important))
                parser->rollbackLastProperties(parser->m_parsedProperties.size() - oldParsedProperties);
            parser->m_valueList = nullptr;
        }
    }
    break;

  case 13:
/* Line 1787 of yacc.c  */
#line 257 "./CSSGrammar.y"
    { parser->m_mediaQuery = adoptPtr((yyvsp[(4) - (5)].mediaQuery)); }
    break;

  case 14:
/* Line 1787 of yacc.c  */
#line 259 "./CSSGrammar.y"
    {
        if ((yyvsp[(4) - (5)].selectorList)) {
            if (parser->m_selectorListForParseSelector)
                parser->m_selectorListForParseSelector->adoptSelectorVector(*(yyvsp[(4) - (5)].selectorList));
            parser->recycleSelectorVector(adoptPtr((yyvsp[(4) - (5)].selectorList)));
        }
    }
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 273 "./CSSGrammar.y"
    {
     if (parser->m_styleSheet)
         parser->m_styleSheet->parserSetEncodingFromCharsetRule((yyvsp[(3) - (5)].string));
     if (parser->isExtractingSourceData() && parser->m_currentRuleDataStack->isEmpty() && parser->m_ruleSourceDataResult)
         parser->addNewRuleToSourceTree(CSSRuleSourceData::createUnknown());
  }
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 284 "./CSSGrammar.y"
    {
        if (RefPtr<StyleRuleBase> rule = adoptRef((yyvsp[(2) - (3)].rule))) {
            if (parser->m_styleSheet)
                parser->m_styleSheet->parserAppendRule(rule.release());
        }
    }
    break;

  case 38:
/* Line 1787 of yacc.c  */
#line 297 "./CSSGrammar.y"
    { (yyval.rule) = nullptr; }
    break;

  case 41:
/* Line 1787 of yacc.c  */
#line 302 "./CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(1) - (1)].rule);
        parser->m_hadSyntacticallyValidCSSRule = true;
    }
    break;

  case 42:
/* Line 1787 of yacc.c  */
#line 306 "./CSSGrammar.y"
    { (yyval.rule) = nullptr; }
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 307 "./CSSGrammar.y"
    { (yyval.rule) = nullptr; }
    break;

  case 44:
/* Line 1787 of yacc.c  */
#line 308 "./CSSGrammar.y"
    { (yyval.rule) = nullptr; }
    break;

  case 45:
/* Line 1787 of yacc.c  */
#line 311 "./CSSGrammar.y"
    { (yyval.ruleList) = nullptr; }
    break;

  case 46:
/* Line 1787 of yacc.c  */
#line 312 "./CSSGrammar.y"
    {
      (yyval.ruleList) = (yyvsp[(1) - (3)].ruleList);
      if (RefPtr<StyleRuleBase> rule = adoptRef((yyvsp[(2) - (3)].rule))) {
          if (!(yyval.ruleList))
              (yyval.ruleList) = new Vector<RefPtr<StyleRuleBase>>;
          (yyval.ruleList)->append(rule.release());
      }
  }
    break;

  case 47:
/* Line 1787 of yacc.c  */
#line 322 "./CSSGrammar.y"
    { (yyval.ruleList) = nullptr; }
    break;

  case 48:
/* Line 1787 of yacc.c  */
#line 323 "./CSSGrammar.y"
    {
      (yyval.ruleList) = (yyvsp[(1) - (3)].ruleList);
      if (RefPtr<StyleRuleBase> rule = adoptRef((yyvsp[(2) - (3)].rule))) {
          if (!(yyval.ruleList))
              (yyval.ruleList) = new Vector<RefPtr<StyleRuleBase>>;
          (yyval.ruleList)->append(rule.release());
      }
  }
    break;

  case 55:
/* Line 1787 of yacc.c  */
#line 339 "./CSSGrammar.y"
    { (yyval.rule) = nullptr; }
    break;

  case 56:
/* Line 1787 of yacc.c  */
#line 339 "./CSSGrammar.y"
    { (yyval.rule) = nullptr; }
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 339 "./CSSGrammar.y"
    { (yyval.rule) = nullptr; }
    break;

  case 60:
/* Line 1787 of yacc.c  */
#line 341 "./CSSGrammar.y"
    {
        parser->markRuleHeaderEnd();
        parser->markRuleBodyStart();
    }
    break;

  case 61:
/* Line 1787 of yacc.c  */
#line 347 "./CSSGrammar.y"
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::IMPORT_RULE);
    }
    break;

  case 62:
/* Line 1787 of yacc.c  */
#line 352 "./CSSGrammar.y"
    {
        (yyval.rule) = parser->createImportRule((yyvsp[(4) - (7)].string), adoptRef((yyvsp[(6) - (7)].mediaList))).leakRef();
    }
    break;

  case 63:
/* Line 1787 of yacc.c  */
#line 355 "./CSSGrammar.y"
    {
        (yyval.rule) = parser->createImportRule((yyvsp[(4) - (7)].string), adoptRef((yyvsp[(6) - (7)].mediaList))).leakRef();
    }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 358 "./CSSGrammar.y"
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
        if ((yyvsp[(6) - (7)].mediaList))
            (yyvsp[(6) - (7)].mediaList)->deref();
    }
    break;

  case 65:
/* Line 1787 of yacc.c  */
#line 364 "./CSSGrammar.y"
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 368 "./CSSGrammar.y"
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
    break;

  case 67:
/* Line 1787 of yacc.c  */
#line 374 "./CSSGrammar.y"
    { parser->addNamespace((yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string)); }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 379 "./CSSGrammar.y"
    { (yyval.string).clear(); }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 381 "./CSSGrammar.y"
    { (yyval.valueList) = nullptr; }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 381 "./CSSGrammar.y"
    { (yyval.valueList) = (yyvsp[(3) - (4)].valueList); }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 383 "./CSSGrammar.y"
    {
        OwnPtr<CSSParserValueList> mediaValue = adoptPtr((yyvsp[(7) - (9)].valueList));
        if ((yyvsp[(1) - (9)].mediaQueryRestrictor) != MediaQuery::None)
            (yyval.mediaQueryExp) = MediaQueryExp::create(emptyString(), nullptr).leakPtr();
        else {
            (yyvsp[(5) - (9)].string).lower();
            (yyval.mediaQueryExp) = MediaQueryExp::create((yyvsp[(5) - (9)].string), mediaValue.get()).leakPtr();
        }
    }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 394 "./CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = new Vector<OwnPtr<MediaQueryExp>>;
        (yyval.mediaQueryExpList)->append(adoptPtr((yyvsp[(1) - (1)].mediaQueryExp)));
    }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 398 "./CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(1) - (5)].mediaQueryExpList);
        (yyval.mediaQueryExpList)->append(adoptPtr((yyvsp[(5) - (5)].mediaQueryExp)));
    }
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 404 "./CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = new Vector<OwnPtr<MediaQueryExp>>;
    }
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 407 "./CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(3) - (3)].mediaQueryExpList);
    }
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 412 "./CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    }
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 415 "./CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 418 "./CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    }
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 423 "./CSSGrammar.y"
    {
        (yyval.mediaQuery) = new MediaQuery(MediaQuery::None, "all", adoptPtr((yyvsp[(1) - (1)].mediaQueryExpList)));
    }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 427 "./CSSGrammar.y"
    {
        (yyvsp[(3) - (5)].string).lower();
        (yyval.mediaQuery) = new MediaQuery((yyvsp[(1) - (5)].mediaQueryRestrictor), (yyvsp[(3) - (5)].string), adoptPtr((yyvsp[(5) - (5)].mediaQueryExpList)));
    }
    break;

  case 87:
/* Line 1787 of yacc.c  */
#line 432 "./CSSGrammar.y"
    { (yyval.mediaList) = MediaQuerySet::create().leakRef(); }
    break;

  case 89:
/* Line 1787 of yacc.c  */
#line 434 "./CSSGrammar.y"
    {
        (yyval.mediaList) = MediaQuerySet::create().leakRef();
        (yyval.mediaList)->addMediaQuery(adoptPtr((yyvsp[(1) - (1)].mediaQuery)));
        parser->updateLastMediaLine((yyval.mediaList));
    }
    break;

  case 90:
/* Line 1787 of yacc.c  */
#line 439 "./CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (4)].mediaList);
        OwnPtr<MediaQuery> mediaQuery = adoptPtr((yyvsp[(4) - (4)].mediaQuery));
        if ((yyval.mediaList)) {
            (yyval.mediaList)->addMediaQuery(mediaQuery.release());
            parser->updateLastMediaLine((yyval.mediaList));
        }
    }
    break;

  case 91:
/* Line 1787 of yacc.c  */
#line 447 "./CSSGrammar.y"
    {
        (yyval.mediaList) = nullptr;
        if ((yyvsp[(1) - (2)].mediaList))
            (yyvsp[(1) - (2)].mediaList)->deref();
    }
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 454 "./CSSGrammar.y"
    {
        parser->markRuleBodyStart();
    }
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 459 "./CSSGrammar.y"
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::MEDIA_RULE);
    }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 464 "./CSSGrammar.y"
    {
        parser->markRuleHeaderEnd();
    }
    break;

  case 95:
/* Line 1787 of yacc.c  */
#line 469 "./CSSGrammar.y"
    {
        (yyval.rule) = parser->createMediaRule(adoptRef((yyvsp[(4) - (10)].mediaList)), adoptPtr((yyvsp[(9) - (10)].ruleList)).get()).leakRef();
    }
    break;

  case 96:
/* Line 1787 of yacc.c  */
#line 472 "./CSSGrammar.y"
    {
        (yyval.rule) = parser->createEmptyMediaRule(adoptPtr((yyvsp[(7) - (8)].ruleList)).get()).leakRef();
    }
    break;

  case 97:
/* Line 1787 of yacc.c  */
#line 475 "./CSSGrammar.y"
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
    break;

  case 98:
/* Line 1787 of yacc.c  */
#line 481 "./CSSGrammar.y"
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::KEYFRAMES_RULE);
    }
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 486 "./CSSGrammar.y"
    {
        (yyval.rule) = parser->createKeyframesRule((yyvsp[(4) - (10)].string), adoptPtr((yyvsp[(9) - (10)].keyframeRuleList))).leakRef();
    }
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 492 "./CSSGrammar.y"
    { (yyval.keyframeRuleList) = new Vector<RefPtr<StyleKeyframe>>; }
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 493 "./CSSGrammar.y"
    {
        (yyval.keyframeRuleList) = (yyvsp[(1) - (3)].keyframeRuleList);
        if (RefPtr<StyleKeyframe> keyframe = adoptRef((yyvsp[(2) - (3)].keyframe)))
            (yyval.keyframeRuleList)->append(keyframe.release());
    }
    break;

  case 104:
/* Line 1787 of yacc.c  */
#line 499 "./CSSGrammar.y"
    { (yyval.keyframe) = parser->createKeyframe(*adoptPtr((yyvsp[(1) - (6)].valueList))).leakRef(); }
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 501 "./CSSGrammar.y"
    {
        (yyval.valueList) = new CSSParserValueList;
        (yyval.valueList)->addValue((yyvsp[(1) - (1)].value));
    }
    break;

  case 106:
/* Line 1787 of yacc.c  */
#line 505 "./CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(1) - (5)].valueList);
        ASSERT((yyvsp[(5) - (5)].value).unit != CSSParserValue::Function);
        if ((yyval.valueList))
            (yyval.valueList)->addValue((yyvsp[(5) - (5)].value));
    }
    break;

  case 107:
/* Line 1787 of yacc.c  */
#line 513 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (2)].integer) * (yyvsp[(2) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; }
    break;

  case 108:
/* Line 1787 of yacc.c  */
#line 514 "./CSSGrammar.y"
    {
        (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER;
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        if (str.equalIgnoringCase("from"))
            (yyval.value).fValue = 0;
        else if (str.equalIgnoringCase("to"))
            (yyval.value).fValue = 100;
        else {
            (yyval.value).unit = 0;
            YYERROR;
        }
    }
    break;

  case 109:
/* Line 1787 of yacc.c  */
#line 526 "./CSSGrammar.y"
    {
        (yyval.value).unit = 0;
    }
    break;

  case 110:
/* Line 1787 of yacc.c  */
#line 531 "./CSSGrammar.y"
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::PAGE_RULE);
    }
    break;

  case 111:
/* Line 1787 of yacc.c  */
#line 537 "./CSSGrammar.y"
    {
        if ((yyvsp[(4) - (10)].selector))
            (yyval.rule) = parser->createPageRule(adoptPtr((yyvsp[(4) - (10)].selector))).leakRef();
        else {
            parser->clearProperties();
            (yyval.rule) = nullptr;
            parser->popRuleData();
        }
    }
    break;

  case 112:
/* Line 1787 of yacc.c  */
#line 546 "./CSSGrammar.y"
    {
        parser->popRuleData();
        (yyval.rule) = nullptr;
    }
    break;

  case 113:
/* Line 1787 of yacc.c  */
#line 550 "./CSSGrammar.y"
    {
        parser->popRuleData();
        (yyval.rule) = nullptr;
    }
    break;

  case 114:
/* Line 1787 of yacc.c  */
#line 556 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector(QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), parser->m_defaultNamespace));
        (yyval.selector)->setForPage();
    }
    break;

  case 115:
/* Line 1787 of yacc.c  */
#line 560 "./CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            (yyval.selector)->prependTagSelector(QualifiedName(nullAtom, (yyvsp[(1) - (2)].string), parser->m_defaultNamespace));
            (yyval.selector)->setForPage();
        }
    }
    break;

  case 116:
/* Line 1787 of yacc.c  */
#line 567 "./CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
        if ((yyval.selector))
            (yyval.selector)->setForPage();
    }
    break;

  case 117:
/* Line 1787 of yacc.c  */
#line 572 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setForPage();
    }
    break;

  case 120:
/* Line 1787 of yacc.c  */
#line 579 "./CSSGrammar.y"
    {
        parser->startDeclarationsForMarginBox();
    }
    break;

  case 121:
/* Line 1787 of yacc.c  */
#line 581 "./CSSGrammar.y"
    {
        parser->createMarginAtRule((yyvsp[(1) - (7)].marginBox));
    }
    break;

  case 122:
/* Line 1787 of yacc.c  */
#line 586 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopLeftCornerMarginBox;
    }
    break;

  case 123:
/* Line 1787 of yacc.c  */
#line 589 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopLeftMarginBox;
    }
    break;

  case 124:
/* Line 1787 of yacc.c  */
#line 592 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopCenterMarginBox;
    }
    break;

  case 125:
/* Line 1787 of yacc.c  */
#line 595 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopRightMarginBox;
    }
    break;

  case 126:
/* Line 1787 of yacc.c  */
#line 598 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopRightCornerMarginBox;
    }
    break;

  case 127:
/* Line 1787 of yacc.c  */
#line 601 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomLeftCornerMarginBox;
    }
    break;

  case 128:
/* Line 1787 of yacc.c  */
#line 604 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomLeftMarginBox;
    }
    break;

  case 129:
/* Line 1787 of yacc.c  */
#line 607 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomCenterMarginBox;
    }
    break;

  case 130:
/* Line 1787 of yacc.c  */
#line 610 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomRightMarginBox;
    }
    break;

  case 131:
/* Line 1787 of yacc.c  */
#line 613 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomRightCornerMarginBox;
    }
    break;

  case 132:
/* Line 1787 of yacc.c  */
#line 616 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::LeftTopMarginBox;
    }
    break;

  case 133:
/* Line 1787 of yacc.c  */
#line 619 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::LeftMiddleMarginBox;
    }
    break;

  case 134:
/* Line 1787 of yacc.c  */
#line 622 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::LeftBottomMarginBox;
    }
    break;

  case 135:
/* Line 1787 of yacc.c  */
#line 625 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::RightTopMarginBox;
    }
    break;

  case 136:
/* Line 1787 of yacc.c  */
#line 628 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::RightMiddleMarginBox;
    }
    break;

  case 137:
/* Line 1787 of yacc.c  */
#line 631 "./CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::RightBottomMarginBox;
    }
    break;

  case 138:
/* Line 1787 of yacc.c  */
#line 636 "./CSSGrammar.y"
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::FONT_FACE_RULE);
    }
    break;

  case 139:
/* Line 1787 of yacc.c  */
#line 641 "./CSSGrammar.y"
    {
        (yyval.rule) = parser->createFontFaceRule().leakRef();
    }
    break;

  case 140:
/* Line 1787 of yacc.c  */
#line 644 "./CSSGrammar.y"
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
    break;

  case 141:
/* Line 1787 of yacc.c  */
#line 648 "./CSSGrammar.y"
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
    break;

  case 142:
/* Line 1787 of yacc.c  */
#line 654 "./CSSGrammar.y"
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::REGION_RULE);
    }
    break;

  case 143:
/* Line 1787 of yacc.c  */
#line 659 "./CSSGrammar.y"
    {
        OwnPtr<Vector<RefPtr<StyleRuleBase>>> ruleList = adoptPtr((yyvsp[(9) - (10)].ruleList));
        if ((yyvsp[(4) - (10)].selectorList))
            (yyval.rule) = parser->createRegionRule(adoptPtr((yyvsp[(4) - (10)].selectorList)).get(), ruleList.get()).leakRef();
        else {
            (yyval.rule) = nullptr;
            parser->popRuleData();
        }
    }
    break;

  case 144:
/* Line 1787 of yacc.c  */
#line 670 "./CSSGrammar.y"
    { (yyval.relation) = CSSSelector::DirectAdjacent; }
    break;

  case 145:
/* Line 1787 of yacc.c  */
#line 671 "./CSSGrammar.y"
    { (yyval.relation) = CSSSelector::IndirectAdjacent; }
    break;

  case 146:
/* Line 1787 of yacc.c  */
#line 672 "./CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Child; }
    break;

  case 148:
/* Line 1787 of yacc.c  */
#line 674 "./CSSGrammar.y"
    { (yyval.integer) = 1; }
    break;

  case 149:
/* Line 1787 of yacc.c  */
#line 675 "./CSSGrammar.y"
    { (yyval.integer) = -1; }
    break;

  case 150:
/* Line 1787 of yacc.c  */
#line 675 "./CSSGrammar.y"
    { (yyval.integer) = 1; }
    break;

  case 151:
/* Line 1787 of yacc.c  */
#line 676 "./CSSGrammar.y"
    { parser->markPropertyStart(); }
    break;

  case 152:
/* Line 1787 of yacc.c  */
#line 678 "./CSSGrammar.y"
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::STYLE_RULE);
        parser->markSelectorStart();
    }
    break;

  case 153:
/* Line 1787 of yacc.c  */
#line 683 "./CSSGrammar.y"
    { parser->markRuleHeaderEnd(); }
    break;

  case 154:
/* Line 1787 of yacc.c  */
#line 684 "./CSSGrammar.y"
    { parser->markSelectorEnd(); }
    break;

  case 155:
/* Line 1787 of yacc.c  */
#line 686 "./CSSGrammar.y"
    {
        (yyval.rule) = parser->createStyleRule((yyvsp[(2) - (9)].selectorList)).leakRef();
        parser->recycleSelectorVector(adoptPtr((yyvsp[(2) - (9)].selectorList)));
    }
    break;

  case 156:
/* Line 1787 of yacc.c  */
#line 691 "./CSSGrammar.y"
    { parser->markSelectorStart(); }
    break;

  case 157:
/* Line 1787 of yacc.c  */
#line 693 "./CSSGrammar.y"
    {
        (yyval.selectorList) = nullptr;
        if ((yyvsp[(1) - (1)].selector)) {
            (yyval.selectorList) = parser->createSelectorVector().leakPtr();
            (yyval.selectorList)->append(adoptPtr((yyvsp[(1) - (1)].selector)));
            parser->updateLastSelectorLineAndPosition();
        }
    }
    break;

  case 158:
/* Line 1787 of yacc.c  */
#line 701 "./CSSGrammar.y"
    {
        OwnPtr<Vector<OwnPtr<CSSParserSelector>>> selectorList = adoptPtr((yyvsp[(1) - (6)].selectorList));
        OwnPtr<CSSParserSelector> selector = adoptPtr((yyvsp[(6) - (6)].selector));
        (yyval.selectorList) = nullptr;
        if (selectorList && selector) {
            (yyval.selectorList) = selectorList.leakPtr();
            (yyval.selectorList)->append(selector.release());
            parser->updateLastSelectorLineAndPosition();
        }
    }
    break;

  case 159:
/* Line 1787 of yacc.c  */
#line 711 "./CSSGrammar.y"
    {
        (yyval.selectorList) = nullptr;
        delete (yyvsp[(1) - (2)].selectorList);
    }
    break;

  case 162:
/* Line 1787 of yacc.c  */
#line 719 "./CSSGrammar.y"
    {
        OwnPtr<CSSParserSelector> left = adoptPtr((yyvsp[(1) - (3)].selector));
        OwnPtr<CSSParserSelector> right = adoptPtr((yyvsp[(3) - (3)].selector));
        (yyval.selector) = nullptr;
        if (left && right) {
            right->appendTagHistory(CSSSelector::Descendant, left.release());
            (yyval.selector) = right.leakPtr();
        }
    }
    break;

  case 163:
/* Line 1787 of yacc.c  */
#line 728 "./CSSGrammar.y"
    {
        OwnPtr<CSSParserSelector> left = adoptPtr((yyvsp[(1) - (3)].selector));
        OwnPtr<CSSParserSelector> right = adoptPtr((yyvsp[(3) - (3)].selector));
        (yyval.selector) = nullptr;
        if (left && right) {
            right->appendTagHistory((yyvsp[(2) - (3)].relation), left.release());
            (yyval.selector) = right.leakPtr();
        }
    }
    break;

  case 164:
/* Line 1787 of yacc.c  */
#line 737 "./CSSGrammar.y"
    {
        (yyval.selector) = nullptr;
        delete (yyvsp[(1) - (2)].selector);
    }
    break;

  case 165:
/* Line 1787 of yacc.c  */
#line 743 "./CSSGrammar.y"
    { (yyval.string).clear(); }
    break;

  case 166:
/* Line 1787 of yacc.c  */
#line 744 "./CSSGrammar.y"
    { static LChar star = '*'; (yyval.string).init(&star, 1); }
    break;

  case 168:
/* Line 1787 of yacc.c  */
#line 748 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector(QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), parser->m_defaultNamespace));
    }
    break;

  case 169:
/* Line 1787 of yacc.c  */
#line 751 "./CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector))
            parser->rewriteSpecifiersWithElementName(nullAtom, (yyvsp[(1) - (2)].string), *(yyval.selector));
    }
    break;

  case 170:
/* Line 1787 of yacc.c  */
#line 756 "./CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
        if ((yyval.selector))
            parser->rewriteSpecifiersWithNamespaceIfNeeded(*(yyval.selector));
    }
    break;

  case 171:
/* Line 1787 of yacc.c  */
#line 761 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector(parser->determineNameInNamespace((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string)));
    }
    break;

  case 172:
/* Line 1787 of yacc.c  */
#line 764 "./CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if ((yyval.selector))
            parser->rewriteSpecifiersWithElementName((yyvsp[(1) - (3)].string), (yyvsp[(2) - (3)].string), *(yyval.selector));
    }
    break;

  case 173:
/* Line 1787 of yacc.c  */
#line 769 "./CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector))
            parser->rewriteSpecifiersWithElementName((yyvsp[(1) - (2)].string), starAtom, *(yyval.selector));
    }
    break;

  case 174:
/* Line 1787 of yacc.c  */
#line 776 "./CSSGrammar.y"
    {
        (yyval.selectorList) = nullptr;
        if ((yyvsp[(1) - (1)].selector)) {
            (yyval.selectorList) = parser->createSelectorVector().leakPtr();
            (yyval.selectorList)->append(adoptPtr((yyvsp[(1) - (1)].selector)));
        }
    }
    break;

  case 175:
/* Line 1787 of yacc.c  */
#line 783 "./CSSGrammar.y"
    {
        OwnPtr<Vector<OwnPtr<CSSParserSelector>>> list = adoptPtr((yyvsp[(1) - (5)].selectorList));
        OwnPtr<CSSParserSelector> selector = adoptPtr((yyvsp[(5) - (5)].selector));
        (yyval.selectorList) = nullptr;
        if (list && selector) {
            (yyval.selectorList) = list.leakPtr();
            (yyval.selectorList)->append(selector.release());
        }
    }
    break;

  case 176:
/* Line 1787 of yacc.c  */
#line 792 "./CSSGrammar.y"
    {
        (yyval.selectorList) = nullptr;
        delete (yyvsp[(1) - (2)].selectorList);
    }
    break;

  case 177:
/* Line 1787 of yacc.c  */
#line 798 "./CSSGrammar.y"
    {
        if (parser->m_context.isHTMLDocument)
            (yyvsp[(1) - (1)].string).lower();
        (yyval.string) = (yyvsp[(1) - (1)].string);
    }
    break;

  case 178:
/* Line 1787 of yacc.c  */
#line 803 "./CSSGrammar.y"
    {
        static LChar star = '*';
        (yyval.string).init(&star, 1);
    }
    break;

  case 180:
/* Line 1787 of yacc.c  */
#line 810 "./CSSGrammar.y"
    {
        OwnPtr<CSSParserSelector> list = adoptPtr((yyvsp[(1) - (2)].selector));
        OwnPtr<CSSParserSelector> specifier = adoptPtr((yyvsp[(2) - (2)].selector));
        (yyval.selector) = nullptr;
        if (list && specifier)
            (yyval.selector) = parser->rewriteSpecifiers(list.release(), specifier.release()).leakPtr();
    }
    break;

  case 181:
/* Line 1787 of yacc.c  */
#line 817 "./CSSGrammar.y"
    {
        (yyval.selector) = nullptr;
        delete (yyvsp[(1) - (2)].selector);
    }
    break;

  case 182:
/* Line 1787 of yacc.c  */
#line 823 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setMatch(CSSSelector::Id);
        if (parser->m_context.mode == CSSQuirksMode)
            (yyvsp[(1) - (1)].string).lower();
        (yyval.selector)->setValue((yyvsp[(1) - (1)].string));
    }
    break;

  case 183:
/* Line 1787 of yacc.c  */
#line 830 "./CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].string)[0] >= '0' && (yyvsp[(1) - (1)].string)[0] <= '9')
            (yyval.selector) = nullptr;
        else {
            (yyval.selector) = new CSSParserSelector;
            (yyval.selector)->setMatch(CSSSelector::Id);
            if (parser->m_context.mode == CSSQuirksMode)
                (yyvsp[(1) - (1)].string).lower();
            (yyval.selector)->setValue((yyvsp[(1) - (1)].string));
        }
    }
    break;

  case 187:
/* Line 1787 of yacc.c  */
#line 846 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setMatch(CSSSelector::Class);
        if (parser->m_context.mode == CSSQuirksMode)
            (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->setValue((yyvsp[(2) - (2)].string));
    }
    break;

  case 188:
/* Line 1787 of yacc.c  */
#line 855 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (5)].string), nullAtom), parser->m_context.isHTMLDocument);
        (yyval.selector)->setMatch(CSSSelector::Set);
    }
    break;

  case 189:
/* Line 1787 of yacc.c  */
#line 860 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (9)].string), nullAtom), parser->m_context.isHTMLDocument);
        (yyval.selector)->setMatch((yyvsp[(5) - (9)].match));
        (yyval.selector)->setValue((yyvsp[(7) - (9)].string));
    }
    break;

  case 190:
/* Line 1787 of yacc.c  */
#line 866 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setAttribute(parser->determineNameInNamespace((yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string)), parser->m_context.isHTMLDocument);
        (yyval.selector)->setMatch(CSSSelector::Set);
    }
    break;

  case 191:
/* Line 1787 of yacc.c  */
#line 871 "./CSSGrammar.y"
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setAttribute(parser->determineNameInNamespace((yyvsp[(3) - (10)].string), (yyvsp[(4) - (10)].string)), parser->m_context.isHTMLDocument);
        (yyval.selector)->setMatch((yyvsp[(6) - (10)].match));
        (yyval.selector)->setValue((yyvsp[(8) - (10)].string));
    }
    break;

  case 192:
/* Line 1787 of yacc.c  */
#line 879 "./CSSGrammar.y"
    {
        (yyval.match) = CSSSelector::Exact;
    }
    break;

  case 193:
/* Line 1787 of yacc.c  */
#line 882 "./CSSGrammar.y"
    {
        (yyval.match) = CSSSelector::List;
    }
    break;

  case 194:
/* Line 1787 of yacc.c  */
#line 885 "./CSSGrammar.y"
    {
        (yyval.match) = CSSSelector::Hyphen;
    }
    break;

  case 195:
/* Line 1787 of yacc.c  */
#line 888 "./CSSGrammar.y"
    {
        (yyval.match) = CSSSelector::Begin;
    }
    break;

  case 196:
/* Line 1787 of yacc.c  */
#line 891 "./CSSGrammar.y"
    {
        (yyval.match) = CSSSelector::End;
    }
    break;

  case 197:
/* Line 1787 of yacc.c  */
#line 894 "./CSSGrammar.y"
    {
        (yyval.match) = CSSSelector::Contain;
    }
    break;

  case 200:
/* Line 1787 of yacc.c  */
#line 900 "./CSSGrammar.y"
    {
        (yyval.selector) = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PagePseudoClass);
        (yyvsp[(2) - (2)].string).lower();
        selector->setValue((yyvsp[(2) - (2)].string));
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            (yyval.selector) = selector.release();
    }
    break;

  case 201:
/* Line 1787 of yacc.c  */
#line 910 "./CSSGrammar.y"
    {
        (yyval.selector) = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        (yyvsp[(2) - (2)].string).lower();
        selector->setValue((yyvsp[(2) - (2)].string));
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            (yyval.selector) = selector.release();
    }
    break;

  case 202:
/* Line 1787 of yacc.c  */
#line 919 "./CSSGrammar.y"
    {
        (yyval.selector) = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoElement);
        (yyvsp[(3) - (3)].string).lower();
        selector->setValue((yyvsp[(3) - (3)].string));
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            (yyval.selector) = selector.release();
    }
    break;

  case 203:
/* Line 1787 of yacc.c  */
#line 928 "./CSSGrammar.y"
    {
        (yyval.selector) = nullptr;
        if ((yyvsp[(4) - (6)].selectorList)) {
            auto selector = std::make_unique<CSSParserSelector>();
            selector->setMatch(CSSSelector::PseudoClass);
            selector->adoptSelectorVector(*adoptPtr((yyvsp[(4) - (6)].selectorList)));
            (yyvsp[(2) - (6)].string).lower();
            selector->setValue((yyvsp[(2) - (6)].string));
            if (selector->pseudoType() == CSSSelector::PseudoAny)
                (yyval.selector) = selector.release();
        }
    }
    break;

  case 204:
/* Line 1787 of yacc.c  */
#line 940 "./CSSGrammar.y"
    {
        (yyval.selector) = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument((yyvsp[(4) - (6)].string));
        selector->setValue((yyvsp[(2) - (6)].string));
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            (yyval.selector) = selector.release();
    }
    break;

  case 205:
/* Line 1787 of yacc.c  */
#line 949 "./CSSGrammar.y"
    {
        (yyval.selector) = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument(AtomicString::number((yyvsp[(4) - (7)].integer) * (yyvsp[(5) - (7)].number)));
        selector->setValue((yyvsp[(2) - (7)].string));
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            (yyval.selector) = selector.release();
    }
    break;

  case 206:
/* Line 1787 of yacc.c  */
#line 958 "./CSSGrammar.y"
    {
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument((yyvsp[(4) - (6)].string));
        (yyvsp[(2) - (6)].string).lower();
        selector->setValue((yyvsp[(2) - (6)].string));
        CSSSelector::PseudoType type = selector->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            selector = nullptr;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (!isValidNthToken((yyvsp[(4) - (6)].string)))
                selector = nullptr;
        }
        (yyval.selector) = selector.release();
    }
    break;

  case 207:
/* Line 1787 of yacc.c  */
#line 976 "./CSSGrammar.y"
    {
        OwnPtr<CSSParserSelector> selector = adoptPtr((yyvsp[(4) - (6)].selector));
        (yyval.selector) = nullptr;
        if (selector && selector->isSimple()) {
            (yyval.selector) = new CSSParserSelector;
            (yyval.selector)->setMatch(CSSSelector::PseudoClass);
            Vector<OwnPtr<CSSParserSelector>> selectorVector;
            selectorVector.append(selector.release());
            (yyval.selector)->adoptSelectorVector(selectorVector);
            (yyvsp[(2) - (6)].string).lower();
            (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        }
    }
    break;

  case 208:
/* Line 1787 of yacc.c  */
#line 991 "./CSSGrammar.y"
    { (yyval.boolean) = false; }
    break;

  case 210:
/* Line 1787 of yacc.c  */
#line 993 "./CSSGrammar.y"
    { (yyval.boolean) = (yyvsp[(1) - (2)].boolean) || (yyvsp[(2) - (2)].boolean); }
    break;

  case 212:
/* Line 1787 of yacc.c  */
#line 995 "./CSSGrammar.y"
    { (yyval.boolean) = false; }
    break;

  case 214:
/* Line 1787 of yacc.c  */
#line 999 "./CSSGrammar.y"
    {
        parser->markPropertyStart();
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    }
    break;

  case 215:
/* Line 1787 of yacc.c  */
#line 1003 "./CSSGrammar.y"
    {
        parser->markPropertyStart();
        (yyval.boolean) = false;
    }
    break;

  case 216:
/* Line 1787 of yacc.c  */
#line 1007 "./CSSGrammar.y"
    {
        parser->markPropertyStart();
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    }
    break;

  case 217:
/* Line 1787 of yacc.c  */
#line 1013 "./CSSGrammar.y"
    {
        parser->markPropertyStart();
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    }
    break;

  case 218:
/* Line 1787 of yacc.c  */
#line 1019 "./CSSGrammar.y"
    {
        parser->syntaxError((yyvsp[(2) - (3)].location), CSSParser::PropertyDeclarationError);
    }
    break;

  case 219:
/* Line 1787 of yacc.c  */
#line 1024 "./CSSGrammar.y"
    {
        delete (yyvsp[(5) - (6)].valueList);
        (yyval.boolean) = false;
    }
    break;

  case 220:
/* Line 1787 of yacc.c  */
#line 1028 "./CSSGrammar.y"
    {
        (yyval.boolean) = false;
        bool isPropertyParsed = false;
        OwnPtr<CSSParserValueList> propertyValue = adoptPtr((yyvsp[(4) - (5)].valueList));
        if ((yyvsp[(1) - (5)].id) && propertyValue) {
            parser->m_valueList = propertyValue.release();
            int oldParsedProperties = parser->m_parsedProperties.size();
            (yyval.boolean) = parser->parseValue((yyvsp[(1) - (5)].id), (yyvsp[(5) - (5)].boolean));
            if (!(yyval.boolean))
                parser->rollbackLastProperties(parser->m_parsedProperties.size() - oldParsedProperties);
            else
                isPropertyParsed = true;
            parser->m_valueList = nullptr;
        }
        parser->markPropertyEnd((yyvsp[(5) - (5)].boolean), isPropertyParsed);
    }
    break;

  case 221:
/* Line 1787 of yacc.c  */
#line 1044 "./CSSGrammar.y"
    { (yyval.boolean) = false; }
    break;

  case 222:
/* Line 1787 of yacc.c  */
#line 1045 "./CSSGrammar.y"
    {
        parser->markPropertyEnd(false, false);
        delete (yyvsp[(4) - (6)].valueList);
        (yyval.boolean) = false;
    }
    break;

  case 223:
/* Line 1787 of yacc.c  */
#line 1050 "./CSSGrammar.y"
    {
        (yyval.boolean) = false;
    }
    break;

  case 224:
/* Line 1787 of yacc.c  */
#line 1053 "./CSSGrammar.y"
    {
        parser->markPropertyEnd(false, false);
        (yyval.boolean) = false;
    }
    break;

  case 225:
/* Line 1787 of yacc.c  */
#line 1058 "./CSSGrammar.y"
    { parser->syntaxError((yyvsp[(2) - (3)].location)); }
    break;

  case 226:
/* Line 1787 of yacc.c  */
#line 1059 "./CSSGrammar.y"
    { (yyval.id) = cssPropertyID((yyvsp[(1) - (2)].string)); }
    break;

  case 227:
/* Line 1787 of yacc.c  */
#line 1060 "./CSSGrammar.y"
    { (yyval.boolean) = true; }
    break;

  case 228:
/* Line 1787 of yacc.c  */
#line 1060 "./CSSGrammar.y"
    { (yyval.boolean) = false; }
    break;

  case 230:
/* Line 1787 of yacc.c  */
#line 1061 "./CSSGrammar.y"
    { (yyval.valueList) = nullptr; delete (yyvsp[(1) - (2)].valueList); }
    break;

  case 231:
/* Line 1787 of yacc.c  */
#line 1063 "./CSSGrammar.y"
    {
        (yyval.valueList) = new CSSParserValueList;
        (yyval.valueList)->addValue((yyvsp[(1) - (1)].value));
    }
    break;

  case 232:
/* Line 1787 of yacc.c  */
#line 1067 "./CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        if (!(yyval.valueList))
            destroy((yyvsp[(3) - (3)].value));
        else {
            if ((yyvsp[(2) - (3)].character)) {
                CSSParserValue v;
                v.id = CSSValueInvalid;
                v.unit = CSSParserValue::Operator;
                v.iValue = (yyvsp[(2) - (3)].character);
                (yyval.valueList)->addValue(v);
            }
            (yyval.valueList)->addValue((yyvsp[(3) - (3)].value));
        }
    }
    break;

  case 234:
/* Line 1787 of yacc.c  */
#line 1084 "./CSSGrammar.y"
    { (yyval.character) = '/'; }
    break;

  case 235:
/* Line 1787 of yacc.c  */
#line 1084 "./CSSGrammar.y"
    { (yyval.character) = ','; }
    break;

  case 236:
/* Line 1787 of yacc.c  */
#line 1084 "./CSSGrammar.y"
    { (yyval.character) = 0; }
    break;

  case 238:
/* Line 1787 of yacc.c  */
#line 1087 "./CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (3)].value); (yyval.value).fValue *= (yyvsp[(1) - (3)].integer); }
    break;

  case 239:
/* Line 1787 of yacc.c  */
#line 1088 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; }
    break;

  case 240:
/* Line 1787 of yacc.c  */
#line 1089 "./CSSGrammar.y"
    {
      (yyval.value).id = cssValueKeywordID((yyvsp[(1) - (2)].string));
      (yyval.value).unit = CSSPrimitiveValue::CSS_IDENT;
      (yyval.value).string = (yyvsp[(1) - (2)].string);
  }
    break;

  case 241:
/* Line 1787 of yacc.c  */
#line 1094 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; }
    break;

  case 242:
/* Line 1787 of yacc.c  */
#line 1095 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(2) - (3)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; }
    break;

  case 243:
/* Line 1787 of yacc.c  */
#line 1096 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_URI; }
    break;

  case 244:
/* Line 1787 of yacc.c  */
#line 1097 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_UNICODE_RANGE; }
    break;

  case 245:
/* Line 1787 of yacc.c  */
#line 1098 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
    break;

  case 246:
/* Line 1787 of yacc.c  */
#line 1099 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = CSSParserString(); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
    break;

  case 247:
/* Line 1787 of yacc.c  */
#line 1100 "./CSSGrammar.y"
    {
      (yyval.value).id = CSSValueInvalid;
      (yyval.value).unit = 0;
  }
    break;

  case 251:
/* Line 1787 of yacc.c  */
#line 1107 "./CSSGrammar.y"
    {
      (yyval.value).id = CSSValueInvalid; (yyval.value).unit = 0;
  }
    break;

  case 252:
/* Line 1787 of yacc.c  */
#line 1112 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = true; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; }
    break;

  case 253:
/* Line 1787 of yacc.c  */
#line 1113 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; }
    break;

  case 254:
/* Line 1787 of yacc.c  */
#line 1114 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PERCENTAGE; }
    break;

  case 255:
/* Line 1787 of yacc.c  */
#line 1115 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PX; }
    break;

  case 256:
/* Line 1787 of yacc.c  */
#line 1116 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CM; }
    break;

  case 257:
/* Line 1787 of yacc.c  */
#line 1117 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MM; }
    break;

  case 258:
/* Line 1787 of yacc.c  */
#line 1118 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_IN; }
    break;

  case 259:
/* Line 1787 of yacc.c  */
#line 1119 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PT; }
    break;

  case 260:
/* Line 1787 of yacc.c  */
#line 1120 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PC; }
    break;

  case 261:
/* Line 1787 of yacc.c  */
#line 1121 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DEG; }
    break;

  case 262:
/* Line 1787 of yacc.c  */
#line 1122 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_RAD; }
    break;

  case 263:
/* Line 1787 of yacc.c  */
#line 1123 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_GRAD; }
    break;

  case 264:
/* Line 1787 of yacc.c  */
#line 1124 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_TURN; }
    break;

  case 265:
/* Line 1787 of yacc.c  */
#line 1125 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MS; }
    break;

  case 266:
/* Line 1787 of yacc.c  */
#line 1126 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_S; }
    break;

  case 267:
/* Line 1787 of yacc.c  */
#line 1127 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_HZ; }
    break;

  case 268:
/* Line 1787 of yacc.c  */
#line 1128 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_KHZ; }
    break;

  case 269:
/* Line 1787 of yacc.c  */
#line 1129 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EMS; }
    break;

  case 270:
/* Line 1787 of yacc.c  */
#line 1130 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSParserValue::Q_EMS; }
    break;

  case 271:
/* Line 1787 of yacc.c  */
#line 1131 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EXS; }
    break;

  case 272:
/* Line 1787 of yacc.c  */
#line 1132 "./CSSGrammar.y"
    {
      (yyval.value).id = CSSValueInvalid;
      (yyval.value).fValue = (yyvsp[(1) - (1)].number);
      (yyval.value).unit = CSSPrimitiveValue::CSS_REMS;
      if (parser->m_styleSheet)
          parser->m_styleSheet->parserSetUsesRemUnits(true);
  }
    break;

  case 273:
/* Line 1787 of yacc.c  */
#line 1139 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CHS; }
    break;

  case 274:
/* Line 1787 of yacc.c  */
#line 1140 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VW; }
    break;

  case 275:
/* Line 1787 of yacc.c  */
#line 1141 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VH; }
    break;

  case 276:
/* Line 1787 of yacc.c  */
#line 1142 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VMIN; }
    break;

  case 277:
/* Line 1787 of yacc.c  */
#line 1143 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VMAX; }
    break;

  case 278:
/* Line 1787 of yacc.c  */
#line 1144 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DPPX; }
    break;

  case 279:
/* Line 1787 of yacc.c  */
#line 1145 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DPI; }
    break;

  case 280:
/* Line 1787 of yacc.c  */
#line 1146 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DPCM; }
    break;

  case 281:
/* Line 1787 of yacc.c  */
#line 1147 "./CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_FR; }
    break;

  case 282:
/* Line 1787 of yacc.c  */
#line 1150 "./CSSGrammar.y"
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[(1) - (4)].string);
        f->args = adoptPtr((yyvsp[(3) - (4)].valueList));
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
    break;

  case 283:
/* Line 1787 of yacc.c  */
#line 1158 "./CSSGrammar.y"
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[(1) - (3)].string);
        f->args = adoptPtr(new CSSParserValueList);
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
    break;

  case 284:
/* Line 1787 of yacc.c  */
#line 1166 "./CSSGrammar.y"
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[(1) - (4)].string);
        f->args = nullptr;
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
  }
    break;

  case 286:
/* Line 1787 of yacc.c  */
#line 1177 "./CSSGrammar.y"
    {
      (yyval.value).id = CSSValueInvalid;
      (yyval.value).unit = 0;
  }
    break;

  case 287:
/* Line 1787 of yacc.c  */
#line 1181 "./CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].integer); }
    break;

  case 288:
/* Line 1787 of yacc.c  */
#line 1184 "./CSSGrammar.y"
    {
        (yyval.character) = '+';
    }
    break;

  case 289:
/* Line 1787 of yacc.c  */
#line 1187 "./CSSGrammar.y"
    {
        (yyval.character) = '-';
    }
    break;

  case 290:
/* Line 1787 of yacc.c  */
#line 1190 "./CSSGrammar.y"
    {
        (yyval.character) = '*';
    }
    break;

  case 291:
/* Line 1787 of yacc.c  */
#line 1193 "./CSSGrammar.y"
    {
        (yyval.character) = '/';
    }
    break;

  case 294:
/* Line 1787 of yacc.c  */
#line 1199 "./CSSGrammar.y"
    {
        (yyval.valueList) = nullptr;
        if ((yyvsp[(3) - (5)].valueList)) {
            (yyval.valueList) = (yyvsp[(3) - (5)].valueList);
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = '(';
            (yyval.valueList)->insertValueAt(0, v);
            v.iValue = ')';
            (yyval.valueList)->addValue(v);
        }
    }
    break;

  case 296:
/* Line 1787 of yacc.c  */
#line 1213 "./CSSGrammar.y"
    { (yyval.valueList) = nullptr; delete (yyvsp[(1) - (2)].valueList); }
    break;

  case 297:
/* Line 1787 of yacc.c  */
#line 1215 "./CSSGrammar.y"
    {
        (yyval.valueList) = new CSSParserValueList;
        (yyval.valueList)->addValue((yyvsp[(1) - (1)].value));
    }
    break;

  case 298:
/* Line 1787 of yacc.c  */
#line 1219 "./CSSGrammar.y"
    {
        OwnPtr<CSSParserValueList> expression = adoptPtr((yyvsp[(1) - (3)].valueList));
        (yyval.valueList) = nullptr;
        if (expression && (yyvsp[(2) - (3)].character)) {
            (yyval.valueList) = expression.leakPtr();
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = (yyvsp[(2) - (3)].character);
            (yyval.valueList)->addValue(v);
            (yyval.valueList)->addValue((yyvsp[(3) - (3)].value));
        } else {
            destroy((yyvsp[(3) - (3)].value));
        }
    }
    break;

  case 299:
/* Line 1787 of yacc.c  */
#line 1234 "./CSSGrammar.y"
    {
        OwnPtr<CSSParserValueList> left = adoptPtr((yyvsp[(1) - (3)].valueList));
        OwnPtr<CSSParserValueList> right = adoptPtr((yyvsp[(3) - (3)].valueList));
        (yyval.valueList) = nullptr;
        if (left && (yyvsp[(2) - (3)].character) && right) {
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = (yyvsp[(2) - (3)].character);
            left->addValue(v);
            left->extend(*right);
            (yyval.valueList) = left.leakPtr();
        }
    }
    break;

  case 302:
/* Line 1787 of yacc.c  */
#line 1252 "./CSSGrammar.y"
    {
        OwnPtr<CSSParserValueList> list = adoptPtr((yyvsp[(1) - (5)].valueList));
        OwnPtr<CSSParserValueList> expression = adoptPtr((yyvsp[(4) - (5)].valueList));
        (yyval.valueList) = nullptr;
        if (list && expression) {
            (yyval.valueList) = list.leakPtr();
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = ',';
            (yyval.valueList)->addValue(v);
            (yyval.valueList)->extend(*expression);
        }
    }
    break;

  case 303:
/* Line 1787 of yacc.c  */
#line 1268 "./CSSGrammar.y"
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[(1) - (5)].string);
        f->args = adoptPtr((yyvsp[(3) - (5)].valueList));
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
    break;

  case 304:
/* Line 1787 of yacc.c  */
#line 1276 "./CSSGrammar.y"
    {
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = 0;
        YYERROR;
    }
    break;

  case 307:
/* Line 1787 of yacc.c  */
#line 1284 "./CSSGrammar.y"
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[(1) - (4)].string);
        f->args = adoptPtr((yyvsp[(3) - (4)].valueList));
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
    break;

  case 308:
/* Line 1787 of yacc.c  */
#line 1292 "./CSSGrammar.y"
    {
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = 0;
        YYERROR;
    }
    break;

  case 314:
/* Line 1787 of yacc.c  */
#line 1301 "./CSSGrammar.y"
    { parser->invalidBlockHit(); }
    break;

  case 326:
/* Line 1787 of yacc.c  */
#line 1307 "./CSSGrammar.y"
    { (yyval.location) = parser->currentLocation(); }
    break;


/* Line 1787 of yacc.c  */
#line 4779 "./CSSGrammar.cpp"
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
#line 1314 "./CSSGrammar.y"

