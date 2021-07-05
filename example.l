 /* 
  * This is only an example skeleton of a flex file.
  * Note: line comment does not work properly, and you should use block comment.
  * Also note: you should add a space (or a tab) before a block comment.
  */
%{
	/* Include your header files here. */
	#include <iostream>
	#include "example.tab.h"
%}

 /* Generate a lexer file that can be combined with a bison parser.  */
%option bison-bridge

 /* Generate yylex with an extra location parameter. */
%option bison-locations

 /* 
  * yywrap comes from lib flex. However the platform does not use '-lfl' option
  * to compile our files, so simply disable yywrap so that we would no longer
  * be dependent of lib flex.
  */
%option noyywrap

 /* Let flex keeps track of the line number. */
%option yylineno

%{
	extern int yylineno; /* This is tracked by flex. */

	/* This macro is called when a token is matched to set the location. */
	#define YY_USER_ACTION \
		do\
		{\
			yylloc->begin = yy::position(/* filename= */nullptr, yylineno, 0);\
			yylloc->end = yy::position(nullptr, yylineno, 0);\
		}while(0);
%}

%%

[0-9]+		{
				yylval->emplace<int>() = atoi(yytext);
				std::cout << "  " << yytext << " is a number." << std::endl;
				return yy::parser::token::NUMBER;
			}
[a-z]+		{
				yylval->emplace<std::string>() = std::string(yytext);
				std::cout << "  " << yytext << " is a variable." << std::endl;
				return yy::parser::token::WORD;
			}
=			{
				std::cout << "  = is an operator." << std::endl;
				return yy::parser::token::EQUAL;
			}
[ \n\r]		; /* Skip the white spaces. */

%%