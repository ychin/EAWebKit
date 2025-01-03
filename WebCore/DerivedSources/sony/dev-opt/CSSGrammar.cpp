/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

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
#define yydebug         cssyydebug
#define yynerrs         cssyynerrs


/* Copy the first part of user declarations.  */
#line 1 "./CSSGrammar.y" /* yacc.c:339  */


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
#include "CSSKeyframeRule.h"
#include "CSSKeyframesRule.h"
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


#line 134 "./CSSGrammar.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
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
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int cssyydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    NTHCHILDSELECTORSEPARATOR = 270,
    HEX = 271,
    IDSEL = 272,
    IMPORT_SYM = 273,
    PAGE_SYM = 274,
    MEDIA_SYM = 275,
    FONT_FACE_SYM = 276,
    CHARSET_SYM = 277,
    KEYFRAME_RULE_SYM = 278,
    KEYFRAMES_SYM = 279,
    NAMESPACE_SYM = 280,
    WEBKIT_RULE_SYM = 281,
    WEBKIT_DECLS_SYM = 282,
    WEBKIT_VALUE_SYM = 283,
    WEBKIT_MEDIAQUERY_SYM = 284,
    WEBKIT_SIZESATTR_SYM = 285,
    WEBKIT_SELECTOR_SYM = 286,
    WEBKIT_REGION_RULE_SYM = 287,
    WEBKIT_VIEWPORT_RULE_SYM = 288,
    TOPLEFTCORNER_SYM = 289,
    TOPLEFT_SYM = 290,
    TOPCENTER_SYM = 291,
    TOPRIGHT_SYM = 292,
    TOPRIGHTCORNER_SYM = 293,
    BOTTOMLEFTCORNER_SYM = 294,
    BOTTOMLEFT_SYM = 295,
    BOTTOMCENTER_SYM = 296,
    BOTTOMRIGHT_SYM = 297,
    BOTTOMRIGHTCORNER_SYM = 298,
    LEFTTOP_SYM = 299,
    LEFTMIDDLE_SYM = 300,
    LEFTBOTTOM_SYM = 301,
    RIGHTTOP_SYM = 302,
    RIGHTMIDDLE_SYM = 303,
    RIGHTBOTTOM_SYM = 304,
    ATKEYWORD = 305,
    IMPORTANT_SYM = 306,
    MEDIA_ONLY = 307,
    MEDIA_NOT = 308,
    MEDIA_AND = 309,
    REMS = 310,
    CHS = 311,
    QEMS = 312,
    EMS = 313,
    EXS = 314,
    PXS = 315,
    CMS = 316,
    MMS = 317,
    INS = 318,
    PTS = 319,
    PCS = 320,
    DEGS = 321,
    RADS = 322,
    GRADS = 323,
    TURNS = 324,
    MSECS = 325,
    SECS = 326,
    HERTZ = 327,
    KHERTZ = 328,
    DIMEN = 329,
    INVALIDDIMEN = 330,
    PERCENTAGE = 331,
    FLOATTOKEN = 332,
    INTEGER = 333,
    VW = 334,
    VH = 335,
    VMIN = 336,
    VMAX = 337,
    DPPX = 338,
    DPI = 339,
    DPCM = 340,
    FR = 341,
    URI = 342,
    FUNCTION = 343,
    ANYFUNCTION = 344,
    NOTFUNCTION = 345,
    CALCFUNCTION = 346,
    MATCHESFUNCTION = 347,
    MAXFUNCTION = 348,
    MINFUNCTION = 349,
    NTHCHILDFUNCTIONS = 350,
    LANGFUNCTION = 351,
    DIRFUNCTION = 352,
    ROLEFUNCTION = 353,
    UNICODERANGE = 354,
    SUPPORTS_AND = 355,
    SUPPORTS_NOT = 356,
    SUPPORTS_OR = 357,
    SUPPORTS_SYM = 358,
    WEBKIT_SUPPORTS_CONDITION_SYM = 359,
    CUEFUNCTION = 360
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 65 "./CSSGrammar.y" /* yacc.c:355  */

    double number;
    CSSParserString string;
    CSSSelector::MarginBoxType marginBox;
    CSSParserValue value;
#line 202 "./CSSGrammar.y" /* yacc.c:355  */
 CSSParserSelectorCombinator relation; 
#line 204 "./CSSGrammar.y" /* yacc.c:355  */
 StyleRuleBase* rule; 
#line 207 "./CSSGrammar.y" /* yacc.c:355  */
 Vector<RefPtr<StyleRuleBase>>* ruleList; 
#line 212 "./CSSGrammar.y" /* yacc.c:355  */
 MediaQuerySet* mediaList; 
#line 215 "./CSSGrammar.y" /* yacc.c:355  */
 MediaQuery* mediaQuery; 
#line 218 "./CSSGrammar.y" /* yacc.c:355  */
 MediaQuery::Restrictor mediaQueryRestrictor; 
#line 220 "./CSSGrammar.y" /* yacc.c:355  */
 MediaQueryExp* mediaQueryExp; 
#line 223 "./CSSGrammar.y" /* yacc.c:355  */
 Vector<CSSParser::SourceSize>* sourceSizeList; 
#line 230 "./CSSGrammar.y" /* yacc.c:355  */
 Vector<std::unique_ptr<MediaQueryExp>>* mediaQueryExpList; 
#line 234 "./CSSGrammar.y" /* yacc.c:355  */
 StyleKeyframe* keyframe; 
#line 237 "./CSSGrammar.y" /* yacc.c:355  */
 Vector<RefPtr<StyleKeyframe>>* keyframeRuleList; 
#line 243 "./CSSGrammar.y" /* yacc.c:355  */
 CSSPropertyID id; 
#line 245 "./CSSGrammar.y" /* yacc.c:355  */
 CSSParserSelector* selector; 
#line 248 "./CSSGrammar.y" /* yacc.c:355  */
 Vector<std::unique_ptr<CSSParserSelector>>* selectorList; 
#line 252 "./CSSGrammar.y" /* yacc.c:355  */
 bool boolean; 
#line 254 "./CSSGrammar.y" /* yacc.c:355  */
 CSSSelector::Match match; 
#line 256 "./CSSGrammar.y" /* yacc.c:355  */
 int integer; 
#line 258 "./CSSGrammar.y" /* yacc.c:355  */
 char character; 
#line 260 "./CSSGrammar.y" /* yacc.c:355  */
 CSSParserValueList* valueList; 
#line 264 "./CSSGrammar.y" /* yacc.c:355  */
 Vector<CSSParserString>* stringList; 
#line 269 "./CSSGrammar.y" /* yacc.c:355  */
 CSSParser::Location location; 

#line 330 "./CSSGrammar.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int cssyyparse (CSSParser* parser);

#endif /* !YY_CSSYY_CSSGRAMMAR_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */
#line 71 "./CSSGrammar.y" /* yacc.c:358  */

static inline int cssyyerror(void*, const char*)
{
    return 1;
}
static inline CSSParserValue makeIdentValue(CSSParserString string)
{
    CSSParserValue v;
    v.id = cssValueKeywordID(string);
    v.unit = CSSPrimitiveValue::CSS_IDENT;
    v.string = string;
    return v;
}
static bool selectorListDoesNotMatchAnyPseudoElement(const Vector<std::unique_ptr<CSSParserSelector>>* selectorVector)
{
    if (!selectorVector)
        return true;
    for (unsigned i = 0; i < selectorVector->size(); ++i) {
        for (const CSSParserSelector* selector = selectorVector->at(i).get(); selector; selector = selector->tagHistory()) {
            if (selector->matchesPseudoElement())
                return false;
        }
    }
    return true;
}
EA_WEBKIT_CSS_GRAMMAR_START();

#line 373 "./CSSGrammar.cpp" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
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
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

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
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  27
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1911

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  126
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  142
/* YYNRULES -- Number of rules.  */
#define YYNRULES  396
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  735

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   360

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,   124,     2,   125,     2,     2,
     117,   113,    21,   118,   116,   121,    19,   123,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    18,   115,
       2,   122,   120,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    20,     2,   114,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   111,    22,   112,   119,     2,     2,     2,
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
      15,    16,    17,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   286,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   296,   297,   298,   300,   310,   312,   320,   321,   321,
     322,   322,   323,   323,   323,   324,   324,   325,   325,   326,
     326,   327,   327,   329,   335,   336,   338,   338,   339,   340,
     348,   349,   350,   351,   352,   353,   354,   355,   356,   359,
     363,   364,   365,   368,   369,   379,   380,   390,   391,   392,
     393,   394,   395,   397,   397,   397,   397,   397,   397,   399,
     405,   410,   413,   416,   422,   426,   432,   433,   434,   435,
     437,   437,   438,   438,   439,   439,   441,   446,   450,   456,
     459,   460,   460,   461,   468,   477,   481,   487,   490,   495,
     498,   501,   506,   510,   515,   515,   517,   522,   530,   537,
     542,   547,   552,   555,   558,   564,   567,   574,   577,   582,
     588,   593,   593,   593,   593,   594,   596,   597,   600,   601,
     604,   605,   606,   609,   625,   630,   634,   634,   636,   637,
     643,   645,   649,   657,   658,   670,   675,   680,   690,   694,
     700,   704,   711,   716,   721,   721,   723,   723,   730,   733,
     736,   739,   742,   745,   748,   751,   754,   757,   760,   763,
     766,   769,   772,   775,   780,   785,   788,   792,   798,   803,
     814,   815,   816,   817,   819,   819,   820,   820,   821,   823,
     828,   829,   831,   836,   838,   846,   856,   861,   862,   864,
     868,   868,   870,   874,   879,   885,   887,   888,   889,   898,
     907,   913,   914,   915,   918,   921,   926,   931,   934,   939,
     946,   953,   962,   968,   971,   977,   978,   985,   991,   998,
    1009,  1010,  1011,  1014,  1023,  1028,  1035,  1040,  1049,  1055,
    1059,  1062,  1065,  1068,  1071,  1074,  1078,  1078,  1080,  1084,
    1087,  1090,  1098,  1101,  1104,  1107,  1118,  1129,  1140,  1149,
    1158,  1173,  1188,  1203,  1212,  1221,  1238,  1254,  1255,  1256,
    1257,  1258,  1259,  1262,  1266,  1270,  1276,  1282,  1287,  1303,
    1304,  1309,  1312,  1317,  1318,  1319,  1319,  1321,  1325,  1331,
    1334,  1337,  1343,  1343,  1345,  1349,  1365,  1366,  1366,  1366,
    1368,  1369,  1370,  1371,  1372,  1373,  1374,  1375,  1376,  1377,
    1378,  1379,  1380,  1381,  1384,  1387,  1388,  1389,  1390,  1391,
    1392,  1393,  1394,  1395,  1396,  1397,  1398,  1399,  1400,  1401,
    1402,  1403,  1404,  1405,  1406,  1407,  1414,  1415,  1416,  1417,
    1418,  1419,  1420,  1421,  1422,  1425,  1433,  1441,  1451,  1452,
    1455,  1458,  1461,  1464,  1468,  1468,  1470,  1484,  1484,  1486,
    1490,  1505,  1519,  1522,  1523,  1539,  1547,  1553,  1553,  1555,
    1563,  1569,  1569,  1570,  1570,  1571,  1572,  1573,  1574,  1576,
    1576,  1576,  1576,  1576,  1576,  1576,  1576,  1576,  1577,  1577,
    1578,  1580,  1581,  1582,  1583,  1584,  1585
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "TOKEN_EOF", "error", "$undefined", "LOWEST_PREC", "UNIMPORTANT_TOK",
  "WHITESPACE", "SGML_CD", "INCLUDES", "DASHMATCH", "BEGINSWITH",
  "ENDSWITH", "CONTAINS", "STRING", "IDENT", "NTH",
  "NTHCHILDSELECTORSEPARATOR", "HEX", "IDSEL", "':'", "'.'", "'['", "'*'",
  "'|'", "IMPORT_SYM", "PAGE_SYM", "MEDIA_SYM", "FONT_FACE_SYM",
  "CHARSET_SYM", "KEYFRAME_RULE_SYM", "KEYFRAMES_SYM", "NAMESPACE_SYM",
  "WEBKIT_RULE_SYM", "WEBKIT_DECLS_SYM", "WEBKIT_VALUE_SYM",
  "WEBKIT_MEDIAQUERY_SYM", "WEBKIT_SIZESATTR_SYM", "WEBKIT_SELECTOR_SYM",
  "WEBKIT_REGION_RULE_SYM", "WEBKIT_VIEWPORT_RULE_SYM",
  "TOPLEFTCORNER_SYM", "TOPLEFT_SYM", "TOPCENTER_SYM", "TOPRIGHT_SYM",
  "TOPRIGHTCORNER_SYM", "BOTTOMLEFTCORNER_SYM", "BOTTOMLEFT_SYM",
  "BOTTOMCENTER_SYM", "BOTTOMRIGHT_SYM", "BOTTOMRIGHTCORNER_SYM",
  "LEFTTOP_SYM", "LEFTMIDDLE_SYM", "LEFTBOTTOM_SYM", "RIGHTTOP_SYM",
  "RIGHTMIDDLE_SYM", "RIGHTBOTTOM_SYM", "ATKEYWORD", "IMPORTANT_SYM",
  "MEDIA_ONLY", "MEDIA_NOT", "MEDIA_AND", "REMS", "CHS", "QEMS", "EMS",
  "EXS", "PXS", "CMS", "MMS", "INS", "PTS", "PCS", "DEGS", "RADS", "GRADS",
  "TURNS", "MSECS", "SECS", "HERTZ", "KHERTZ", "DIMEN", "INVALIDDIMEN",
  "PERCENTAGE", "FLOATTOKEN", "INTEGER", "VW", "VH", "VMIN", "VMAX",
  "DPPX", "DPI", "DPCM", "FR", "URI", "FUNCTION", "ANYFUNCTION",
  "NOTFUNCTION", "CALCFUNCTION", "MATCHESFUNCTION", "MAXFUNCTION",
  "MINFUNCTION", "NTHCHILDFUNCTIONS", "LANGFUNCTION", "DIRFUNCTION",
  "ROLEFUNCTION", "UNICODERANGE", "SUPPORTS_AND", "SUPPORTS_NOT",
  "SUPPORTS_OR", "SUPPORTS_SYM", "WEBKIT_SUPPORTS_CONDITION_SYM",
  "CUEFUNCTION", "'{'", "'}'", "')'", "']'", "';'", "','", "'('", "'+'",
  "'~'", "'>'", "'-'", "'='", "'/'", "'#'", "'%'", "$accept", "stylesheet",
  "webkit_rule", "webkit_keyframe_rule", "webkit_decls", "webkit_value",
  "webkit_mediaquery", "webkit_selector", "webkit_supports_condition",
  "space", "maybe_space", "maybe_sgml", "maybe_charset", "closing_brace",
  "closing_parenthesis", "closing_bracket", "charset", "ignored_charset",
  "rule_list", "valid_rule", "rule", "block_rule_list",
  "block_valid_rule_list", "block_valid_rule", "block_rule",
  "at_import_header_end_maybe_space", "before_import_rule", "import",
  "namespace", "maybe_ns_prefix", "string_or_uri", "maybe_media_value",
  "webkit_source_size_list", "source_size_list",
  "maybe_source_media_query_exp", "source_size_length",
  "base_media_query_exp", "media_query_exp", "media_query_exp_list",
  "maybe_and_media_query_exp_list", "maybe_media_restrictor",
  "media_query", "maybe_media_list", "media_list", "at_rule_body_start",
  "before_media_rule", "at_rule_header_end_maybe_space", "media",
  "supports", "supports_error", "before_supports_rule",
  "at_supports_rule_header_end", "supports_condition", "supports_negation",
  "supports_conjunction", "supports_disjunction",
  "supports_condition_in_parens", "supports_declaration_condition",
  "before_keyframes_rule", "keyframes", "keyframe_name", "keyframes_rule",
  "keyframe_rule", "key_list", "key", "before_page_rule", "page",
  "page_selector", "declarations_and_margins", "margin_box", "$@1",
  "margin_sym", "before_font_face_rule", "font_face", "before_region_rule",
  "region", "combinator", "maybe_unary_operator", "unary_operator",
  "maybe_space_before_declaration", "before_selector_list",
  "at_rule_header_end", "at_selector_end", "ruleset",
  "before_selector_group_item", "selector_list",
  "before_nested_selector_list", "after_nested_selector_list",
  "nested_selector_list", "lang_range", "comma_separated_lang_ranges",
  "complex_selector_with_trailing_whitespace", "complex_selector",
  "namespace_selector", "compound_selector", "simple_selector_list",
  "element_name", "specifier_list", "specifier", "class", "attrib",
  "attrib_flags", "match", "ident_or_string", "pseudo_page",
  "nth_selector_ending", "pseudo", "declaration_list", "decl_list",
  "decl_list_recovery", "declaration", "declaration_recovery", "property",
  "priority", "ident_list", "track_names_list", "expr", "valid_expr",
  "expr_recovery", "operator", "term", "unary_term", "function",
  "calc_func_term", "calc_func_operator", "calc_maybe_space",
  "calc_func_paren_expr", "calc_func_expr", "valid_calc_func_expr",
  "calc_func_expr_list", "calc_function", "min_or_max",
  "min_or_max_function", "save_block", "invalid_at", "invalid_rule",
  "invalid_block", "invalid_square_brackets_block",
  "invalid_parentheses_block", "opening_parenthesis", "error_location",
  "error_recovery", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,    58,    46,
      91,    42,   124,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,   352,   353,   354,   355,   356,   357,   358,   359,
     360,   123,   125,    41,    93,    59,    44,    40,    43,   126,
      62,    45,    61,    47,    35,    37
};
# endif

