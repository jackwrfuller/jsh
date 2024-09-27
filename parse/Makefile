test: jsh.y jsh.l
	bison -d jsh.y
	flex jsh.l
	gcc -g -c commands.c -o commands.o
	gcc -g -c jsh.tab.c -o jsh.tab.o
	gcc -g -c lex.yy.c -o lex.yy.o
	gcc -g jsh.tab.o lex.yy.o commands.o -o test

clean:
	rm test jsh.tab.o lex.yy.o commands.o
