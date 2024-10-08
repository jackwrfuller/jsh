#ifndef PARSER_H
#define PARSER_H


#define MAX_JOBS 16
#define MAX_PROC_PER_JOB 16
#define MAX_ARGS_PER_PROC 16
#define MAX_CHARS_PER_ARG 64
#define MAX_FILE_LEN 256

typedef struct __proc
{
    int argc;
    char argv[MAX_ARGS_PER_PROC][MAX_CHARS_PER_ARG];
} proc;

typedef struct __proc_table
{
    int procc; // The number of commands stored currently.
    proc procs[MAX_PROC_PER_JOB];
    char infile[MAX_FILE_LEN];
    char outfile[MAX_FILE_LEN];
    char errfile[MAX_FILE_LEN];
    int foreground;
} proc_table;

typedef struct __job_table
{
    int jobc;
    proc_table jobs[MAX_JOBS];
} job_table;


extern job_table* jt;

void print_table(job_table* job_table);
void build_table(job_table** job_table, char* input);
void insert(job_table* jt, int i, int j, int k, char* arg);
void set_redirection(job_table* jt, int job_num, char* file, int direction);
job_table *create_job_table();

#endif