#define YYPACT_NINF -564

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-564)))

#define YYTABLE_NINF -358

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-358)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     813,   -38,   -25,   -16,    -5,   106,   133,   213,   329,   101,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,    72,  -564,  -564,
    -564,  -564,  -564,  -564,   231,  -564,  -564,  -564,   349,   349,
     349,   349,   349,   349,   349,  -564,   108,  -564,  -564,   349,
     144,  1421,   349,   279,  1004,   541,  -564,  -564,  1751,  -564,
    1093,   235,   295,   296,   377,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,   284,  -564,   633,  -564,   344,  -564,  -564,   360,
    -564,  -564,   289,   436,  -564,   448,  -564,   472,  -564,   473,
    -564,  1235,  -564,  -564,  -564,  -564,   399,  1521,   370,   402,
      78,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,  1788,  -564,   417,   693,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,    57,  -564,   431,
     563,   315,  -564,  -564,  -564,   349,   519,  -564,  -564,  1330,
     540,  -564,   533,  -564,    50,  1235,   407,  1295,  -564,   810,
     438,  -564,  -564,  -564,  -564,  -564,   102,   467,  -564,   480,
     493,    17,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  1676,
     151,   335,  -564,   363,   581,   324,   635,  -564,   129,  -564,
     612,   425,  -564,    49,  -564,   349,   482,  -564,   488,   513,
    -564,  -564,  -564,  -564,  -564,   349,   349,   349,   374,   349,
     349,   819,  1100,   349,   349,   349,  -564,  -564,   349,  -564,
    -564,  -564,  -564,  -564,  1402,   349,   349,   349,  1100,   349,
      99,   281,  -564,  -564,  -564,  -564,  -564,  -564,   427,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,   602,  -564,
    -564,  -564,   539,  -564,  -564,  -564,  -564,  -564,   548,  1235,
    -564,  -564,   810,   547,   683,  -564,  -564,   113,   558,   286,
    -564,  -564,  -564,  -564,  -564,   494,   332,   569,  -564,   688,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,    27,  -564,   499,   349,    27,   526,   542,   559,   235,
    -564,   614,   562,   305,   570,   349,   580,  1093,   539,  1192,
    -564,  -564,  -564,   349,   349,  -564,   915,  -564,  -564,  -564,
    -564,    68,    41,  -564,  -564,  -564,    76,    76,  -564,  1820,
      76,  -564,  -564,  -564,   687,   325,   349,   349,  -564,   349,
     349,  -564,    76,   687,   121,  -564,  -564,  -564,    84,   141,
    -564,  -564,   300,  1093,   349,   349,   381,   638,   596,   639,
     519,   533,   682,  -564,   349,   349,  -564,   349,  -564,   835,
    -564,  -564,   583,   686,   113,   113,   113,   113,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,   327,   571,   377,   404,   144,   349,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,    96,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,   689,   695,  -564,  -564,  -564,
    -564,  -564,   158,   599,   349,   349,  1227,  -564,   655,   349,
    -564,  -564,  -564,  -564,  -564,  1322,  -564,  -564,   118,   169,
    1689,    31,  -564,  1103,  -564,   240,  -564,  -564,   541,   337,
     349,  -564,   601,  1751,  1093,  -564,  -564,   629,  -564,   163,
    1235,  -564,  -564,    82,    82,   634,  -564,  -564,  -564,   346,
    -564,  -564,   498,  -564,   349,   349,  -564,  -564,  -564,  -564,
    -564,  -564,  1037,  1262,  -564,  -564,  -564,  -564,   177,  -564,
     507,   112,  -564,  -564,   613,  -564,   617,   618,  -564,  -564,
     619,  -564,   637,  -564,  -564,   821,   349,   687,  -564,   745,
     745,  -564,  -564,  -564,  -564,  -564,  1322,  -564,  -564,  -564,
    -564,  1004,  -564,  -564,   163,   236,   302,  -564,  -564,   364,
     357,   355,   408,  -564,  -564,   373,  -564,  -564,    82,  -564,
     419,   409,   456,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,   711,  1235,   349,  1004,  -564,  -564,   338,  -564,  -564,
    -564,    69,    56,   541,  -564,   349,  -564,  -564,  -564,   247,
    -564,  -564,   349,  -564,    31,   746,   746,   349,   349,   687,
     141,   541,  -564,   429,  -564,  -564,   459,  -564,  -564,  -564,
    -564,  -564,   745,  -564,  -564,  -564,  -564,  -564,  -564,   649,
    -564,  -564,   462,   655,  -564,  -564,  -564,  -564,  -564,  -564,
    1636,  -564,  -564,  -564,   177,  -564,   247,  -564,   240,    57,
     349,  -564,  -564,  1093,   746,   638,  -564,  -564,  -564,   649,
     660,   349,   333,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,   349,   349,
    1578,  -564,   349,   177,  -564,  -564,  -564,   651,  -564,  -564,
    1636,  -564,   377,  1636,   272,   919,  -564,  1029,  -564,   464,
    -564,   673,   651,   349,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,
    -564,  -564,  -564,  -564,  -564,  -564,  -564,   177,  -564,  -564,
    -564,   349,  -564,   674,   349,  1502,  -564,   377,  -564,  -564,
     131,  -564,   404,   177,  -564
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      20,    20,    20,    20,    20,    20,    20,    25,    20,    20,
      20,    20,    20,    20,    89,    20,    20,     1,     3,     9,
       4,     5,     6,     8,    10,    21,     0,    22,    26,     7,
       0,   189,   188,     0,     0,    99,    20,    20,     0,    20,
       0,     0,     0,     0,    38,   145,   144,   187,   186,    20,
      20,   141,     0,   184,     0,    20,     0,    46,    45,     0,
      41,    48,     0,     0,    44,     0,    42,     0,    43,     0,
      47,     0,    40,   391,    20,    20,     0,     0,   271,   268,
       0,    20,    20,    20,    20,   335,   336,   333,   332,   334,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,    20,   317,   316,   315,   337,   338,
     339,   340,   341,   342,   343,   344,    20,    20,    20,   368,
     367,    20,    20,    20,     0,    20,     0,     0,   294,    20,
      20,    20,    20,    20,   100,   101,    95,   102,    20,     0,
       0,     0,    87,    91,    92,    90,   223,   229,   228,     0,
       0,    20,   224,   211,     0,   207,     0,     0,   206,   214,
       0,   225,   230,   231,   232,    20,     0,     0,   122,   123,
     124,   121,   131,   392,    35,    34,    20,    24,    23,     0,
       0,     0,   143,     0,    80,     0,     0,    20,     0,    20,
       0,     0,    20,     0,   392,   284,     0,    13,   272,   269,
      20,    20,   391,    20,   279,   302,   303,   308,     0,   304,
     306,     0,     0,   307,   309,   313,    20,    20,   314,    14,
     391,    20,    20,   293,     0,   300,   310,   311,     0,   312,
       0,     0,    15,    20,    86,    20,   213,   252,     0,    20,
      20,    20,    20,    20,    20,    20,    20,   233,     0,   212,
     196,    16,     0,   208,   210,   205,    20,    20,    20,     0,
     223,   224,   217,     0,     0,   227,   226,     0,     0,     0,
      17,    20,    20,    20,    20,     0,     0,     0,    20,     0,
      50,    49,    22,    52,    51,    12,    20,    20,    79,    78,
      20,     0,    11,     0,    69,     0,    99,     0,     0,     0,
     116,     0,     0,   153,     0,   111,     0,     0,   190,     0,
     281,    20,    20,   274,   273,   392,     0,    32,    20,    31,
     289,     0,     0,    30,    29,   346,     0,     0,    20,     0,
       0,   348,   359,   362,   354,     0,   305,   301,   392,   298,
     297,   295,     0,   354,     0,    20,    20,    20,    84,    89,
     253,    20,   185,     0,   197,   197,   185,     0,     0,     0,
      20,     0,     0,    20,   180,   181,    20,   182,   209,     0,
     125,   132,     0,     0,     0,     0,     0,     0,    28,   393,
     392,   380,   385,   386,   381,   382,   383,   384,   387,   388,
     389,   390,    27,   379,   376,   394,   395,   396,   392,    33,
     375,     0,     0,    39,     0,     0,    81,    82,    83,    20,
      74,    75,    20,   106,     0,   109,   114,   117,   118,   120,
     137,   136,    20,   149,   148,   150,     0,    20,   152,   177,
     176,   109,     0,     0,   276,   275,     0,   282,   286,   287,
      20,   290,   291,   345,   347,     0,   349,   366,   355,     0,
       0,     0,   358,   296,   370,   363,    20,   369,    99,    97,
      94,    20,     0,     0,     0,    20,    20,     0,   220,     0,
       0,    20,    20,     0,     0,     0,   201,   200,   202,     0,
      20,    20,     0,    20,   193,   183,    20,    20,   127,   129,
     126,   128,     0,     0,    20,    37,   374,   373,     0,   142,
       0,   104,   108,    20,     0,    20,     0,     0,   151,   248,
       0,    20,     0,   109,    20,     0,   288,   354,    19,     0,
       0,   360,   361,    20,    20,   365,     0,    96,    20,    20,
     103,     0,    93,    88,     0,     0,     0,    20,   222,     0,
       0,     0,     0,    18,   249,     0,   262,   260,     0,   204,
       0,     0,     0,   241,   242,   243,   244,   245,   234,   240,
      20,     0,     0,   130,     0,   377,   378,     0,   140,    76,
      77,     0,     0,    99,   109,    53,   109,   109,   109,     0,
     109,    20,   285,   280,     0,   350,   351,   352,   353,   354,
       0,    99,    20,     0,   265,   263,     0,   255,    20,   199,
     266,   256,     0,   250,   261,   257,    20,   258,   259,     0,
     236,    20,     0,   286,    36,    72,    71,    73,   107,    20,
       0,    20,    20,    20,     0,    20,     0,   356,   364,    98,
      85,   254,   264,     0,   197,     0,   247,   246,    20,     0,
       0,    53,     0,   371,    63,    22,    67,    66,    60,    62,
      61,    58,    59,    68,    57,   113,    65,    64,    53,   138,
       0,   175,    55,     0,   221,    20,   203,   239,    20,    20,
       0,   372,    54,     0,     0,     0,   154,     0,   192,     0,
      20,     0,   239,   133,   112,   115,   135,    20,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   147,    20,   156,     0,    22,   179,
     251,   238,   235,     0,   139,     0,    20,    56,   237,   155,
       0,    20,     0,     0,   157
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -564,  -213,
       0,  -283,  -564,  -284,  -282,  -297,  -564,  -564,  -564,   604,
    -564,  -563,  -564,    65,  -564,  -564,  -564,    -8,    -4,  -564,
     485,  -564,  -564,  -564,   439,   331,  -234,   339,   199,  -564,
    -438,   -43,  -564,   299,  -370,  -564,  -173,    -1,     2,  -564,
    -564,  -564,  -201,  -564,  -564,  -564,   175,  -564,  -564,     3,
    -564,  -564,   128,  -564,   398,  -564,     4,  -564,  -564,  -564,
    -564,  -564,  -564,    20,  -564,    33,  -564,   321,   -35,  -210,
    -564,  -344,   620,    34,  -564,   -73,  -564,  -564,  -338,   170,
    -564,  -564,   262,   579,  -159,   368,   671,   361,  -122,  -564,
    -564,   164,   287,   194,   415,  -446,  -564,  -361,  -564,   770,
     772,  -189,   597,   254,  -564,  -564,  -197,  -564,  -155,  -564,
     644,   -44,  -564,   453,  -564,  -321,   454,  -217,  -564,  -564,
     -45,  -564,  -564,   -11,   710,   732,   -10,  -564,  -564,  -564,
    -130,  -175
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     9,    10,    11,    12,    13,    14,    15,    16,   459,
      42,    54,    37,   653,   335,   330,    38,   290,   189,    65,
     292,   630,   687,   654,   655,   305,    66,   656,   657,   301,
     419,   472,    18,    47,    48,   152,    49,   146,   147,   540,
     148,   423,   581,   424,   515,    69,   307,   658,   659,   310,
      72,   516,   177,   178,   179,   180,   181,   182,    73,   660,
     432,   684,    59,    60,    61,    75,   661,   437,   685,   715,
     726,   716,    77,   662,    79,   663,   269,    62,   134,    43,
      81,   443,   262,   664,   572,   164,   480,   609,   481,   488,
     489,   165,   166,   167,   168,   479,   169,   170,   171,   172,
     173,   691,   570,   648,   438,   556,   174,    86,    87,    88,
      89,   214,    90,   525,   331,   135,   136,   137,   233,   234,
     138,   139,   140,   342,   460,   461,   343,   344,   345,   354,
     141,   142,   143,   665,   666,   667,   405,   406,   407,   408,
     204,   285
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      17,   404,   149,   154,   153,    63,   263,   357,   203,   413,
      28,    29,    30,    31,    32,    33,    34,   320,    39,    40,
      41,   353,    44,    45,   336,    50,    51,   482,   316,   319,
     538,   333,   465,    67,   451,   452,    53,    68,   557,   417,
      70,   327,   185,    71,    74,    76,   150,   151,   276,   155,
     260,   260,   533,   508,   453,   454,  -105,   512,   457,   190,
     191,    78,   -20,   332,   194,   195,   337,   340,   327,   625,
     464,   521,   467,    19,    80,    82,   333,    35,   382,   212,
     514,   450,   325,   352,   205,   206,    20,   553,   680,    35,
     227,   215,   216,   217,   218,    21,   213,   512,   522,    36,
     348,    27,   471,   278,    35,   683,    22,   -20,   429,    52,
     378,    23,   614,   -20,   219,   -20,   -20,    35,    35,   418,
     -20,   333,   283,   -18,   284,   -99,   220,   221,   222,   448,
     308,   223,   224,   225,   -20,   228,    35,   447,    24,   235,
     236,   237,   238,   239,   334,    55,    35,   240,   241,    35,
     446,   276,   276,   591,   534,   329,    35,    56,   355,   260,
    -191,   258,   261,   538,   548,  -191,  -191,  -105,   -20,   144,
     145,  -105,   513,   463,   528,   277,   279,   388,   341,   535,
     183,    67,   329,   299,   626,    68,   286,   339,    70,   334,
     462,    71,    74,    76,   341,   554,   304,   306,   309,   311,
     313,   315,   317,   339,   478,   575,   594,  -190,   -20,    78,
     323,   324,   513,   326,   629,   502,   631,   632,   633,   -20,
     635,   576,    80,    82,   578,  -185,   346,   347,   634,   -99,
     176,   349,   350,   503,   334,   -20,   -18,   466,   527,   -18,
      35,    35,   731,   358,   442,   359,   -20,  -267,    83,   362,
     363,   364,   365,   366,   367,   368,   369,   276,    46,   517,
      84,   533,    57,   295,   520,    58,   374,   375,   377,  -190,
     555,   555,   388,    55,  -191,   673,   -20,   410,   638,   -20,
      83,   384,   385,   386,   387,    56,    35,   529,   411,   402,
     530,    35,    84,   421,   356,   456,   414,   415,   428,    84,
     416,    35,   434,    85,   440,    35,   675,    35,   186,   686,
      35,   589,   637,   475,   476,   478,   595,   596,   435,   599,
      35,   444,   445,   436,    25,  -357,   230,    63,   449,    35,
    -357,    63,    35,   388,    26,    85,   593,    35,   455,   504,
      35,   175,    35,    35,   602,   555,  -357,   559,    46,   604,
     671,   -20,   176,  -185,    35,   468,   469,   470,   260,  -267,
      35,   474,  -198,   534,   729,   192,   357,   196,   681,    35,
     492,   733,   682,   494,   327,   230,   495,   623,   528,    35,
      63,   636,   187,   188,   402,   197,    35,   328,   612,   688,
      57,  -267,   175,    58,   483,   484,   539,   198,    46,   644,
     696,   714,   507,   176,  -267,    83,   183,   550,   264,    35,
     184,   341,   265,    35,    35,   605,   341,    84,    57,   510,
     339,    58,   511,   670,    35,   339,   314,   244,   154,   153,
     -20,   245,   315,   681,    35,   727,   302,   315,  -357,   275,
     360,  -357,   505,  -216,   183,   402,   296,   409,  -357,   734,
     526,   297,   380,   624,   157,   158,   159,   160,   161,   -20,
      85,    35,   -20,   264,    35,   199,   536,   265,   610,    35,
    -198,   541,   200,  -191,   183,   545,   546,   607,   298,   549,
     608,   551,   552,   212,   674,   210,   613,    35,   329,   560,
     561,   562,   341,   571,   388,   389,   573,   574,   201,    57,
     580,   339,    58,    35,   577,   563,   564,   565,   566,   567,
     202,   207,    35,   583,   390,   585,  -267,   211,  -194,  -194,
    -194,   611,   617,  -194,   592,   266,   267,   268,   273,   229,
     274,    35,   615,   597,   598,   616,   -20,   361,   600,   601,
     628,   246,   641,   242,   603,   608,    35,   606,   275,  -216,
    -216,  -216,  -219,   257,  -216,   259,  -216,  -216,  -216,   498,
     499,   500,   501,   157,   158,   159,   160,   161,    35,   618,
     619,   627,   642,  -195,  -195,  -195,   243,   720,  -195,   280,
     266,   267,   268,   144,   145,   281,    35,   391,   392,   393,
     394,   395,   396,   397,   300,   398,   399,   400,   144,   145,
     282,    35,   640,   321,   401,   183,   402,    35,   643,   490,
     183,   403,   568,   312,   420,   370,   645,   -20,   183,    35,
     569,   649,   579,   371,   163,   -20,   430,   431,   322,   651,
     -20,   668,   669,   379,   193,   672,   303,  -111,   -20,   240,
     -20,  -111,   410,    35,    35,   -20,   -20,   -20,   677,    63,
     486,   487,   491,   425,    35,   373,    35,   426,  -219,  -219,
    -219,   646,   647,  -219,   690,  -219,  -219,  -219,   376,   694,
     183,   381,   695,   183,   427,   689,   719,   433,   692,   693,
     183,   183,   183,   477,   275,   439,   506,   485,  -215,   412,
     721,   441,   458,  -292,   230,   493,   496,   724,  -292,   157,
     158,   159,   160,   161,   497,  -299,  -299,   436,   519,  -299,
     523,   524,   547,  -299,   542,   725,    35,   558,   563,   564,
     565,   566,   567,   -20,   584,   -20,   730,   -20,   586,   587,
     588,   732,  -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,
    -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,   590,  -292,
     553,   528,   718,  -299,  -299,  -299,  -299,  -299,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,
    -299,  -299,  -299,   679,  -299,  -299,  -299,  -299,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,   722,   728,  -299,
     422,  -299,  -299,   291,  -215,  -215,  -215,  -299,   473,  -215,
     639,  -215,  -215,  -215,   543,  -292,  -292,   537,  -292,   231,
     582,  -299,   697,   509,  -299,   676,   232,  -299,  -299,   333,
     230,  -278,   212,   318,    35,   620,   157,   158,   159,   160,
     161,    91,    92,   569,   622,    93,   275,   372,   272,    94,
    -218,     1,   544,   678,     2,     3,     4,     5,     6,     7,
     518,   157,   158,   159,   160,   161,   723,   208,   621,   209,
    -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,
    -278,  -278,  -278,  -278,  -278,  -278,   383,   650,   351,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   293,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   531,   532,   128,   212,   129,   130,   388,
      35,   294,     8,   131,     0,     0,     0,    91,    92,     0,
       0,    93,   334,  -278,     0,    94,  -278,    57,     0,     0,
      58,     0,     0,   132,   133,     0,  -218,  -218,  -218,     0,
       0,  -218,     0,  -218,  -218,  -218,     0,     0,   698,   699,
     700,   701,   702,   703,   704,   705,   706,   707,   708,   709,
     710,   711,   712,   713,     0,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,    35,
       0,   128,     0,   129,   130,     0,    91,    92,     0,   131,
      93,     0,     0,     0,    94,     0,     0,     0,     0,   388,
     717,   402,     0,    57,     0,     0,    58,   327,   389,   132,
     133,     0,  -189,     0,     0,  -189,  -189,  -189,  -189,  -189,
    -189,  -189,     0,  -146,  -110,  -174,     0,   390,  -134,     0,
       0,     0,     0,     0,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,     0,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,    35,     0,
     128,   230,   129,   130,  -358,    35,   156,     0,   131,   157,
     158,   159,   160,   161,   162,   163,     0,     0,     0,     0,
       0,     0,    57,   390,     0,    58,     0,     0,   132,   133,
     391,   392,   393,   394,   395,   396,   397,  -119,   398,   399,
     400,   402,     0,     0,     0,     0,     0,   401,   183,     0,
       0,   329,     0,     0,   403,     0,     0,     0,     0,     0,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,     0,
       0,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,  -277,   389,     0,     0,   391,   392,   393,   394,
     395,   396,   397,     0,   398,   399,   400,     0,     0,     0,
       0,     0,   390,   401,   183,     0,     0,   338,    57,     0,
     403,    58,     0,     0,     0,     0,     0,  -283,   389,     0,
       0,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,   390,   156,     0,
       0,   157,   158,   159,   160,   161,   162,   163,     0,     0,
       0,     0,   333,   389,     0,     0,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,   390,     0,     0,   391,   392,   393,   394,   395,
     396,   397,     0,   398,   399,   400,     0,     0,     0,     0,
       0,     0,   401,   183,  -277,     0,     0,  -277,   270,   403,
       0,   157,   158,   159,   160,   161,   271,     0,     0,     0,
     391,   392,   393,   394,   395,   396,   397,    35,   398,   399,
     400,     0,     0,     0,     0,     0,     0,   401,   183,  -283,
       0,     0,  -283,   247,   403,     0,     0,     0,   248,     0,
       0,     0,     0,     0,     0,   391,   392,   393,   394,   395,
     396,   397,     0,   398,   399,   400,     0,     0,     0,     0,
       0,     0,   401,   183,     0,   334,     0,     0,     0,   403,
       0,     0,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,     0,     0,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,    91,    92,     0,     0,    93,     0,
       0,     0,    94,   249,   250,   251,    35,   252,     0,     0,
     253,   254,   255,   256,     0,     0,     0,     0,     0,   338,
      57,     0,     0,    58,   -70,  -146,  -110,  -174,     0,     0,
    -134,    64,     0,     0,     0,     0,     0,     0,  -178,     0,
       0,     0,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,     0,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,     0,     0,   128,     0,
     129,   130,  -267,    83,     0,     0,   131,    35,     0,     0,
       0,     0,     0,     0,     0,    84,     0,     0,     0,     0,
      57,  -270,    83,    58,     0,     0,   132,   133,     0,  -119,
       0,     0,     0,     0,    84,     0,     0,     0,     0,     0,
       0,  -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,
    -267,  -267,  -267,  -267,  -267,  -267,  -267,     0,    85,     0,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,     0,    85,  -267,    83,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    84,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -267,     0,     0,  -267,  -267,  -267,
    -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,  -267,
    -267,  -267,  -267,  -270,    85,     0,   388,   652,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -189,
       0,     0,  -189,  -189,  -189,  -189,  -189,  -189,  -189,   -70,
    -146,  -110,  -174,     0,     0,  -134,    64,     0,     0,     0,
       0,     0,     0,  -178,     0,     0,    -2,   287,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -189,
    -267,   289,  -189,  -189,  -189,  -189,  -189,  -189,  -189,   -70,
    -146,  -110,  -174,   288,     0,  -134,    64,     0,     0,     0,
       0,     0,     0,  -178,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   289,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -119,     0,     0,     0,   402,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,     0,     0,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,     0,     0,     0,  -119,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   338,    57,     0,     0,
      58,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
       0,     0,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,     0,     0,     0,     0,   128,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   226,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,     0,
       0,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125
};

