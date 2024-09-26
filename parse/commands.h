#ifndef COMMANDS_H
#define COMMANDS_H 


#define MAX_JOBS 64
#define MAX_ARGS 64
#define MAX_CMDS 64


typedef struct __simple_cmd {
    int argc;
    char** argv;
} simple_cmd;

typedef struct __cmd_table {
    int cmdc;                   // The number of commands stored currently.
    simple_cmd* simple_cmds;
    char* infile;
    char* outfile;
    char* errfile;
    int foreground;
} cmd_table;

typedef struct __job_table {
    int jobc;
    cmd_table* jobs;
} job_table;

job_table* create_job_table();


void insert_arg(simple_cmd* simple_cmd, char* arg);
void insert_cmd(cmd_table* cmd_table, simple_cmd* simple_cmd);
void insert_job(job_table* job_table, cmd_table* cmd_table);

#endif
