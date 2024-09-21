CC = gcc 
CFLAGS = -g -Wall

SRCDIR = src/
INCDIR = include/
OBJDIR = build/
BINDIR = bin/
TARGET = jsh 

SRC = $(wildcard $(addsuffix *.c, $(SRCDIR)))
OBJ = $(addprefix $(OBJDIR), $(patsubst %.c, %.o, $(notdir $(SRC))))

VPATH = $(SRCDIR)


$(TARGET) : $(OBJ)
	@echo Linking...
	@mkdir -p $(BINDIR)
	@$(CC) $(CFLAGS) -o $(BINDIR)$@ $(OBJ)

$(OBJDIR)%.o : %.c 
	@echo Compiling $< in $@...
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) $(addprefix -I,$(INCDIR)) -c -o $@ $< 

clean : 
	@$(RM) -r $(OBJDIR)
	@$(RM) -r $(BINDIR)