static const yytype_int16 yycheck[] =
{
       0,   285,    45,    48,    48,    40,   165,   241,    81,   292,
      10,    11,    12,    13,    14,    15,    16,   206,    18,    19,
      20,   238,    22,    23,   221,    25,    26,   365,   201,   204,
     468,     0,   353,    41,   331,   332,    36,    41,   484,    12,
      41,     0,    52,    41,    41,    41,    46,    47,   170,    49,
       1,     1,    21,   414,   336,   337,     0,     1,   340,    59,
      60,    41,     5,   218,    64,    65,   221,   222,     0,     0,
     352,   441,   354,   111,    41,    41,     0,     5,   279,     1,
     424,    13,   212,   238,    84,    85,   111,     5,   651,     5,
     134,    91,    92,    93,    94,   111,    18,     1,   442,    27,
     230,     0,    18,     1,     5,   668,   111,     5,   309,     1,
     269,     5,   558,     5,   114,    13,    59,     5,     5,    92,
      12,     0,   105,     5,   107,    13,   126,   127,   128,   326,
       1,   131,   132,   133,     5,   135,     5,   326,     5,   139,
     140,   141,   142,   143,   113,     1,     5,   147,   148,     5,
     325,   273,   274,   523,   123,   114,     5,    13,    59,     1,
     111,   161,   112,   601,     1,   116,   116,   111,     5,    57,
      58,   115,   116,   348,     5,   175,   176,     0,   222,   461,
     111,   189,   114,   193,   115,   189,   186,   222,   189,   113,
     345,   189,   189,   189,   238,   113,   196,   197,   198,   199,
     200,   201,   202,   238,   363,   502,   527,   111,   106,   189,
     210,   211,   116,   213,   584,   390,   586,   587,   588,   117,
     590,   503,   189,   189,   508,    81,   226,   227,   589,   117,
     117,   231,   232,   408,   113,   106,   118,   116,   455,   121,
       5,     5,   111,   243,   317,   245,   117,     0,     1,   249,
     250,   251,   252,   253,   254,   255,   256,   379,   117,   432,
      13,    21,   118,   112,   437,   121,   266,   267,   268,   111,
     483,   484,     0,     1,   116,   636,   113,   287,   599,   116,
       1,   281,   282,   283,   284,    13,     5,   118,   288,   112,
     121,     5,    13,   303,    13,   339,   296,   297,   308,    13,
     300,     5,   312,    56,   314,     5,   644,     5,    12,   670,
       5,   521,   594,    13,    14,   474,   529,   530,    13,   536,
       5,   321,   322,    18,   111,     0,     1,   362,   328,     5,
       5,   366,     5,     0,     5,    56,   525,     5,   338,    12,
       5,   106,     5,     5,   541,   558,    21,     1,   117,   113,
     634,     5,   117,    81,     5,   355,   356,   357,     1,   112,
       5,   361,     5,   123,   725,    81,   600,    23,   652,     5,
     370,   732,   655,   373,     0,     1,   376,   574,     5,     5,
     415,   591,     5,     6,   112,    25,     5,    13,    15,   673,
     118,   112,   106,   121,    13,    14,    59,   108,   117,   612,
     684,   685,   412,   117,     0,     1,   111,   480,     1,     5,
     115,   455,     5,     5,     5,   113,   460,    13,   118,   419,
     455,   121,   422,   633,     5,   460,     1,   112,   473,   473,
       5,   116,   432,   717,     5,   718,   112,   437,   113,     1,
      13,   116,   115,     5,   111,   112,   111,   115,   123,   733,
     450,   116,   277,   115,    16,    17,    18,    19,    20,   113,
      56,     5,   116,     1,     5,    29,   466,     5,   113,     5,
     113,   471,    24,   116,   111,   475,   476,   113,   115,   479,
     116,   481,   482,     1,   643,   115,   113,     5,   114,   489,
     490,   491,   536,   493,     0,     1,   496,   497,    26,   118,
     510,   536,   121,     5,   504,     7,     8,     9,    10,    11,
      37,   112,     5,   513,    20,   515,   112,   115,   111,   112,
     113,   113,   113,   116,   524,   118,   119,   120,   167,   112,
     169,     5,   113,   533,   534,   116,   111,   110,   538,   539,
     583,    22,   113,   112,   544,   116,     5,   547,     1,   111,
     112,   113,     5,    13,   116,    22,   118,   119,   120,   384,
     385,   386,   387,    16,    17,    18,    19,    20,     5,   113,
     570,   581,   113,   111,   112,   113,    13,   113,   116,   112,
     118,   119,   120,    57,    58,   105,     5,    93,    94,    95,
      96,    97,    98,    99,    13,   101,   102,   103,    57,    58,
     107,     5,   602,   115,   110,   111,   112,     5,   608,    13,
     111,   117,   114,     1,   115,    13,   616,     5,   111,     5,
     122,   621,   115,    21,    22,    13,    12,    13,   115,   629,
      18,   631,   632,   272,     1,   635,     1,   111,     5,   639,
       5,   115,   652,     5,     5,    12,    13,    12,   648,   684,
      12,    13,    13,   111,     5,   116,     5,   115,   111,   112,
     113,    12,    13,   116,    13,   118,   119,   120,   120,   680,
     111,   113,   683,   111,   115,   675,   687,   115,   678,   679,
     111,   111,   111,   362,     1,   115,   115,   366,     5,     1,
     690,   111,     5,     0,     1,    13,   113,   697,     5,    16,
      17,    18,    19,    20,    18,    12,    13,    18,    13,    16,
     111,    56,    83,    20,   113,   715,     5,    83,     7,     8,
       9,    10,    11,   111,   111,    92,   726,    92,   111,   111,
     111,   731,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,   111,    56,
       5,     5,   687,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,   113,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,   114,   114,    96,
     305,    98,    99,   189,   111,   112,   113,   104,   359,   116,
     601,   118,   119,   120,   473,   112,   113,   468,   115,   116,
     511,   118,   684,   415,   121,   645,   123,   124,   125,     0,
       1,     0,     1,   203,     5,   114,    16,    17,    18,    19,
      20,    12,    13,   122,   572,    16,     1,   258,   167,    20,
       5,    28,   474,   649,    31,    32,    33,    34,    35,    36,
     435,    16,    17,    18,    19,    20,   692,    87,   571,    87,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,   279,   623,   234,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,   189,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,   460,   460,    96,     1,    98,    99,     0,
       5,   189,   109,   104,    -1,    -1,    -1,    12,    13,    -1,
      -1,    16,   113,   112,    -1,    20,   115,   118,    -1,    -1,
     121,    -1,    -1,   124,   125,    -1,   111,   112,   113,    -1,
      -1,   116,    -1,   118,   119,   120,    -1,    -1,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    -1,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    -1,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,     5,
      -1,    96,    -1,    98,    99,    -1,    12,    13,    -1,   104,
      16,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,     0,
       1,   112,    -1,   118,    -1,    -1,   121,     0,     1,   124,
     125,    -1,    13,    -1,    -1,    16,    17,    18,    19,    20,
      21,    22,    -1,    24,    25,    26,    -1,    20,    29,    -1,
      -1,    -1,    -1,    -1,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    -1,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,     5,    -1,
      96,     1,    98,    99,     1,     5,    13,    -1,   104,    16,
      17,    18,    19,    20,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,   118,    20,    -1,   121,    -1,    -1,   124,   125,
      93,    94,    95,    96,    97,    98,    99,   108,   101,   102,
     103,   112,    -1,    -1,    -1,    -1,    -1,   110,   111,    -1,
      -1,   114,    -1,    -1,   117,    -1,    -1,    -1,    -1,    -1,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    -1,
      -1,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,     0,     1,    -1,    -1,    93,    94,    95,    96,
      97,    98,    99,    -1,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    20,   110,   111,    -1,    -1,   117,   118,    -1,
     117,   121,    -1,    -1,    -1,    -1,    -1,     0,     1,    -1,
      -1,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    20,    13,    -1,
      -1,    16,    17,    18,    19,    20,    21,    22,    -1,    -1,
      -1,    -1,     0,     1,    -1,    -1,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    20,    -1,    -1,    93,    94,    95,    96,    97,
      98,    99,    -1,   101,   102,   103,    -1,    -1,    -1,    -1,
      -1,    -1,   110,   111,   112,    -1,    -1,   115,    13,   117,
      -1,    16,    17,    18,    19,    20,    21,    -1,    -1,    -1,
      93,    94,    95,    96,    97,    98,    99,     5,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,   110,   111,   112,
      -1,    -1,   115,    13,   117,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    93,    94,    95,    96,    97,
      98,    99,    -1,   101,   102,   103,    -1,    -1,    -1,    -1,
      -1,    -1,   110,   111,    -1,   113,    -1,    -1,    -1,   117,
      -1,    -1,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    -1,    -1,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    12,    13,    -1,    -1,    16,    -1,
      -1,    -1,    20,    93,    94,    95,     5,    97,    -1,    -1,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,   117,
     118,    -1,    -1,   121,    23,    24,    25,    26,    -1,    -1,
      29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,
      -1,    -1,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    -1,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    -1,    -1,    96,    -1,
      98,    99,     0,     1,    -1,    -1,   104,     5,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,
     118,     0,     1,   121,    -1,    -1,   124,   125,    -1,   108,
      -1,    -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    -1,    56,    -1,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    -1,    56,     0,     1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,    -1,    -1,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,   112,    56,    -1,     0,     1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,
      -1,    -1,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    -1,    -1,     0,     1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,
     112,    55,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    -1,    29,    30,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   108,    -1,    -1,    -1,   112,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    -1,    -1,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    -1,    -1,    -1,   108,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,   118,    -1,    -1,
     121,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      -1,    -1,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    -1,    -1,    -1,    -1,    96,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    -1,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    -1,
      -1,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,    28,    31,    32,    33,    34,    35,    36,   109,   127,
     128,   129,   130,   131,   132,   133,   134,   136,   158,   111,
     111,   111,   111,     5,     5,   111,     5,     0,   136,   136,
     136,   136,   136,   136,   136,     5,    27,   138,   142,   136,
     136,   136,   136,   205,   136,   136,   117,   159,   160,   162,
     136,   136,     1,   136,   137,     1,    13,   118,   121,   188,
     189,   190,   203,   204,    30,   145,   152,   153,   154,   171,
     173,   174,   176,   184,   185,   191,   192,   198,   199,   200,
     201,   206,   209,     1,    13,    56,   233,   234,   235,   236,
     238,    12,    13,    16,    20,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    96,    98,
      99,   104,   124,   125,   204,   241,   242,   243,   246,   247,
     248,   256,   257,   258,    57,    58,   163,   164,   166,   167,
     136,   136,   161,   247,   256,   136,    13,    16,    17,    18,
      19,    20,    21,    22,   211,   217,   218,   219,   220,   222,
     223,   224,   225,   226,   232,   106,   117,   178,   179,   180,
     181,   182,   183,   111,   115,   262,    12,     5,     6,   144,
     136,   136,    81,     1,   136,   136,    23,    25,   108,    29,
      24,    26,    37,   211,   266,   136,   136,   112,   235,   236,
     115,   115,     1,    18,   237,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,    79,   247,   136,   112,
       1,   116,   123,   244,   245,   136,   136,   136,   136,   136,
     136,   136,   112,    13,   112,   116,    22,    13,    18,    93,
      94,    95,    97,   100,   101,   102,   103,    13,   136,    22,
       1,   112,   208,   220,     1,     5,   118,   119,   120,   202,
      13,    21,   222,   223,   223,     1,   224,   136,     1,   136,
     112,   105,   107,   105,   107,   267,   136,     1,    27,    55,
     143,   145,   146,   260,   261,   112,   111,   116,   115,   262,
      13,   155,   112,     1,   136,   151,   136,   172,     1,   136,
     175,   136,     1,   136,     1,   136,   172,   136,   208,   267,
     237,   115,   115,   136,   136,   266,   136,     0,    13,   114,
     141,   240,   244,     0,   113,   140,   242,   244,   117,   204,
     244,   247,   249,   252,   253,   254,   136,   136,   266,   136,
     136,   246,   244,   253,   255,    59,    13,   162,   136,   136,
      13,   110,   136,   136,   136,   136,   136,   136,   136,   136,
      13,    21,   219,   116,   136,   136,   120,   136,   220,   223,
     182,   113,   178,   238,   136,   136,   136,   136,     0,     1,
      20,    93,    94,    95,    96,    97,    98,    99,   101,   102,
     103,   110,   112,   117,   139,   262,   263,   264,   265,   115,
     262,   136,     1,   137,   136,   136,   136,    12,    92,   156,
     115,   262,   156,   167,   169,   111,   115,   115,   262,   178,
      12,    13,   186,   115,   262,    13,    18,   193,   230,   115,
     262,   111,   211,   207,   136,   136,   267,   237,   242,   136,
      13,   141,   141,   140,   140,   136,   247,   140,     5,   135,
     250,   251,   244,   267,   140,   251,   116,   140,   136,   136,
     136,    18,   157,   160,   136,    13,    14,   203,   220,   221,
     212,   214,   214,    13,    14,   203,    12,    13,   215,   216,
      13,    13,   136,    13,   136,   136,   113,    18,   182,   182,
     182,   182,   267,   267,    12,   115,   115,   262,   233,   190,
     136,   136,     1,   116,   207,   170,   177,   172,   230,    13,
     172,   170,   207,   111,    56,   239,   136,   253,     5,   118,
     121,   249,   252,    21,   123,   140,   136,   163,   166,    59,
     165,   136,   113,   161,   221,   136,   136,    83,     1,   136,
     211,   136,   136,     5,   113,   135,   231,   231,    83,     1,
     136,   136,   136,     7,     8,     9,    10,    11,   114,   122,
     228,   136,   210,   136,   136,   141,   140,   136,   139,   115,
     262,   168,   169,   136,   111,   136,   111,   111,   111,   205,
     111,   170,   136,   237,   251,   135,   135,   136,   136,   253,
     136,   136,   242,   136,   113,   113,   136,   113,   116,   213,
     113,   113,    15,   113,   231,   113,   116,   113,   113,   136,
     114,   228,   218,   242,   115,     0,   115,   262,   167,   170,
     147,   170,   170,   170,   233,   170,   205,   140,   251,   164,
     136,   113,   113,   136,   135,   136,    12,    13,   229,   136,
     239,   136,     1,   139,   149,   150,   153,   154,   173,   174,
     185,   192,   199,   201,   209,   259,   260,   261,   136,   136,
     205,   139,   136,   233,   220,   214,   215,   136,   229,   113,
     147,   139,   137,   147,   187,   194,   233,   148,   139,   136,
      13,   227,   136,   136,   259,   259,   139,   188,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,   139,   195,   197,     1,   149,   259,
     113,   136,   114,   227,   136,   136,   196,   137,   114,   233,
     136,   111,   136,   233,   139
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   126,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   135,
     136,   136,   137,   137,   137,   138,   138,   139,   139,   140,
     140,   141,   141,   142,   142,   142,   143,   143,   144,   144,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   146,
     146,   146,   146,   147,   147,   148,   148,   149,   149,   149,
     149,   149,   149,   150,   150,   150,   150,   150,   150,   151,
     152,   153,   153,   153,   153,   153,   154,   154,   154,   154,
     155,   155,   156,   156,   157,   157,   158,   159,   159,   160,
     160,   161,   161,   162,   163,   164,   164,   165,   165,   166,
     166,   166,   167,   167,   168,   168,   169,   169,   169,   170,
     171,   172,   173,   173,   173,   174,   174,   175,   175,   176,
     177,   178,   178,   178,   178,   179,   180,   180,   181,   181,
     182,   182,   182,   183,   184,   185,   186,   186,   187,   187,
     188,   189,   189,   190,   190,   190,   191,   192,   192,   192,
     193,   193,   193,   193,   194,   194,   196,   195,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   198,   199,   199,   199,   200,   201,
     202,   202,   202,   202,   203,   203,   204,   204,   205,   206,
     207,   208,   209,   210,   211,   211,   211,   212,   213,   214,
     215,   215,   216,   216,   216,   217,   218,   218,   218,   218,
     218,   219,   219,   219,   220,   220,   220,   220,   220,   220,
     221,   221,   221,   222,   222,   223,   223,   223,   224,   224,
     224,   224,   224,   225,   226,   226,   226,   226,   227,   227,
     228,   228,   228,   228,   228,   228,   229,   229,   230,   231,
     231,   231,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   233,   233,   233,
     233,   233,   233,   234,   234,   234,   234,   235,   236,   236,
     236,   236,   236,   237,   238,   239,   239,   240,   240,   241,
     241,   241,   242,   242,   243,   243,   244,   245,   245,   245,
     246,   246,   246,   246,   246,   246,   246,   246,   246,   246,
     246,   246,   246,   246,   246,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   248,   248,   248,   249,   249,
     250,   250,   250,   250,   251,   251,   252,   253,   253,   254,
     254,   254,   254,   255,   255,   256,   256,   257,   257,   258,
     258,   259,   259,   260,   260,   261,   262,   263,   264,   265,
     265,   265,   265,   265,   265,   265,   265,   265,   265,   265,
     265,   266,   267,   267,   267,   267,   267
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     2,     2,     2,     2,     2,     2,     2,
       2,     6,     6,     5,     5,     5,     5,     5,     1,     2,
       0,     2,     0,     2,     2,     0,     1,     1,     1,     1,
       1,     1,     1,     5,     3,     3,     5,     3,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     3,     0,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     7,     7,     7,     4,     4,     6,     6,     3,     3,
       0,     2,     1,     1,     0,     4,     5,     2,     6,     0,
       2,     1,     1,     6,     4,     1,     5,     0,     3,     0,
       1,     1,     1,     5,     0,     1,     1,     4,     2,     0,
       0,     1,    10,     8,     4,    10,     3,     2,     2,     0,
       0,     1,     1,     1,     1,     3,     4,     4,     4,     4,
       5,     1,     3,     9,     0,    10,     1,     1,     0,     3,
       6,     1,     5,     2,     1,     1,     0,    10,     4,     4,
       1,     2,     1,     0,     1,     4,     0,     7,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     8,     4,     4,     0,    10,
       2,     2,     2,     3,     1,     0,     1,     1,     1,     0,
       0,     0,     9,     0,     1,     6,     2,     0,     0,     3,
       1,     1,     1,     5,     2,     2,     1,     1,     2,     3,
       2,     1,     2,     2,     1,     2,     1,     2,     3,     2,
       1,     5,     2,     1,     1,     1,     2,     2,     1,     1,
       1,     1,     1,     2,     5,    10,     6,    11,     2,     0,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     6,     2,     3,     7,     6,     6,     6,     6,     6,
       5,     6,     5,     6,     7,     6,     6,     0,     1,     2,
       1,     1,     2,     3,     3,     4,     4,     3,     5,     2,
       6,     3,     4,     3,     2,     2,     0,     2,     3,     3,
       4,     4,     1,     2,     1,     3,     3,     2,     2,     0,
       2,     3,     2,     2,     2,     3,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     3,     4,     1,     2,
       3,     3,     3,     3,     0,     1,     5,     1,     2,     1,
       3,     3,     1,     2,     5,     5,     4,     1,     1,     4,
       4,     1,     2,     3,     3,     2,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     0,     2,     2,     2,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, parser); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, CSSParser* parser)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (parser);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, CSSParser* parser)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, CSSParser* parser)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , parser);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, parser); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
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
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
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

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, CSSParser* parser)
{
  YYUSE (yyvaluep);
  YYUSE (parser);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 145: /* valid_rule  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2022 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 146: /* rule  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2028 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 147: /* block_rule_list  */
#line 209 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).ruleList); }
#line 2034 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 148: /* block_valid_rule_list  */
#line 209 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).ruleList); }
#line 2040 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 149: /* block_valid_rule  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2046 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 150: /* block_rule  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2052 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 153: /* import  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2058 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 157: /* maybe_media_value  */
#line 262 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).valueList); }
#line 2064 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 159: /* source_size_list  */
#line 225 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).sourceSizeList); }
#line 2070 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 160: /* maybe_source_media_query_exp  */
#line 227 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).mediaQueryExp); }
#line 2076 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 161: /* source_size_length  */
#line 229 "./CSSGrammar.y" /* yacc.c:1257  */
      { destroy(((*yyvaluep).value)); }
#line 2082 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 162: /* base_media_query_exp  */
#line 222 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).mediaQueryExp); }
#line 2088 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 163: /* media_query_exp  */
#line 222 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).mediaQueryExp); }
#line 2094 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 164: /* media_query_exp_list  */
#line 232 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).mediaQueryExpList); }
#line 2100 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 165: /* maybe_and_media_query_exp_list  */
#line 232 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).mediaQueryExpList); }
#line 2106 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 167: /* media_query  */
#line 217 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).mediaQuery); }
#line 2112 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 168: /* maybe_media_list  */
#line 214 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).mediaList)) ((*yyvaluep).mediaList)->deref(); }
#line 2118 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 169: /* media_list  */
#line 214 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).mediaList)) ((*yyvaluep).mediaList)->deref(); }
#line 2124 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 173: /* media  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2130 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 174: /* supports  */
#line 281 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2136 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 185: /* keyframes  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2142 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 187: /* keyframes_rule  */
#line 239 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).keyframeRuleList); }
#line 2148 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 188: /* keyframe_rule  */
#line 236 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).keyframe)) ((*yyvaluep).keyframe)->deref(); }
#line 2154 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 189: /* key_list  */
#line 262 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).valueList); }
#line 2160 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 192: /* page  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2166 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 193: /* page_selector  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2172 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 199: /* font_face  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2178 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 201: /* region  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2184 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 209: /* ruleset  */
#line 206 "./CSSGrammar.y" /* yacc.c:1257  */
      { if (((*yyvaluep).rule)) ((*yyvaluep).rule)->deref(); }
#line 2190 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 211: /* selector_list  */
#line 250 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selectorList); }
#line 2196 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 214: /* nested_selector_list  */
#line 250 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selectorList); }
#line 2202 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 216: /* comma_separated_lang_ranges  */
#line 266 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).stringList); }
#line 2208 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 217: /* complex_selector_with_trailing_whitespace  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2214 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 218: /* complex_selector  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2220 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 220: /* compound_selector  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2226 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 221: /* simple_selector_list  */
#line 250 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selectorList); }
#line 2232 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 223: /* specifier_list  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2238 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 224: /* specifier  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2244 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 225: /* class  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2250 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 226: /* attrib  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2256 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 230: /* pseudo_page  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2262 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 231: /* nth_selector_ending  */
#line 251 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selectorList); }
#line 2268 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 232: /* pseudo  */
#line 247 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).selector); }
#line 2274 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 240: /* ident_list  */
#line 272 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).valueList); }
#line 2280 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 241: /* track_names_list  */
#line 274 "./CSSGrammar.y" /* yacc.c:1257  */
      { destroy(((*yyvaluep).value)); }
#line 2286 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 242: /* expr  */
#line 262 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).valueList); }
#line 2292 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 243: /* valid_expr  */
#line 262 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).valueList); }
#line 2298 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 246: /* term  */
#line 242 "./CSSGrammar.y" /* yacc.c:1257  */
      { destroy(((*yyvaluep).value)); }
#line 2304 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 248: /* function  */
#line 242 "./CSSGrammar.y" /* yacc.c:1257  */
      { destroy(((*yyvaluep).value)); }
#line 2310 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 252: /* calc_func_paren_expr  */
#line 262 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).valueList); }
#line 2316 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 253: /* calc_func_expr  */
#line 262 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).valueList); }
#line 2322 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 254: /* valid_calc_func_expr  */
#line 262 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).valueList); }
#line 2328 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 255: /* calc_func_expr_list  */
#line 262 "./CSSGrammar.y" /* yacc.c:1257  */
      { delete ((*yyvaluep).valueList); }
#line 2334 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 256: /* calc_function  */
#line 242 "./CSSGrammar.y" /* yacc.c:1257  */
      { destroy(((*yyvaluep).value)); }
#line 2340 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;

    case 258: /* min_or_max_function  */
#line 242 "./CSSGrammar.y" /* yacc.c:1257  */
      { destroy(((*yyvaluep).value)); }
#line 2346 "./CSSGrammar.cpp" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (CSSParser* parser)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

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
      yychar = yylex (&yylval, parser);
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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 11:
#line 296 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->m_rule = adoptRef((yyvsp[-2].rule)); }
#line 2614 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 297 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->m_keyframe = adoptRef((yyvsp[-2].keyframe)); }
#line 2620 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 300 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        if ((yyvsp[-1].valueList)) {
            parser->m_valueList = std::unique_ptr<CSSParserValueList>((yyvsp[-1].valueList));
            int oldParsedProperties = parser->m_parsedProperties.size();
            if (!parser->parseValue(parser->m_id, parser->m_important))
                parser->rollbackLastProperties(parser->m_parsedProperties.size() - oldParsedProperties);
            parser->m_valueList = nullptr;
        }
    }
