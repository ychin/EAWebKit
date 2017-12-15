#ifndef CSSGRAMMAR_H
#define CSSGRAMMAR_H
/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 65 "./CSSGrammar.y" /* yacc.c:1909  */

    double number;
    CSSParserString string;
    CSSSelector::MarginBoxType marginBox;
    CSSParserValue value;
#line 202 "./CSSGrammar.y" /* yacc.c:1909  */
 CSSParserSelectorCombinator relation; 
#line 204 "./CSSGrammar.y" /* yacc.c:1909  */
 StyleRuleBase* rule; 
#line 207 "./CSSGrammar.y" /* yacc.c:1909  */
 Vector<RefPtr<StyleRuleBase>>* ruleList; 
#line 212 "./CSSGrammar.y" /* yacc.c:1909  */
 MediaQuerySet* mediaList; 
#line 215 "./CSSGrammar.y" /* yacc.c:1909  */
 MediaQuery* mediaQuery; 
#line 218 "./CSSGrammar.y" /* yacc.c:1909  */
 MediaQuery::Restrictor mediaQueryRestrictor; 
#line 220 "./CSSGrammar.y" /* yacc.c:1909  */
 MediaQueryExp* mediaQueryExp; 
#line 223 "./CSSGrammar.y" /* yacc.c:1909  */
 Vector<CSSParser::SourceSize>* sourceSizeList; 
#line 230 "./CSSGrammar.y" /* yacc.c:1909  */
 Vector<std::unique_ptr<MediaQueryExp>>* mediaQueryExpList; 
#line 234 "./CSSGrammar.y" /* yacc.c:1909  */
 StyleKeyframe* keyframe; 
#line 237 "./CSSGrammar.y" /* yacc.c:1909  */
 Vector<RefPtr<StyleKeyframe>>* keyframeRuleList; 
#line 243 "./CSSGrammar.y" /* yacc.c:1909  */
 CSSPropertyID id; 
#line 245 "./CSSGrammar.y" /* yacc.c:1909  */
 CSSParserSelector* selector; 
#line 248 "./CSSGrammar.y" /* yacc.c:1909  */
 Vector<std::unique_ptr<CSSParserSelector>>* selectorList; 
#line 252 "./CSSGrammar.y" /* yacc.c:1909  */
 bool boolean; 
#line 254 "./CSSGrammar.y" /* yacc.c:1909  */
 CSSSelector::Match match; 
#line 256 "./CSSGrammar.y" /* yacc.c:1909  */
 int integer; 
#line 258 "./CSSGrammar.y" /* yacc.c:1909  */
 char character; 
#line 260 "./CSSGrammar.y" /* yacc.c:1909  */
 CSSParserValueList* valueList; 
#line 264 "./CSSGrammar.y" /* yacc.c:1909  */
 Vector<CSSParserString>* stringList; 
#line 269 "./CSSGrammar.y" /* yacc.c:1909  */
 CSSParser::Location location; 

#line 210 "./CSSGrammar.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int cssyyparse (CSSParser* parser);

#endif /* !YY_CSSYY_CSSGRAMMAR_HPP_INCLUDED  */
#endif
