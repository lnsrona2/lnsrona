#include <fstream>
#include "driver.h"
using namespace std;
using namespace C1;

int main(int argc, char *argv [])
{
	if (argc < 2)
	{
		cout << "missing command parameter : [source file(s)]" << endl;
		return 1;
	}
	string sourceFileName(argv[1]);
	ifstream source(sourceFileName);
	//ConsoleDriver driver(&source, &cout);
	C1::Parser parser(source, sourceFileName);
	auto ast = parser.parse();
	cout << ast->ToString();
}