#line 2634 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 310 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->m_mediaQuery = std::unique_ptr<MediaQuery>((yyvsp[-1].mediaQuery)); }
#line 2640 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 312 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        if ((yyvsp[-1].selectorList)) {
            if (parser->m_selectorListForParseSelector)
                parser->m_selectorListForParseSelector->adoptSelectorVector(*(yyvsp[-1].selectorList));
            parser->recycleSelectorVector(std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>>((yyvsp[-1].selectorList)));
        }
    }
#line 2652 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 320 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->m_supportsCondition = (yyvsp[-1].boolean); }
#line 2658 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 329 "./CSSGrammar.y" /* yacc.c:1646  */
    {
     if (parser->m_styleSheet)
         parser->m_styleSheet->parserSetEncodingFromCharsetRule((yyvsp[-2].string));
     if (parser->isExtractingSourceData() && parser->m_currentRuleDataStack->isEmpty() && parser->m_ruleSourceDataResult)
         parser->addNewRuleToSourceTree(CSSRuleSourceData::createUnknown());
  }
#line 2669 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 340 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        if (RefPtr<StyleRuleBase> rule = adoptRef((yyvsp[-1].rule))) {
            if (parser->m_styleSheet)
                parser->m_styleSheet->parserAppendRule(rule.releaseNonNull());
        }
    }
