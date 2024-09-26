#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "commands.h"

void setup_cmd(simple_cmd* cmd) {
    char** argv;

    cmd->argc = 0;
    
    argv = malloc(MAX_ARGS * sizeof(char*));
    if (!argv) {
        fprintf(stderr, "malloc error");
        exit(1);
    }
    cmd->argv = argv;
}

void setup_cmd_table(cmd_table* ct) {
    simple_cmd* sc;
    
    ct->cmdc = 0;

    sc = (simple_cmd*) malloc(MAX_CMDS * sizeof(simple_cmd));
    if (!sc) {
        fprintf(stderr, "malloc error");
        exit(1);
    }
    ct->simple_cmds = sc;
    ct->infile = 0;
    ct->outfile = 0;
    ct->errfile = 0;
    ct->foreground = 0;

    for (int i = 0; i < MAX_CMDS; i++) {
        setup_cmd(&ct->simple_cmds[i]);
    }
}


job_table* create_job_table() {
    job_table* jt;
    cmd_table* ct;

    jt = (job_table*) malloc(sizeof(job_table));
    if (!jt) {
        fprintf(stderr, "malloc error");
        exit(1);
    }
    
    ct = (cmd_table*) malloc(MAX_JOBS * sizeof(cmd_table));
    if (!ct) {
        fprintf(stderr, "malloc error");
        exit(1);
    }
    jt->jobs = ct;
    jt->jobc = 0;

    for (int i = 0; i < MAX_JOBS; i++) {
        setup_cmd_table(&jt->jobs[i]);
    }

    return jt;
}


void insert_arg(simple_cmd* simple_cmd, char* arg) {
    if (simple_cmd->argc >= MAX_ARGS) {
        exit(1);
    }
    simple_cmd->argc += 1;
    simple_cmd->argv[simple_cmd->argc] = strdup(arg); 
}


