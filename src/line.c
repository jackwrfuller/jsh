#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/jsh.h"
#include "../include/parser.h"

char** split_line(char* line) {
    if (line == NULL) {
        return NULL;
    }

    int bufsize = TOK_BUFSIZE;
    int position = 0;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;

    check_malloc(tokens);
    
    //TODO use reentrant version strtok_r instead.
    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position += 1;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            check_malloc(tokens);
        }

        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

job* parse_line(char* line) {
    job* j;
    process* p;
    char** argv;
    
    job_table* jt;
    build_table(&jt, line);
    if (jt) {
        print_table(jt);
    }
    free(jt);
    
    p = (process*) malloc(sizeof(process));
    check_malloc(p);
    
    argv = split_line(line);
    p->argv = argv;

    p->next = NULL;

    j = (job*) malloc(sizeof(job));
    check_malloc(j);
    
    j->first_process = p;
    j->stdin = STDIN_FILENO;
    j->stdout = STDOUT_FILENO;
    j->stderr = STDERR_FILENO;
    j->next = NULL;
    j->command = argv[0];
    j->pgid = 0;


    
    return j;
}
