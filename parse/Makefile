test: jsh.y jsh.l
	bison -d jsh.y
	flex jsh.l
	gcc -c jsh.tab.c -o jsh.tab.o
	gcc -c lex.yy.c -o lex.yy.o
	gcc jsh.tab.o lex.yy.o -o test

clean:
	rm test jsh.tab.o lex.yy.o
