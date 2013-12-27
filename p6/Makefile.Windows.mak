CINC	= -Iinclude
CONF	= config
INC		= include
SRC		= src
BIN		= bin

CC       = ml $(CINC)
LEX		= flex -i -I
YACC	= bison -v
DEL		= del
MOV		= move /y

all: $(SRC)/c1.tab.cpp $(SRC)/c1.lex.cpp

$(SRC)/c1.tab.cpp : $(CONF)/c1.y
	$(YACC) -o $(SRC)/c1.tab.cpp $(CONF)/c1.y
	$(MOV) src\c1.tab.hpp include\

	$(MOV) src\position.hh include\

	$(MOV) src\location.hh include\

	$(MOV) src\stack.hh include\


$(SRC)/c1.lex.cpp : $(CONF)/c1.lex
	$(LEX) -o$(SRC)/c1.lex.cpp $(CONF)/c1.lex

clean:
	$(DEL) $(SRC)\c1.lex.cpp
	$(DEL) $(SRC)\c1.tab.cpp
	$(DEL) $(SRC)\c1.output
	$(DEL) $(INC)\c1.tab.hpp
	$(DEL) $(INC)\position.hh
	$(DEL) $(INC)\location.hh
	$(DEL) $(INC)\stack.hh
