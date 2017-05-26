/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PROJECT = 258,
     TEXT = 259,
     DATE = 260,
     TIME = 261,
     LONG = 262,
     INT = 263,
     FILE_ = 264,
     REAL = 265,
     BOOL = 266,
     ENTITY = 267,
     LBRACE = 268,
     RBRACE = 269,
     PK = 270,
     UNIQUE = 271,
     SEMICOLON = 272,
     RELATIONSHIP = 273,
     BILATERAL = 274,
     LPAR = 275,
     RPAR = 276,
     FROM = 277,
     TO = 278,
     UPTO = 279,
     ARITY = 280,
     IDENTIFIER = 281,
     WEAK = 282
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 62 "gen.y"

	gen::Field *field;
	gen::Fields *fields;
	char *str;
	char chr;
	gen::Entity *entity;
	gen::Relationship *rel;
	gen::Type type;



/* Line 1685 of yacc.c  */
#line 90 "gen_parser.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