#line 2680 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 353 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.rule) = nullptr; }
#line 2686 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 359 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = (yyvsp[0].rule);
        parser->m_hadSyntacticallyValidCSSRule = true;
    }
#line 2695 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 363 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.rule) = nullptr; }
#line 2701 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 364 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.rule) = nullptr; }
#line 2707 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 365 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.rule) = nullptr; }
#line 2713 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 368 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.ruleList) = nullptr; }
#line 2719 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 369 "./CSSGrammar.y" /* yacc.c:1646  */
    {
      (yyval.ruleList) = (yyvsp[-2].ruleList);
      if (RefPtr<StyleRuleBase> rule = adoptRef((yyvsp[-1].rule))) {
          if (!(yyval.ruleList))
              (yyval.ruleList) = new Vector<RefPtr<StyleRuleBase>>;
          (yyval.ruleList)->append(rule.release());
      }
  }
#line 2732 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 379 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.ruleList) = nullptr; }
#line 2738 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 380 "./CSSGrammar.y" /* yacc.c:1646  */
    {
      (yyval.ruleList) = (yyvsp[-2].ruleList);
      if (RefPtr<StyleRuleBase> rule = adoptRef((yyvsp[-1].rule))) {
          if (!(yyval.ruleList))
              (yyval.ruleList) = new Vector<RefPtr<StyleRuleBase>>;
          (yyval.ruleList)->append(rule.release());
      }
  }
#line 2751 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 397 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.rule) = nullptr; }
#line 2757 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 397 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.rule) = nullptr; }
#line 2763 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 397 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.rule) = nullptr; }
#line 2769 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 399 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderEnd();
        parser->markRuleBodyStart();
    }
#line 2778 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 405 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::IMPORT_RULE);
    }
