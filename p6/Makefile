CINC	 = -Iinclude
CONF	 = config
SRC	 = src
BIN	 = bin
UTILSRC  = $(SRC)/list.c $(SRC)/error.c $(SRC)/ast.c

CC       = gcc -g $(CINC)
LEX      = flex -i -I 
YACC     = bison -d -y

all: c0 c0ast c0mid

# Generates a compiler for a simple expression language,
# the grammar is ambiguous, and uses precedence declarations
c0:	$(CONF)/c0.y $(CONF)/c0.lex
	$(YACC) -b c0 -o $(SRC)/c0.tab.c $(CONF)/c0.y
	$(LEX) -o$(SRC)/c0.lex.c $(CONF)/c0.lex
	$(CC) -o $(BIN)/c0 $(SRC)/c0.lex.c $(SRC)/c0.tab.c -ll -lm


# Generates a compiler for an assignment statement sequence language,
# which constructs an abstract syntax tree for an input program.
c0mid:   $(CONF)/c0ast_inter.y $(CONF)/c0ast.lex
	$(YACC) -b c0mid -o $(SRC)/c0ast.tab.c $(CONF)/c0ast_inter.y
	$(LEX) -o$(SRC)/c0ast.lex.c $(CONF)/c0ast.lex
	$(CC) -DDEBUG -o $(BIN)/c0mid $(SRC)/c0ast.lex.c $(SRC)/symtab.c $(SRC)/c0ast.tab.c $(UTILSRC) -ll -lm

clean:
	rm -f *.BAK *.o core *~* *.a 
	rm -f $(SRC)/*.tab.h $(SRC)/*.tab.c
	rm -f $(SRC)/*.lex.c *.out
