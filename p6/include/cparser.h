#pragma once
#include "cscanner.h"

namespace C1 {
	class BisonParser;

	class Parser {
	public:
		Parser() : parser(scanner) {}

		int parse() {
			return parser.parse();
		}

	private:
		C1::FlexScanner scanner;
		C1::BisonParser parser;
	};
}