#line 2786 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 410 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = parser->createImportRule((yyvsp[-3].string), adoptRef((yyvsp[-1].mediaList))).leakRef();
    }
#line 2794 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 413 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = parser->createImportRule((yyvsp[-3].string), adoptRef((yyvsp[-1].mediaList))).leakRef();
    }
#line 2802 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 416 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
        if ((yyvsp[-1].mediaList))
            (yyvsp[-1].mediaList)->deref();
    }
#line 2813 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 422 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
#line 2822 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 426 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
#line 2831 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 432 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->addNamespace((yyvsp[-3].string), (yyvsp[-2].string)); }
#line 2837 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 437 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.string).clear(); }
#line 2843 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 439 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.valueList) = nullptr; }
#line 2849 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 439 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.valueList) = (yyvsp[-1].valueList); }
#line 2855 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 441 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->m_sourceSizeList = std::unique_ptr<Vector<CSSParser::SourceSize>>((yyvsp[-2].sourceSizeList));
    }
#line 2863 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 446 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.sourceSizeList) = new Vector<CSSParser::SourceSize>;
        (yyval.sourceSizeList)->append(parser->sourceSize(std::unique_ptr<MediaQueryExp>((yyvsp[-1].mediaQueryExp)), (yyvsp[0].value)));
    }
#line 2872 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 450 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.sourceSizeList) = (yyvsp[-5].sourceSizeList);
        (yyval.sourceSizeList)->append(parser->sourceSize(std::unique_ptr<MediaQueryExp>((yyvsp[-1].mediaQueryExp)), (yyvsp[0].value)));
    }
#line 2881 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 456 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaQueryExp) = new MediaQueryExp;
    }
#line 2889 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 461 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<CSSParserValueList> mediaValue((yyvsp[-1].valueList));
        (yyvsp[-3].string).lower();
        (yyval.mediaQueryExp) = new MediaQueryExp((yyvsp[-3].string), mediaValue.get());
    }
#line 2899 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 468 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        if ((yyvsp[-3].mediaQueryRestrictor) != MediaQuery::None) {
            delete (yyvsp[-1].mediaQueryExp);
            (yyval.mediaQueryExp) = new MediaQueryExp;
        } else
            (yyval.mediaQueryExp) = (yyvsp[-1].mediaQueryExp);
    }
#line 2911 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 477 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaQueryExpList) = new Vector<std::unique_ptr<MediaQueryExp>>;
        (yyval.mediaQueryExpList)->append(std::unique_ptr<MediaQueryExp>((yyvsp[0].mediaQueryExp)));
    }
#line 2920 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 481 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaQueryExpList) = (yyvsp[-4].mediaQueryExpList);
        (yyval.mediaQueryExpList)->append(std::unique_ptr<MediaQueryExp>((yyvsp[0].mediaQueryExp)));
    }
#line 2929 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 487 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaQueryExpList) = new Vector<std::unique_ptr<MediaQueryExp>>;
    }
#line 2937 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 490 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaQueryExpList) = (yyvsp[0].mediaQueryExpList);
    }
#line 2945 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 495 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    }
#line 2953 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 498 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    }
#line 2961 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 501 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    }
#line 2969 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 506 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaQuery) = new MediaQuery(MediaQuery::None, "all", std::unique_ptr<Vector<std::unique_ptr<MediaQueryExp>>>((yyvsp[0].mediaQueryExpList)));
    }
#line 2977 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 510 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyvsp[-2].string).lower();
        (yyval.mediaQuery) = new MediaQuery((yyvsp[-4].mediaQueryRestrictor), (yyvsp[-2].string), std::unique_ptr<Vector<std::unique_ptr<MediaQueryExp>>>((yyvsp[0].mediaQueryExpList)));
    }
#line 2986 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 515 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.mediaList) = &MediaQuerySet::create().leakRef(); }
#line 2992 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 517 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaList) = &MediaQuerySet::create().leakRef();
        (yyval.mediaList)->addMediaQuery(std::unique_ptr<MediaQuery>((yyvsp[0].mediaQuery)));
        parser->updateLastMediaLine((yyval.mediaList));
    }
#line 3002 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 522 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaList) = (yyvsp[-3].mediaList);
        std::unique_ptr<MediaQuery> mediaQuery((yyvsp[0].mediaQuery));
        if ((yyval.mediaList)) {
            (yyval.mediaList)->addMediaQuery(WTF::move(mediaQuery));
            parser->updateLastMediaLine((yyval.mediaList));
        }
    }
#line 3015 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 530 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.mediaList) = nullptr;
        if ((yyvsp[-1].mediaList))
            (yyvsp[-1].mediaList)->deref();
    }
#line 3025 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 537 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleBodyStart();
    }
#line 3033 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 542 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::MEDIA_RULE);
    }
#line 3041 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 111:
#line 547 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderEnd();
    }
#line 3049 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 552 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = parser->createMediaRule(adoptRef((yyvsp[-6].mediaList)), std::unique_ptr<Vector<RefPtr<StyleRuleBase>>>((yyvsp[-1].ruleList)).get()).leakRef();
    }
#line 3057 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 555 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = parser->createEmptyMediaRule(std::unique_ptr<Vector<RefPtr<StyleRuleBase>>>((yyvsp[-1].ruleList)).get()).leakRef();
    }
#line 3065 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 114:
#line 558 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
#line 3074 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 115:
#line 564 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = parser->createSupportsRule((yyvsp[-6].boolean), std::unique_ptr<Vector<RefPtr<StyleRuleBase>>>((yyvsp[-1].ruleList)).get()).leakRef();
    }
#line 3082 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 116:
#line 567 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
        parser->popSupportsRuleData();
    }
#line 3092 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 117:
#line 574 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.boolean) = false;
    }
#line 3100 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 118:
#line 577 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.boolean) = false;
    }
#line 3108 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 119:
#line 582 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::SUPPORTS_RULE);
        parser->markSupportsRuleHeaderStart();
    }
#line 3117 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 120:
#line 588 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderEnd();
        parser->markSupportsRuleHeaderEnd();
    }
#line 3126 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 125:
#line 594 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = !(yyvsp[0].boolean); }
#line 3132 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 126:
#line 596 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = (yyvsp[-3].boolean) && (yyvsp[0].boolean); }
#line 3138 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 127:
#line 597 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = (yyvsp[-3].boolean) && (yyvsp[0].boolean); }
#line 3144 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 128:
#line 600 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = (yyvsp[-3].boolean) || (yyvsp[0].boolean); }
#line 3150 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 129:
#line 601 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = (yyvsp[-3].boolean) || (yyvsp[0].boolean); }
#line 3156 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 130:
#line 604 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = (yyvsp[-2].boolean); }
#line 3162 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 131:
#line 605 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = (yyvsp[0].boolean); }
#line 3168 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 132:
#line 606 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = false; }
#line 3174 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 133:
#line 609 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.boolean) = false;
        CSSParser* p = static_cast<CSSParser*>(parser);
        std::unique_ptr<CSSParserValueList> propertyValue((yyvsp[-3].valueList));
        if ((yyvsp[-6].id) && propertyValue) {
            p->m_valueList = WTF::move(propertyValue);
            int oldParsedProperties = p->m_parsedProperties.size();
            (yyval.boolean) = p->parseValue((yyvsp[-6].id), (yyvsp[-2].boolean));
            if ((yyval.boolean))
                p->rollbackLastProperties(p->m_parsedProperties.size() - oldParsedProperties);
            p->m_valueList = nullptr;
        }
        p->markPropertyEnd((yyvsp[-2].boolean), false);
    }
#line 3193 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 134:
#line 625 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::KEYFRAMES_RULE);
    }
#line 3201 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 135:
#line 630 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = parser->createKeyframesRule((yyvsp[-6].string), std::unique_ptr<Vector<RefPtr<StyleKeyframe>>>((yyvsp[-1].keyframeRuleList))).leakRef();
    }
#line 3209 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 138:
#line 636 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.keyframeRuleList) = new Vector<RefPtr<StyleKeyframe>>; }
#line 3215 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 139:
#line 637 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.keyframeRuleList) = (yyvsp[-2].keyframeRuleList);
        if (RefPtr<StyleKeyframe> keyframe = adoptRef((yyvsp[-1].keyframe)))
            (yyval.keyframeRuleList)->append(keyframe.release());
    }
#line 3225 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 140:
#line 643 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.keyframe) = parser->createKeyframe(*std::unique_ptr<CSSParserValueList>((yyvsp[-5].valueList))).leakRef(); }
#line 3231 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 141:
#line 645 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.valueList) = new CSSParserValueList;
        (yyval.valueList)->addValue((yyvsp[0].value));
    }
#line 3240 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 142:
#line 649 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.valueList) = (yyvsp[-4].valueList);
        ASSERT((yyvsp[0].value).unit != CSSParserValue::Function);
        if ((yyval.valueList))
            (yyval.valueList)->addValue((yyvsp[0].value));
    }
#line 3251 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 143:
#line 657 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[-1].integer) * (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; }
#line 3257 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 144:
#line 658 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER;
        CSSParserString& str = (yyvsp[0].string);
        if (str.equalIgnoringCase("from"))
            (yyval.value).fValue = 0;
        else if (str.equalIgnoringCase("to"))
            (yyval.value).fValue = 100;
        else {
            (yyval.value).unit = 0;
            YYERROR;
        }
    }
#line 3274 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 145:
#line 670 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.value).unit = 0;
    }
#line 3282 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 146:
#line 675 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::PAGE_RULE);
    }
#line 3290 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 147:
#line 681 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        if ((yyvsp[-6].selector))
            (yyval.rule) = parser->createPageRule(std::unique_ptr<CSSParserSelector>((yyvsp[-6].selector))).leakRef();
        else {
            parser->clearProperties();
            (yyval.rule) = nullptr;
            parser->popRuleData();
        }
    }
#line 3304 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 148:
#line 690 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->popRuleData();
        (yyval.rule) = nullptr;
    }
#line 3313 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 149:
#line 694 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->popRuleData();
        (yyval.rule) = nullptr;
    }
#line 3322 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 150:
#line 700 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector(QualifiedName(nullAtom, (yyvsp[0].string), parser->m_defaultNamespace));
        (yyval.selector)->setForPage();
    }
#line 3331 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 151:
#line 704 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = (yyvsp[0].selector);
        if ((yyval.selector)) {
            (yyval.selector)->prependTagSelector(QualifiedName(nullAtom, (yyvsp[-1].string), parser->m_defaultNamespace));
            (yyval.selector)->setForPage();
        }
    }
#line 3343 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 152:
#line 711 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = (yyvsp[0].selector);
        if ((yyval.selector))
            (yyval.selector)->setForPage();
    }
#line 3353 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 153:
#line 716 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setForPage();
    }
#line 3362 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 156:
#line 723 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->startDeclarationsForMarginBox();
    }
#line 3370 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 157:
#line 725 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->createMarginAtRule((yyvsp[-6].marginBox));
    }
#line 3378 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 158:
#line 730 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::TopLeftCornerMarginBox;
    }
#line 3386 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 159:
#line 733 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::TopLeftMarginBox;
    }
#line 3394 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 160:
#line 736 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::TopCenterMarginBox;
    }
#line 3402 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 161:
#line 739 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::TopRightMarginBox;
    }
#line 3410 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 162:
#line 742 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::TopRightCornerMarginBox;
    }
#line 3418 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 163:
#line 745 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::BottomLeftCornerMarginBox;
    }
#line 3426 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 164:
#line 748 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::BottomLeftMarginBox;
    }
#line 3434 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 165:
#line 751 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::BottomCenterMarginBox;
    }
#line 3442 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 166:
#line 754 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::BottomRightMarginBox;
    }
#line 3450 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 167:
#line 757 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::BottomRightCornerMarginBox;
    }
#line 3458 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 168:
#line 760 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::LeftTopMarginBox;
    }
#line 3466 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 169:
#line 763 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::LeftMiddleMarginBox;
    }
#line 3474 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 170:
#line 766 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::LeftBottomMarginBox;
    }
#line 3482 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 171:
#line 769 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::RightTopMarginBox;
    }
#line 3490 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 172:
#line 772 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::RightMiddleMarginBox;
    }
#line 3498 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 173:
#line 775 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.marginBox) = CSSSelector::RightBottomMarginBox;
    }
#line 3506 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 174:
#line 780 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::FONT_FACE_RULE);
    }
#line 3514 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 175:
#line 785 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = parser->createFontFaceRule().leakRef();
    }
#line 3522 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 176:
#line 788 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
#line 3531 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 177:
#line 792 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = nullptr;
        parser->popRuleData();
    }
#line 3540 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 178:
#line 798 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::REGION_RULE);
    }
#line 3548 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 179:
#line 803 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<Vector<RefPtr<StyleRuleBase>>> ruleList((yyvsp[-1].ruleList));
        if ((yyvsp[-6].selectorList))
            (yyval.rule) = parser->createRegionRule(std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>>((yyvsp[-6].selectorList)).get(), ruleList.get()).leakRef();
        else {
            (yyval.rule) = nullptr;
            parser->popRuleData();
        }
    }
#line 3562 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 180:
#line 814 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.relation) = CSSParserSelectorCombinator::DirectAdjacent; }
#line 3568 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 181:
#line 815 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.relation) = CSSParserSelectorCombinator::IndirectAdjacent; }
#line 3574 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 182:
#line 816 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.relation) = CSSParserSelectorCombinator::Child; }
#line 3580 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 183:
#line 817 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.relation) = CSSParserSelectorCombinator::DescendantDoubleChild; }
#line 3586 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 185:
#line 819 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.integer) = 1; }
#line 3592 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 186:
#line 820 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.integer) = -1; }
#line 3598 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 187:
#line 820 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.integer) = 1; }
#line 3604 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 188:
#line 821 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->markPropertyStart(); }
#line 3610 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 189:
#line 823 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::STYLE_RULE);
        parser->markSelectorStart();
    }
