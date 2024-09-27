#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"

// void setup_proc(proc* proc)
// {
//     char **argv;
//     char* arg;

//     proc->argc = 0;

//     argv = malloc(MAX_ARGS_PER_PROC * sizeof(char *));
//     if (!argv)
//     {
//         fprintf(stderr, "malloc error");
//         exit(1);
//     }
//     proc->argv = argv;
// }

// void setup_proc_table(proc_table *ct)
// {
//     proc *sc;

//     ct->procc = 0;

//     sc = (proc *)malloc(MAX_PROC_PER_JOB * sizeof(proc));
//     if (!sc)
//     {
//         fprintf(stderr, "malloc error");
//         exit(1);
//     }
//     ct->procs = sc;
//     ct->infile = 0;
//     ct->outfile = 0;
//     ct->errfile = 0;
//     ct->foreground = 0;

//     for (int i = 0; i < MAX_PROC_PER_JOB; i++)
//     {
//         setup_proc(&ct->procs[i]);
//     }
// }

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

    // pt = (proc_table *)malloc(MAX_JOBS * sizeof(proc_table));
    // if (!pt)
    // {
    //     fprintf(stderr, "malloc error");
    //     exit(1);
    // }
    // jt->jobs = pt;
    // jt->jobc = 0;

    // for (int i = 0; i < MAX_JOBS; i++)
    // {
    //     setup_proc_table(&jt->jobs[i]);
    // }

    return jt;
}

// void insert_arg(proc *proc, char *arg)
// {
//     if (proc->argc >= MAX_ARGS_PER_PROC)
//     {
//         exit(1);
//     }
//     strcpy(proc->argv[proc->argc], "TEST");
//     proc->argc += 1;
// }

void insert(job_table* jt, int i, int j, int k, char* arg) {
    proc_table* job = &jt->jobs[i];
    proc* proc = &job->procs[j];
    char* argp = proc->argv[k];
    strcpy(argp, arg);
}
