%language "c++"
%require "3.2" // requires bison of version higher than 3.2.

%code requires
{
	// Include your headers here.
	#include <iostream>
	#include <string>
}

%code provides
{
	// Bison does not define YYSTYPE and YYLTYPE in a C++ header file, so we
	// have to define them explicitly for flex.
	typedef yy::parser::semantic_type YYSTYPE;
	typedef yy::parser::location_type YYLTYPE;

	// Call yylex in C style.
	extern "C"
		int yylex(YYSTYPE *yylval,YYLTYPE *yylloc);
}

%defines "example.tab.h" // Generate a header file with specific filename.
%locations // Track token locations.
%define api.location.file "./location.h" // Generate a location.h header file so
						// that other files (e.g. ast.h) could include only them.
%parse-param {int &stmt_cnt} // To return something from a bison parser, you
							// should add references its argument.

%define api.value.type variant; // Define the return value of all symbols to be a
								// variant. (i.e. to be any type)
%token<int> NUMBER; // The return value of terminal NUMBER is an int, and so forth.
%token<std::string> WORD;
%token<int> EQUAL; // We never use the return value of EQUAL, simply define it as int.
%nterm<int> statement file; // %nterm is used to mark non-terminals.

// Define the precedence and (left or right) associativity of operators.
%right EQUAL

%%

// A file consists of several statements.
file		: file statement
				{
					stmt_cnt++;
					std::cout << " statement ends!" << std::endl;
				}
			| %empty
				{ stmt_cnt = 0; }

// A statement looks like "xx = yy = 3"
statement	: WORD EQUAL statement[substmt]
				{
					std::cout << " " << $WORD << " = " << $substmt << std::endl;
					$$ = $substmt;
				}
			| NUMBER
				{
					$$ = $NUMBER;
				}
%%

// The minimal error handling function.
namespace yy
{
	void parser::error(const location_type &loc, const std::string& msg)
	{
		std::cerr << "error at line " << loc.begin.line
			<< ": " << msg << std::endl;
	}

}

int main()
{
	int stmt_cnt;
	yy::parser(stmt_cnt).parse();
	std::cout << "matched " << stmt_cnt << " statements." << std::endl;
}