#line 3619 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 190:
#line 828 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->markRuleHeaderEnd(); }
#line 3625 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 191:
#line 829 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->markSelectorEnd(); }
#line 3631 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 192:
#line 831 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.rule) = parser->createStyleRule((yyvsp[-7].selectorList)).leakRef();
        parser->recycleSelectorVector(std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>>((yyvsp[-7].selectorList)));
    }
#line 3640 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 193:
#line 836 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->markSelectorStart(); }
#line 3646 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 194:
#line 838 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selectorList) = nullptr;
        if ((yyvsp[0].selector)) {
            (yyval.selectorList) = parser->createSelectorVector().release();
            (yyval.selectorList)->append(std::unique_ptr<CSSParserSelector>((yyvsp[0].selector)));
            parser->updateLastSelectorLineAndPosition();
        }
    }
#line 3659 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 195:
#line 846 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>> selectorList((yyvsp[-5].selectorList));
        std::unique_ptr<CSSParserSelector> selector((yyvsp[0].selector));
        (yyval.selectorList) = nullptr;
        if (selectorList && selector) {
            (yyval.selectorList) = selectorList.release();
            (yyval.selectorList)->append(WTF::move(selector));
            parser->updateLastSelectorLineAndPosition();
        }
    }
#line 3674 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 196:
#line 856 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selectorList) = nullptr;
        delete (yyvsp[-1].selectorList);
    }
#line 3683 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 197:
#line 861 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->startNestedSelectorList(); }
#line 3689 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 198:
#line 862 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->endNestedSelectorList(); }
#line 3695 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 199:
#line 864 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selectorList) = (yyvsp[-1].selectorList);
    }
#line 3703 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 202:
#line 870 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.stringList) = new Vector<CSSParserString>;
        (yyval.stringList)->append((yyvsp[0].string));
    }
#line 3712 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 203:
#line 874 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.stringList) = (yyvsp[-4].stringList);
        if ((yyval.stringList))
            (yyvsp[-4].stringList)->append((yyvsp[0].string));
    }
#line 3722 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 204:
#line 879 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.stringList) = nullptr;
        delete (yyvsp[-1].stringList);
    }
#line 3731 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 208:
#line 889 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<CSSParserSelector> left((yyvsp[-1].selector));
        std::unique_ptr<CSSParserSelector> right((yyvsp[0].selector));
        (yyval.selector) = nullptr;
        if (left && right) {
            right->appendTagHistory(CSSParserSelectorCombinator::DescendantSpace, WTF::move(left));
            (yyval.selector) = right.release();
        }
    }
#line 3745 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 209:
#line 898 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<CSSParserSelector> left((yyvsp[-2].selector));
        std::unique_ptr<CSSParserSelector> right((yyvsp[0].selector));
        (yyval.selector) = nullptr;
        if (left && right) {
            right->appendTagHistory((yyvsp[-1].relation), WTF::move(left));
            (yyval.selector) = right.release();
        }
    }
#line 3759 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 210:
#line 907 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        delete (yyvsp[-1].selector);
    }
#line 3768 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 211:
#line 913 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.string).clear(); }
#line 3774 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 212:
#line 914 "./CSSGrammar.y" /* yacc.c:1646  */
    { static LChar star = '*'; (yyval.string).init(&star, 1); }
#line 3780 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 214:
#line 918 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector(QualifiedName(nullAtom, (yyvsp[0].string), parser->m_defaultNamespace));
    }
#line 3788 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 215:
#line 921 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = (yyvsp[0].selector);
        if ((yyval.selector))
            parser->rewriteSpecifiersWithElementName(nullAtom, (yyvsp[-1].string), *(yyval.selector));
    }
#line 3798 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 216:
#line 926 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = (yyvsp[0].selector);
        if ((yyval.selector))
            parser->rewriteSpecifiersWithNamespaceIfNeeded(*(yyval.selector));
    }
#line 3808 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 217:
#line 931 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector(parser->determineNameInNamespace((yyvsp[-1].string), (yyvsp[0].string)));
    }
#line 3816 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 218:
#line 934 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = (yyvsp[0].selector);
        if ((yyval.selector))
            parser->rewriteSpecifiersWithElementName((yyvsp[-2].string), (yyvsp[-1].string), *(yyval.selector));
    }
#line 3826 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 219:
#line 939 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = (yyvsp[0].selector);
        if ((yyval.selector))
            parser->rewriteSpecifiersWithElementName((yyvsp[-1].string), starAtom, *(yyval.selector));
    }
#line 3836 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 220:
#line 946 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selectorList) = nullptr;
        if ((yyvsp[0].selector)) {
            (yyval.selectorList) = parser->createSelectorVector().release();
            (yyval.selectorList)->append(std::unique_ptr<CSSParserSelector>((yyvsp[0].selector)));
        }
    }
#line 3848 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 221:
#line 953 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>> list((yyvsp[-4].selectorList));
        std::unique_ptr<CSSParserSelector> selector((yyvsp[0].selector));
        (yyval.selectorList) = nullptr;
        if (list && selector) {
            (yyval.selectorList) = list.release();
            (yyval.selectorList)->append(WTF::move(selector));
        }
    }
#line 3862 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 222:
#line 962 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selectorList) = nullptr;
        delete (yyvsp[-1].selectorList);
    }
#line 3871 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 223:
#line 968 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.string) = (yyvsp[0].string);
    }
#line 3879 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 224:
#line 971 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        static LChar star = '*';
        (yyval.string).init(&star, 1);
    }
#line 3888 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 226:
#line 978 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<CSSParserSelector> list((yyvsp[-1].selector));
        std::unique_ptr<CSSParserSelector> specifier((yyvsp[0].selector));
        (yyval.selector) = nullptr;
        if (list && specifier)
            (yyval.selector) = parser->rewriteSpecifiers(WTF::move(list), WTF::move(specifier)).release();
    }
#line 3900 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 227:
#line 985 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        delete (yyvsp[-1].selector);
    }
#line 3909 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 228:
#line 991 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setMatch(CSSSelector::Id);
        if (parser->m_context.mode == CSSQuirksMode)
            (yyvsp[0].string).lower();
        (yyval.selector)->setValue((yyvsp[0].string));
    }
#line 3921 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 229:
#line 998 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        if ((yyvsp[0].string)[0] >= '0' && (yyvsp[0].string)[0] <= '9')
            (yyval.selector) = nullptr;
        else {
            (yyval.selector) = new CSSParserSelector;
            (yyval.selector)->setMatch(CSSSelector::Id);
            if (parser->m_context.mode == CSSQuirksMode)
                (yyvsp[0].string).lower();
            (yyval.selector)->setValue((yyvsp[0].string));
        }
    }
#line 3937 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 233:
#line 1014 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setMatch(CSSSelector::Class);
        if (parser->m_context.mode == CSSQuirksMode)
            (yyvsp[0].string).lower();
        (yyval.selector)->setValue((yyvsp[0].string));
    }
#line 3949 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 234:
#line 1023 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[-2].string), nullAtom), parser->m_context.isHTMLDocument);
        (yyval.selector)->setMatch(CSSSelector::Set);
    }
#line 3959 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 235:
#line 1028 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[-7].string), nullAtom), parser->m_context.isHTMLDocument);
        (yyval.selector)->setMatch((yyvsp[-5].match));
        (yyval.selector)->setValue((yyvsp[-3].string));
        (yyval.selector)->setAttributeValueMatchingIsCaseInsensitive((yyvsp[-1].boolean));
    }
#line 3971 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 236:
#line 1035 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setAttribute(parser->determineNameInNamespace((yyvsp[-3].string), (yyvsp[-2].string)), parser->m_context.isHTMLDocument);
        (yyval.selector)->setMatch(CSSSelector::Set);
    }
#line 3981 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 237:
#line 1040 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = new CSSParserSelector;
        (yyval.selector)->setAttribute(parser->determineNameInNamespace((yyvsp[-8].string), (yyvsp[-7].string)), parser->m_context.isHTMLDocument);
        (yyval.selector)->setMatch((yyvsp[-5].match));
        (yyval.selector)->setValue((yyvsp[-3].string));
        (yyval.selector)->setAttributeValueMatchingIsCaseInsensitive((yyvsp[-1].boolean));
    }
#line 3993 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 238:
#line 1049 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        if (UNLIKELY((yyvsp[-1].string).length() != 1 || !isASCIIAlphaCaselessEqual((yyvsp[-1].string)[0], 'i')))
            YYERROR;
        (yyval.boolean) = true;
    }
#line 4003 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 239:
#line 1055 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.boolean) = false;
    }
#line 4011 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 240:
#line 1059 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.match) = CSSSelector::Exact;
    }
#line 4019 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 241:
#line 1062 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.match) = CSSSelector::List;
    }
#line 4027 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 242:
#line 1065 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.match) = CSSSelector::Hyphen;
    }
#line 4035 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 243:
#line 1068 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.match) = CSSSelector::Begin;
    }
#line 4043 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 244:
#line 1071 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.match) = CSSSelector::End;
    }
#line 4051 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 245:
#line 1074 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.match) = CSSSelector::Contain;
    }
#line 4059 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 248:
#line 1080 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = CSSParserSelector::parsePagePseudoSelector((yyvsp[0].string));
    }
#line 4067 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 249:
#line 1084 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selectorList) = nullptr;
    }
#line 4075 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 250:
#line 1087 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selectorList) = nullptr;
    }
#line 4083 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 251:
#line 1090 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        if ((yyvsp[-2].selectorList))
            (yyval.selectorList) = (yyvsp[-2].selectorList);
        else
            YYERROR;
    }
#line 4094 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 252:
#line 1098 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = CSSParserSelector::parsePseudoClassAndCompatibilityElementSelector((yyvsp[0].string));
    }
#line 4102 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 253:
#line 1101 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = CSSParserSelector::parsePseudoElementSelector((yyvsp[0].string));
    }
#line 4110 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 254:
#line 1104 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = CSSParserSelector::parsePseudoElementCueFunctionSelector((yyvsp[-4].string), (yyvsp[-2].selectorList));
    }
#line 4118 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 255:
#line 1107 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        if ((yyvsp[-2].selectorList)) {
            auto selector = std::make_unique<CSSParserSelector>();
            selector->setMatch(CSSSelector::PseudoClass);
            selector->adoptSelectorVector(*std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>>((yyvsp[-2].selectorList)));
            selector->setPseudoClassValue((yyvsp[-4].string));
            if (selector->pseudoClassType() == CSSSelector::PseudoClassAny)
                (yyval.selector) = selector.release();
        }
    }
#line 4134 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 256:
#line 1118 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        if ((yyvsp[-2].selectorList)) {
            auto selector = std::make_unique<CSSParserSelector>();
            selector->setMatch(CSSSelector::PseudoClass);
            selector->adoptSelectorVector(*std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>>((yyvsp[-2].selectorList)));
            selector->setPseudoClassValue((yyvsp[-4].string));
            if (selector->pseudoClassType() == CSSSelector::PseudoClassMatches)
                (yyval.selector) = selector.release();
        }
    }
#line 4150 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 257:
#line 1129 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        if ((yyvsp[-2].stringList)) {
            auto selector = std::make_unique<CSSParserSelector>();
            selector->setMatch(CSSSelector::PseudoClass);
            selector->setLangArgumentList(*std::unique_ptr<Vector<CSSParserString>>((yyvsp[-2].stringList)));
            selector->setPseudoClassValue((yyvsp[-4].string));
            if (selector->pseudoClassType() == CSSSelector::PseudoClassLang)
                (yyval.selector) = selector.release();
        }
    }
#line 4166 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 258:
#line 1140 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument((yyvsp[-2].string));
        selector->setPseudoClassValue((yyvsp[-4].string));
        if (selector->pseudoClassType() == CSSSelector::PseudoClassDir)
            (yyval.selector) = selector.release();
    }
#line 4180 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 259:
#line 1149 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument((yyvsp[-2].string));
        selector->setPseudoClassValue((yyvsp[-4].string));
        if (selector->pseudoClassType() == CSSSelector::PseudoClassRole)
            (yyval.selector) = selector.release();
    }
#line 4194 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 260:
#line 1158 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>> ending((yyvsp[0].selectorList));
        if (selectorListDoesNotMatchAnyPseudoElement(ending.get())) {
            auto selector = std::make_unique<CSSParserSelector>();
            selector->setMatch(CSSSelector::PseudoClass);
            selector->setArgument((yyvsp[-1].string));
            selector->setPseudoClassValue((yyvsp[-3].string));
            if (ending)
                selector->adoptSelectorVector(*ending);
            CSSSelector::PseudoClassType pseudoClassType = selector->pseudoClassType();
            if (pseudoClassType == CSSSelector::PseudoClassNthChild || pseudoClassType == CSSSelector::PseudoClassNthLastChild)
                (yyval.selector) = selector.release();
        }
    }
#line 4214 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 261:
#line 1173 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>> ending((yyvsp[0].selectorList));
        if (selectorListDoesNotMatchAnyPseudoElement(ending.get())) {
            auto selector = std::make_unique<CSSParserSelector>();
            selector->setMatch(CSSSelector::PseudoClass);
            selector->setArgument(AtomicString::number((yyvsp[-2].integer) * (yyvsp[-1].number)));
            selector->setPseudoClassValue((yyvsp[-4].string));
            if (ending)
                selector->adoptSelectorVector(*ending);
            CSSSelector::PseudoClassType pseudoClassType = selector->pseudoClassType();
            if (pseudoClassType == CSSSelector::PseudoClassNthChild || pseudoClassType == CSSSelector::PseudoClassNthLastChild)
                (yyval.selector) = selector.release();
        }
    }
#line 4234 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 262:
#line 1188 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>> ending((yyvsp[0].selectorList));
        if (isValidNthToken((yyvsp[-1].string)) && selectorListDoesNotMatchAnyPseudoElement(ending.get())) {
            auto selector = std::make_unique<CSSParserSelector>();
            selector->setMatch(CSSSelector::PseudoClass);
            selector->setArgument((yyvsp[-1].string));
            selector->setPseudoClassValue((yyvsp[-3].string));
            if (ending)
               selector->adoptSelectorVector(*ending);
            CSSSelector::PseudoClassType pseudoClassType = selector->pseudoClassType();
            if (pseudoClassType == CSSSelector::PseudoClassNthChild || pseudoClassType == CSSSelector::PseudoClassNthLastChild)
                (yyval.selector) = selector.release();
        }
    }
