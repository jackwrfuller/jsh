#include "commands.h"
#include "parser.h"
#include "jsh.tab.h"
#include "lexer.h"


void build_table(job_table** job_table, char* input) {
    YY_BUFFER_STATE buf = yy_scan_string(input);   
    
    jt = *job_table;
    yyparse();
    *job_table = jt;

    yy_delete_buffer(buf);
}

int main() {
    job_table* jt;
    char* input = "job1; job2\n";
    build_table(&jt, input);
    
    print_table(jt);
    free(jt);

    return 0;
}
