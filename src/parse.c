#include <string.h>
#include <stdlib.h>

#include "../include/jsh.h"


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
