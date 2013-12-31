#pragma once

// Only include FlexLexer.h if it hasn't been already included
#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

// Override the interface for yylex since we namespaced it
#undef YY_DECL
#define YY_DECL int C1::FlexScanner::yylex()
//#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
//	yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
//	yycolumn += yyleng;
// Include Bison for types / tokens
#include "c1.tab.hpp"


namespace C1 {
	class FlexScanner : public yyFlexLexer {
	public:
		// save the pointer to yylval so we can change it, and invoke scanner
		int yylex(C1::BisonParser::semantic_type * pVal,C1::BisonParser::location_type * pLoc)
		{
			int retval;
			yylval = pVal;
			yylloc = pLoc;
			retval = yylex();
			//pLoc->begin.line = pLoc->end.line = yylineno;
			//pLoc->begin.column = yycolumn;
			//pLoc->end.column = yycolumn + yyleng - 1;
			//yycolumn += yyleng;
			return retval;
		}

	private:
		// Scanning function created by Flex; make this private to force usage
		// of the overloaded method so we can get a pointer to Bison's yylval
		int yylex();

		// point to yylval (provided by Bison in overloaded yylex)
		C1::BisonParser::semantic_type * yylval;
		C1::BisonParser::location_type * yylloc;
		unsigned int yycolumn;
	};
}