#line 4254 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 263:
#line 1203 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument((yyvsp[-2].string));
        selector->setPseudoClassValue((yyvsp[-4].string));
        if (selector->pseudoClassType() != CSSSelector::PseudoClassUnknown)
            (yyval.selector) = selector.release();
    }
#line 4268 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 264:
#line 1212 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument(AtomicString::number((yyvsp[-3].integer) * (yyvsp[-2].number)));
        selector->setPseudoClassValue((yyvsp[-5].string));
        if (selector->pseudoClassType() != CSSSelector::PseudoClassUnknown)
            (yyval.selector) = selector.release();
    }
#line 4282 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 265:
#line 1221 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument((yyvsp[-2].string));
        selector->setPseudoClassValue((yyvsp[-4].string));
        CSSSelector::PseudoClassType type = selector->pseudoClassType();
        if (type == CSSSelector::PseudoClassUnknown)
            selector = nullptr;
        else if (type == CSSSelector::PseudoClassNthChild ||
                 type == CSSSelector::PseudoClassNthOfType ||
                 type == CSSSelector::PseudoClassNthLastChild ||
                 type == CSSSelector::PseudoClassNthLastOfType) {
            if (!isValidNthToken((yyvsp[-2].string)))
                selector = nullptr;
        }
        (yyval.selector) = selector.release();
    }
#line 4304 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 266:
#line 1238 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.selector) = nullptr;
        if ((yyvsp[-2].selectorList)) {
            std::unique_ptr<Vector<std::unique_ptr<CSSParserSelector>>> list((yyvsp[-2].selectorList));
            if (selectorListDoesNotMatchAnyPseudoElement(list.get())) {
                auto selector = std::make_unique<CSSParserSelector>();
                selector->setMatch(CSSSelector::PseudoClass);
                selector->setPseudoClassValue((yyvsp[-4].string));
                selector->adoptSelectorVector(*list);
                if (selector->pseudoClassType() == CSSSelector::PseudoClassNot)
                    (yyval.selector) = selector.release();
            }
        }
    }
#line 4323 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 267:
#line 1254 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = false; }
#line 4329 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 269:
#line 1256 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = (yyvsp[-1].boolean) || (yyvsp[0].boolean); }
#line 4335 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 271:
#line 1258 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = false; }
#line 4341 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 273:
#line 1262 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markPropertyStart();
        (yyval.boolean) = (yyvsp[-2].boolean);
    }
#line 4350 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 274:
#line 1266 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markPropertyStart();
        (yyval.boolean) = false;
    }
#line 4359 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 275:
#line 1270 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markPropertyStart();
        (yyval.boolean) = (yyvsp[-3].boolean);
        if ((yyvsp[-2].boolean))
            (yyval.boolean) = (yyvsp[-2].boolean);
    }
#line 4370 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 276:
#line 1276 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markPropertyStart();
        (yyval.boolean) = (yyvsp[-3].boolean);
    }
#line 4379 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 277:
#line 1282 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->syntaxError((yyvsp[-1].location), CSSParser::PropertyDeclarationError);
    }
#line 4387 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 278:
#line 1287 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.boolean) = false;
        bool isPropertyParsed = false;
        std::unique_ptr<CSSParserValueList> propertyValue((yyvsp[-1].valueList));
        if ((yyvsp[-4].id) && propertyValue) {
            parser->m_valueList = WTF::move(propertyValue);
            int oldParsedProperties = parser->m_parsedProperties.size();
            (yyval.boolean) = parser->parseValue((yyvsp[-4].id), (yyvsp[0].boolean));
            if (!(yyval.boolean))
                parser->rollbackLastProperties(parser->m_parsedProperties.size() - oldParsedProperties);
            else
                isPropertyParsed = true;
            parser->m_valueList = nullptr;
        }
        parser->markPropertyEnd((yyvsp[0].boolean), isPropertyParsed);
    }
#line 4408 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 279:
#line 1303 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = false; }
#line 4414 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 280:
#line 1304 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markPropertyEnd(false, false);
        delete (yyvsp[-2].valueList);
        (yyval.boolean) = false;
    }
#line 4424 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 281:
#line 1309 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.boolean) = false;
    }
#line 4432 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 282:
#line 1312 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        parser->markPropertyEnd(false, false);
        (yyval.boolean) = false;
    }
#line 4441 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 283:
#line 1317 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->syntaxError((yyvsp[-1].location)); }
#line 4447 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 284:
#line 1318 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.id) = cssPropertyID((yyvsp[-1].string)); }
#line 4453 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 285:
#line 1319 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = true; }
#line 4459 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 286:
#line 1319 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.boolean) = false; }
#line 4465 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 287:
#line 1321 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.valueList) = new CSSParserValueList;
        (yyval.valueList)->addValue(makeIdentValue((yyvsp[-1].string)));
    }
#line 4474 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 288:
#line 1325 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.valueList) = (yyvsp[-2].valueList);
        (yyval.valueList)->addValue(makeIdentValue((yyvsp[-1].string)));
    }
#line 4483 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 289:
#line 1331 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.value).setFromValueList(std::make_unique<CSSParserValueList>());
    }
#line 4491 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 290:
#line 1334 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.value).setFromValueList(std::unique_ptr<CSSParserValueList>((yyvsp[-1].valueList)));
    }
#line 4499 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 291:
#line 1337 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = 0;
        YYERROR;
    }
#line 4509 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 293:
#line 1343 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.valueList) = nullptr; delete (yyvsp[-1].valueList); }
#line 4515 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 294:
#line 1345 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.valueList) = new CSSParserValueList;
        (yyval.valueList)->addValue((yyvsp[0].value));
    }
#line 4524 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 295:
#line 1349 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.valueList) = (yyvsp[-2].valueList);
        if (!(yyval.valueList))
            destroy((yyvsp[0].value));
        else {
            if ((yyvsp[-1].character)) {
                CSSParserValue v;
                v.id = CSSValueInvalid;
                v.unit = CSSParserValue::Operator;
                v.iValue = (yyvsp[-1].character);
                (yyval.valueList)->addValue(v);
            }
            (yyval.valueList)->addValue((yyvsp[0].value));
        }
    }
#line 4544 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 297:
#line 1366 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.character) = '/'; }
#line 4550 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 298:
#line 1366 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.character) = ','; }
#line 4556 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 299:
#line 1366 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.character) = 0; }
#line 4562 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 301:
#line 1369 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value) = (yyvsp[-1].value); (yyval.value).fValue *= (yyvsp[-2].integer); }
#line 4568 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 302:
#line 1370 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[-1].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; }
#line 4574 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 303:
#line 1371 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value) = makeIdentValue((yyvsp[-1].string)); }
#line 4580 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 304:
#line 1372 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[-1].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; }
#line 4586 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 305:
#line 1373 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[-1].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; }
#line 4592 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 306:
#line 1374 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[-1].string); (yyval.value).unit = CSSPrimitiveValue::CSS_URI; }
#line 4598 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 307:
#line 1375 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[-1].string); (yyval.value).unit = CSSPrimitiveValue::CSS_UNICODE_RANGE; }
#line 4604 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 308:
#line 1376 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[-1].string); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
#line 4610 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 309:
#line 1377 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = CSSParserString(); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
#line 4616 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 313:
#line 1381 "./CSSGrammar.y" /* yacc.c:1646  */
    {
      (yyval.value).id = CSSValueInvalid; (yyval.value).unit = 0;
  }
#line 4624 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 315:
#line 1387 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = true; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; }
#line 4630 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 316:
#line 1388 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; }
#line 4636 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 317:
#line 1389 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PERCENTAGE; }
#line 4642 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 318:
#line 1390 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PX; }
#line 4648 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 319:
#line 1391 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CM; }
#line 4654 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 320:
#line 1392 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MM; }
#line 4660 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 321:
#line 1393 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_IN; }
#line 4666 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 322:
#line 1394 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PT; }
#line 4672 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 323:
#line 1395 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PC; }
#line 4678 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 324:
#line 1396 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DEG; }
#line 4684 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 325:
#line 1397 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_RAD; }
#line 4690 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 326:
#line 1398 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_GRAD; }
#line 4696 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 327:
#line 1399 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_TURN; }
#line 4702 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 328:
#line 1400 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MS; }
#line 4708 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 329:
#line 1401 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_S; }
#line 4714 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 330:
#line 1402 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_HZ; }
#line 4720 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 331:
#line 1403 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_KHZ; }
#line 4726 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 332:
#line 1404 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EMS; }
#line 4732 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 333:
#line 1405 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSParserValue::Q_EMS; }
#line 4738 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 334:
#line 1406 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EXS; }
#line 4744 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 335:
#line 1407 "./CSSGrammar.y" /* yacc.c:1646  */
    {
      (yyval.value).id = CSSValueInvalid;
      (yyval.value).fValue = (yyvsp[0].number);
      (yyval.value).unit = CSSPrimitiveValue::CSS_REMS;
      if (parser->m_styleSheet)
          parser->m_styleSheet->parserSetUsesRemUnits();
  }
#line 4756 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 336:
#line 1414 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CHS; }
#line 4762 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 337:
#line 1415 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VW; }
#line 4768 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 338:
#line 1416 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VH; }
#line 4774 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 339:
#line 1417 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VMIN; }
#line 4780 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 340:
#line 1418 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VMAX; }
#line 4786 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 341:
#line 1419 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DPPX; }
#line 4792 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 342:
#line 1420 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DPI; }
#line 4798 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 343:
#line 1421 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DPCM; }
#line 4804 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 344:
#line 1422 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value).id = CSSValueInvalid; (yyval.value).fValue = (yyvsp[0].number); (yyval.value).unit = CSSPrimitiveValue::CSS_FR; }
#line 4810 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 345:
#line 1425 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[-3].string);
        f->args = std::unique_ptr<CSSParserValueList>((yyvsp[-1].valueList));
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
#line 4823 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 346:
#line 1433 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[-2].string);
        f->args = std::unique_ptr<CSSParserValueList>(new CSSParserValueList);
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
#line 4836 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 347:
#line 1441 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[-3].string);
        f->args = nullptr;
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
  }
#line 4849 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 349:
#line 1452 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.value) = (yyvsp[0].value); (yyval.value).fValue *= (yyvsp[-1].integer); }
#line 4855 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 350:
#line 1455 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.character) = '+';
    }
#line 4863 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 351:
#line 1458 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.character) = '-';
    }
#line 4871 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 352:
#line 1461 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.character) = '*';
    }
#line 4879 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 353:
#line 1464 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.character) = '/';
    }
#line 4887 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 356:
#line 1470 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.valueList) = nullptr;
        if ((yyvsp[-2].valueList)) {
            (yyval.valueList) = (yyvsp[-2].valueList);
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = '(';
            (yyval.valueList)->insertValueAt(0, v);
            v.iValue = ')';
            (yyval.valueList)->addValue(v);
        }
    }
#line 4905 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 358:
#line 1484 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.valueList) = nullptr; delete (yyvsp[-1].valueList); }
#line 4911 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 359:
#line 1486 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.valueList) = new CSSParserValueList;
        (yyval.valueList)->addValue((yyvsp[0].value));
    }
#line 4920 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 360:
#line 1490 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<CSSParserValueList> expression((yyvsp[-2].valueList));
        (yyval.valueList) = nullptr;
        if (expression && (yyvsp[-1].character)) {
            (yyval.valueList) = expression.release();
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = (yyvsp[-1].character);
            (yyval.valueList)->addValue(v);
            (yyval.valueList)->addValue((yyvsp[0].value));
        } else {
            destroy((yyvsp[0].value));
        }
    }
#line 4940 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 361:
#line 1505 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<CSSParserValueList> left((yyvsp[-2].valueList));
        std::unique_ptr<CSSParserValueList> right((yyvsp[0].valueList));
        (yyval.valueList) = nullptr;
        if (left && (yyvsp[-1].character) && right) {
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = (yyvsp[-1].character);
            left->addValue(v);
            left->extend(*right);
            (yyval.valueList) = left.release();
        }
    }
#line 4959 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 364:
#line 1523 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        std::unique_ptr<CSSParserValueList> list((yyvsp[-4].valueList));
        std::unique_ptr<CSSParserValueList> expression((yyvsp[-1].valueList));
        (yyval.valueList) = nullptr;
        if (list && expression) {
            (yyval.valueList) = list.release();
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = ',';
            (yyval.valueList)->addValue(v);
            (yyval.valueList)->extend(*expression);
        }
    }
#line 4978 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 365:
#line 1539 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[-4].string);
        f->args = std::unique_ptr<CSSParserValueList>((yyvsp[-2].valueList));
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
#line 4991 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 366:
#line 1547 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = 0;
        YYERROR;
    }
#line 5001 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 369:
#line 1555 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = (yyvsp[-3].string);
        f->args = std::unique_ptr<CSSParserValueList>((yyvsp[-1].valueList));
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
#line 5014 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 370:
#line 1563 "./CSSGrammar.y" /* yacc.c:1646  */
    {
        (yyval.value).id = CSSValueInvalid;
        (yyval.value).unit = 0;
        YYERROR;
    }
#line 5024 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 376:
#line 1572 "./CSSGrammar.y" /* yacc.c:1646  */
    { parser->invalidBlockHit(); }
#line 5030 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;

  case 391:
#line 1580 "./CSSGrammar.y" /* yacc.c:1646  */
    { (yyval.location) = parser->currentLocation(); }
#line 5036 "./CSSGrammar.cpp" /* yacc.c:1646  */
    break;


#line 5040 "./CSSGrammar.cpp" /* yacc.c:1646  */
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

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

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
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 1587 "./CSSGrammar.y" /* yacc.c:1906  */

EA_WEBKIT_CSS_GRAMMAR_END();
