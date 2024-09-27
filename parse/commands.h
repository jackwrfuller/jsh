#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_JOBS 64
#define MAX_ARGS_PER_PROC 64
#define MAX_PROC_PER_JOB 64

typedef struct __proc
{
    int argc;
    char* argv[MAX_ARGS_PER_PROC];
} proc;

typedef struct __proc_table
{
    int procc; // The number of commands stored currently.
    proc procs[MAX_PROC_PER_JOB];
    char *infile;
    char *outfile;
    char *errfile;
    int foreground;
} proc_table;

typedef struct __job_table
{
    int jobc;
    proc_table jobs[MAX_JOBS];
} job_table;

job_table *create_job_table();

void insert_arg(proc *proc, char *arg);
void insert_cmd(proc_table *proc_table, proc *proc);
void insert_job(job_table *job_table, proc_table *proc_table);

void insert(job_table* jt, int i, int j, int k, char* arg);

#endif
