CINC	 = -Iinclude
CONF	 = config
SRC	 = src
BIN	 = bin
UTILSRC  = $(SRC)/list.c $(SRC)/error.c $(SRC)/ast.c

CC       = gcc -g $(CINC)
LEX      = flex -i -I 
YACC     = bison -d -y

all: c0ast

c0ast:   $(CONF)/c0ast.y $(CONF)/c0ast.lex
	$(YACC) -b c0ast -o $(SRC)/c0ast.tab.c $(CONF)/c0ast.y
	$(LEX) -o$(SRC)/c0ast.lex.c $(CONF)/c0ast.lex
	$(CC) -DDEBUG -o $(BIN)/c0ast $(SRC)/c0ast.lex.c $(SRC)/symtab.c $(SRC)/c0ast.tab.c $(UTILSRC) -ll -lm

clean:
	rm -f *.BAK *.o core *~* *.a 
	rm -f $(SRC)/*.tab.h $(SRC)/*.tab.c
	rm -f $(SRC)/*.lex.c *.out
