test: jsh.y jsh.l
	bison -d jsh.y
	flex jsh.l
	gcc -c commands.c -o commands.o
	gcc -c jsh.tab.c -o jsh.tab.o
	gcc -c lex.yy.c -o lex.yy.o
	gcc jsh.tab.o lex.yy.o commands.o -o test

clean:
	rm test jsh.tab.o lex.yy.o commands.o
