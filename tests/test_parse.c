#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/jsh.tab.h"


int main() {
    const char* input = "ls";
    
    YY_BUFFER_STATE buf = yy_scan_string(input);   
    
    yyparse();
    print_table(jt);

    yy_delete_buffer(buf);
    return 0;
}