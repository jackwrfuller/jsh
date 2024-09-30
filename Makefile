CC = gcc 
CFLAGS = -g -Wall

SRCDIR = src/
INCDIR = include/
OBJDIR = build/
BINDIR = bin/
TARGET = jsh 

SRC = $(wildcard $(addsuffix *.c, $(SRCDIR))) src/parse/parse.c src/parse/jsh.tab.c src/parse/lex.yy.c
OBJ = $(addprefix $(OBJDIR), $(patsubst %.c, %.o, $(notdir $(SRC))))

VPATH = $(SRCDIR)


$(TARGET) :  build_parse $(OBJ) 
	@echo Linking...
	@mkdir -p $(BINDIR)
	@$(CC) $(CFLAGS) -o $(BINDIR)$@ $(OBJ)

$(OBJDIR)%.o : %.c 
	@echo Compiling $< in $@...
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) $(addprefix -I,$(INCDIR)) -c -o $@ $< 

build_parse: src/parse/jsh.y src/parse/jsh.l src/parse/parse.c
	@echo Compiling parse files...
	@mkdir -p $(OBJDIR)
	@bison -d src/parse/jsh.y -o $(OBJDIR)jsh.tab.c
	@flex -o $(OBJDIR)lex.yy.c src/parse/jsh.l
	@gcc -g -c $(OBJDIR)jsh.tab.c -o $(OBJDIR)jsh.tab.o
	@gcc -g -c $(OBJDIR)lex.yy.c -o $(OBJDIR)lex.yy.o
	@gcc -g -c src/parse/parse.c -o $(OBJDIR)parse.o 
	
.PHONY: clean
clean : 
	@$(RM) -r $(OBJDIR)
	@$(RM) -r $(BINDIR)

test_parse: src/parse/jsh.y src/parse/jsh.l src/parse/parse.c
	@echo Building parser subsystem binary for testing purposes...
	@mkdir -p $(OBJDIR)
	@bison -d src/parse/jsh.y -o $(OBJDIR)jsh.tab.c
	@flex -o $(OBJDIR)lex.yy.c src/parse/jsh.l
	@gcc -g -c $(OBJDIR)jsh.tab.c -o $(OBJDIR)jsh.tab.o
	@gcc -g -c $(OBJDIR)lex.yy.c -o $(OBJDIR)lex.yy.o
	@gcc -g -c src/parse/parse.c -o $(OBJDIR)parse.o
	@gcc -g -c tests/test_parse.c -o $(OBJDIR)test_parse.o
	@gcc -g -I$(INCDIR) $(OBJDIR)test_parse.o $(OBJDIR)parse.o $(OBJDIR)lex.yy.o $(OBJDIR)jsh.tab.o -o $(BINDIR)test_parse
