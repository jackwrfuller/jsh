#include <stdio.h>
#include "parser.h"
#include "jsh.tab.h"
#include "lexer.h"


int main() {
    
    char* input = "job1; job2\n";

    YY_BUFFER_STATE buf = yy_scan_string(input);

    if (yyparse() == 0) {
        printf("Parsing completed successfully!\n");
        print_table();
    } else {
        printf("Prasing failed.\n");
    }
    
    yy_delete_buffer(buf);

    return 0;
}
