#ifndef CSSGRAMMAR_H
#define CSSGRAMMAR_H
/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison interface for Yacc-like parsers in C
   
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
/* Line 2053 of yacc.c  */
#line 65 "./CSSGrammar.y"

    double number;
    CSSParserString string;
    CSSSelector::MarginBoxType marginBox;
    CSSParserValue value;

/* Line 2053 of yacc.c  */
#line 176 "./CSSGrammar.y"
 CSSSelector::Relation relation; 
/* Line 2053 of yacc.c  */
#line 178 "./CSSGrammar.y"
 StyleRuleBase* rule; 
/* Line 2053 of yacc.c  */
#line 181 "./CSSGrammar.y"
 Vector<RefPtr<StyleRuleBase>>* ruleList; 
/* Line 2053 of yacc.c  */
#line 186 "./CSSGrammar.y"
 MediaQuerySet* mediaList; 
/* Line 2053 of yacc.c  */
#line 189 "./CSSGrammar.y"
 MediaQuery* mediaQuery; 
/* Line 2053 of yacc.c  */
#line 192 "./CSSGrammar.y"
 MediaQuery::Restrictor mediaQueryRestrictor; 
/* Line 2053 of yacc.c  */
#line 194 "./CSSGrammar.y"
 MediaQueryExp* mediaQueryExp; 
/* Line 2053 of yacc.c  */
#line 197 "./CSSGrammar.y"
 Vector<OwnPtr<MediaQueryExp>>* mediaQueryExpList; 
/* Line 2053 of yacc.c  */
#line 201 "./CSSGrammar.y"
 StyleKeyframe* keyframe; 
/* Line 2053 of yacc.c  */
#line 204 "./CSSGrammar.y"
 Vector<RefPtr<StyleKeyframe>>* keyframeRuleList; 
/* Line 2053 of yacc.c  */
#line 210 "./CSSGrammar.y"
 CSSPropertyID id; 
/* Line 2053 of yacc.c  */
#line 212 "./CSSGrammar.y"
 CSSParserSelector* selector; 
/* Line 2053 of yacc.c  */
#line 215 "./CSSGrammar.y"
 Vector<OwnPtr<CSSParserSelector>>* selectorList; 
/* Line 2053 of yacc.c  */
#line 218 "./CSSGrammar.y"
 bool boolean; 
/* Line 2053 of yacc.c  */
#line 220 "./CSSGrammar.y"
 CSSSelector::Match match; 
/* Line 2053 of yacc.c  */
#line 222 "./CSSGrammar.y"
 int integer; 
/* Line 2053 of yacc.c  */
#line 224 "./CSSGrammar.y"
 char character; 
/* Line 2053 of yacc.c  */
#line 226 "./CSSGrammar.y"
 CSSParserValueList* valueList; 
/* Line 2053 of yacc.c  */
#line 231 "./CSSGrammar.y"
 CSSParser::Location location; 

/* Line 2053 of yacc.c  */
#line 217 "./CSSGrammar.hpp"
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
#endif
