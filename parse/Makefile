test: jsh.y jsh.l commands.c commands.h test.c
	bison -v -d jsh.y
	flex jsh.l
	gcc -g -c test.c -o test.o
	gcc -g -c commands.c -o commands.o
	gcc -g -c jsh.tab.c -o jsh.tab.o
	gcc -g -c lex.yy.c -o lex.yy.o
	gcc -g jsh.tab.o lex.yy.o commands.o test.o -o test

clean:
	rm test jsh.tab.o lex.yy.o commands.o test.o
