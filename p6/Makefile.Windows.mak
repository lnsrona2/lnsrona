CINC	= -Iinclude
CONF	= config
SRC		= src
BIN		= bin

CC       = gcc $(CINC)
LEX		= flex -i -I
YACC	= bison -d -v -y
DEL		= del

all: $(SRC)/c0ast.tab.c $(SRC)/c0ast.lex.c

$(SRC)/c0ast.tab.c : $(CONF)/c0ast.y
	$(YACC) -o $(SRC)/c0ast.tab.c $(CONF)/c0ast.y

$(SRC)/c0ast.lex.c : $(CONF)/c0ast.lex
	$(LEX) -o$(SRC)/c0ast.lex.c $(CONF)/c0ast.lex

clean:
	$(DEL) $(SRC)\c0ast.lex.c
	$(DEL) $(SRC)\c0ast.tab.c
	$(DEL) $(SRC)\c0ast.tab.h
	$(DEL) $(SRC)\c0ast.output
