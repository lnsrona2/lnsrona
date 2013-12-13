CINC	 = -Iinclude
CONF	 = config
SRC	 = src
BIN	 = bin
UTILSRC  = $(SRC)/list.c $(SRC)/error.c $(SRC)/ast.c $(SRC)/generate.c

CC       = gcc $(CINC)
LEX      = flex -i -I 
YACC     = bison -d -y

all: c0mid inter

inter: $(SRC)/interpret.c 
	$(CC) -DDEBUG -o $(BIN)/inter $(SRC)/interpret.c -ll -lm

$(SRC)/c0ast.tab.c : $(CONF)/c0ast.y
	$(YACC) -b -o $(SRC)/c0ast.tab.c $(CONF)/c0ast.y

$(SRC)/c0ast.lex.c : $(CONF)/c0ast.lex
	$(LEX) -o$(SRC)/c0ast.lex.c $(CONF)/c0ast.lex

# Generates a compiler for an assignment statement sequence language,
# which constructs an abstract syntax tree for an input program.
c0mid : $(SRC)/c0ast.lex.c $(SRC)/c0ast.tab.c
	$(CC) -DDEBUG -o $(BIN)/c0mid $(SRC)/c0ast.lex.c $(SRC)/symtab.c $(SRC)/c0ast.tab.c $(SRC)/shell.c $(UTILSRC) -ll -lm

clean:
	rm -f *.BAK *.o core *~* *.a 
	rm -f $(SRC)/*.tab.h $(SRC)/*.tab.c
	rm -f $(SRC)/*.lex.c *.out
