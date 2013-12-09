CINC	 = -Iinclude
CONF	 = config
SRC	 = src
BIN	 = bin

CC       = gcc -g $(CINC)
LEX      = flex -i -I 
YACC     = bison -d -y

all: c0

c0:	$(CONF)/c0.y $(CONF)/c0.lex 
	$(YACC) -b c0 -o $(SRC)/c0.tab.c $(CONF)/c0.y
	$(LEX) -o$(SRC)/c0.lex.c $(CONF)/c0.lex
	$(CC) -o $(BIN)/c0 $(SRC)/c0.lex.c $(SRC)/c0.tab.c  -ll -lm

clean:
	rm -f *.BAK *.o core *~* *.a 
	rm -f $(SRC)/*.tab.h $(SRC)/*.tab.c
	rm -f $(SRC)/*.lex.c *.out
