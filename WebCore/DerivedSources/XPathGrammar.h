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
/* Line 2053 of yacc.c  */
#line 54 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 NumericOp::Opcode numericOpcode; 
/* Line 2053 of yacc.c  */
#line 57 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 EqTestOp::Opcode equalityTestOpcode; 
/* Line 2053 of yacc.c  */
#line 64 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 StringImpl* string; 
/* Line 2053 of yacc.c  */
#line 68 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Step::Axis axis; 
/* Line 2053 of yacc.c  */
#line 74 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 LocationPath* locationPath; 
/* Line 2053 of yacc.c  */
#line 78 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Step::NodeTest* nodeTest; 
/* Line 2053 of yacc.c  */
#line 82 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Vector<std::unique_ptr<Expression>>* expressionVector; 
/* Line 2053 of yacc.c  */
#line 86 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Step* step; 
/* Line 2053 of yacc.c  */
#line 90 "/home/abaldeva/WEBKIT~1/Source/WebCore/xml/XPathGrammar.y"
 Expression* expression; 

/* Line 2053 of yacc.c  */
#line 107 "./XPathGrammar.hpp"
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
#endif
