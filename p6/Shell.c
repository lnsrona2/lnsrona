/*
* The shell of the compilier, interpret the command line arguments, output to file
*/

#include "common.h"
#include "C0.h"
#include <string.h>

Table symtab = NULL; //Strong defineation
ASTTree ast = NULL;

int main(int argc, char *argv[])
{
	symtab = newTable();
	ast = newAST();

	// Section of parsing command line arguments
	// Added by Yupeng
	int k, flag = 0;
	for (k = 1; k<argc; k++)
	{
		if (argv[k][0] != '-') {
			strcpy(infilename, argv[1]);
			freopen(infilename, "r", stdin);
			flag = 1;
		}
		else {
			if (argv[k][1] == 'o') {
				k++;
				//				printf("outputfile : argv[%d] : %s\n",k,argv[k]);
				if (k == argc) {
					printf("Error : Inlegal command. There must be a param after -o option.\n");
				}
				else
				{
					strcpy(outfilename, argv[k]);
					outfile = fopen(outfilename, "w");
				}
			}
		}
	}
	if (!flag) printf("Warning : haven't decide a source file, system will take the standard input as source.\n");
	if (outfile == NULL)
		outfile = fopen("a.o", "w");
	// End of Arguments parsing

	printf("Parsing ...\n");
	yyparse();
	printf("\n\nDump the program from the generated AST:\n  ");
	dumpAST(ast->root);

	// Section of code generate and output to file
	// Added by Yupeng Zhang
	generate(ast->root); 
	
	fwrite(&cx, sizeof(long), 1, outfile);
	fwrite(code, sizeof(instruction), cx, outfile);
	// End of Section

	destroyAST(&ast->root);
	printf("\n\nFinished destroying AST.\n");
	destroyTable(&symtab);
	printf("\n\nFinished destroying symbolic table.\n");
	return(0);
}
