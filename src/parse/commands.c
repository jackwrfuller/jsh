#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"


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
