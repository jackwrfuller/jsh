#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/parser.h"
#include "../../include/jsh.tab.h"
#include "../../include/lexer.h"

job_table* jt;

void build_table(job_table** job_table, char* input) {
    YY_BUFFER_STATE buf = yy_scan_string(input);   
    
    jt = *job_table;
    yyparse();
    *job_table = jt;

    yy_delete_buffer(buf);
}


job_table *create_job_table()
{
    job_table *jt;
    proc_table *pt;

    jt = (job_table *)malloc(sizeof(job_table));
    if (!jt)
    {
        fprintf(stderr, "malloc error");
        exit(1);
    }

    return jt;
}

void insert(job_table* jt, int i, int j, int k, char* arg) {
    proc_table* job = &jt->jobs[i];
    proc* proc = &job->procs[j];
    
    if (k >= 0 && k < MAX_ARGS_PER_PROC) {
        strncpy(proc->argv[k], arg, MAX_CHARS_PER_ARG - 1);
        proc->argv[k][MAX_CHARS_PER_ARG - 1] = '\0';
        proc->argc++;
        printf("WORD=%s ", proc->argv[k]);
    } else {
        printf("Error: index out bounds in insert");
    }
}

void set_redirection(job_table* jt, int job_num, char* file, int direction) {
    proc_table* job = &jt->jobs[job_num];
    
    if (direction == 0) {
        // Infile
        strncpy(job->infile, file, MAX_FILE_LEN - 1);
        job->infile[MAX_FILE_LEN - 1] = '\0';
    } else if (direction > 0) {
        // Outfile
        strncpy(job->outfile, file, MAX_FILE_LEN - 1);
        job->infile[MAX_FILE_LEN - 1] = '\0';
    } else {
        // Errfile
        strncpy(job->errfile, file, MAX_FILE_LEN - 1);
        job->infile[MAX_FILE_LEN - 1] = '\0';
    }

}

void print_table(job_table* jt) {
    printf("\nPRINTING RESULTING TABLE:\n");
    proc_table* pt;
    for (int i = 0; i < jt->jobc; i++) {
        printf("Job: %i\n", i);
        pt = &jt->jobs[i];

        proc* p;
        for (int j = 0; j < pt->procc; j++) {
            p = &pt->procs[j];

            char* arg;
            for (int k = 0; k < p->argc; k++) {
                arg = p->argv[k];
                printf("%s, ", arg);
            }
            printf(" | ");
        }
        printf("\n");
        printf("Infile: %s\n", pt->infile);
        printf("Outfile: %s\n", pt->outfile);
        printf("Errfile: %s\n", pt->errfile);
        printf("Foreground: %i\n", pt->foreground);
    }
}

/*int main() {*/
/*    job_table* jt;*/
/*    char* input = "job1; job2\n";*/
/*    build_table(&jt, input);*/
/**/
/*    print_table(jt);*/
/*    free(jt);*/
/**/
/*    return 0;*/
/*}*/
