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

void copy_argv(process* src, proc* dst) {
    src->argv = Malloc(MAX_ARGS_PER_PROC * sizeof(char*));

    for (int i = 0; i < MAX_ARGS_PER_PROC; i++) {
        src->argv[i] = Malloc(MAX_CHARS_PER_ARG * sizeof(char));
        // Is this open to shellcode RCE?
        strncpy(src->argv[i], dst->argv[i], MAX_CHARS_PER_ARG);
    }
}

process* create_proc(proc* proc) {
    process* p;

    p = Malloc(sizeof(process));

    // Initialise values
    copy_argv(p, proc);
    p->pid = 0;
    p->completed = 0;
    p->stopped = 0;
    p->status = 0;

    return p;
}

/*
 * Given a proc_table, allocate and initialise a job struct.
 */
job* create_job(proc_table* pt) {
    job* job;
    process* first_proc;
    process* current_proc;
    process* tmp;

    if (pt == NULL || pt->procc <= 0) {
        return NULL;
    }
    job = Malloc(sizeof(job));

    first_proc = create_proc(&pt->procs[0]);
    if (first_proc == NULL) {
        return NULL;
    }
    job->first_process = first_proc;
    current_proc = job->first_process;

    for (int i = 1; i < pt->procc; i++) {
        tmp = create_proc(&pt->procs[i]);
        if (tmp == NULL) {
            return NULL;
        }

        current_proc->next = tmp;
        current_proc = current_proc->next;
    }

    return job;
}


/*
 * Create a linked list of jobs ready to be executed from a job_table.
 */
job* create_jobs_from_job_table(job_table* jt) {
    job* first_job;
    job* current_job;
    job* tmp;

    if (jt == NULL || jt->jobc == 0) {
        return NULL;
    }

    first_job = create_job(&jt->jobs[0]);
    if (first_job == NULL) {
        return NULL;
    }

    current_job = first_job;
    for (int i = 1; i < jt->jobc; i++) {
        tmp = create_job(&jt->jobs[i]);
        if (tmp == NULL) {
            return NULL;
        }
        current_job->next = tmp;
        current_job = current_job->next;
    }
    
    return first_job;
}

job* parse_line(char* line) {
    job* j;
    job_table* jt;
    
    build_table(&jt, line);
    if (jt) {
        print_table(jt);
    }

    j = create_jobs_from_job_table(jt);
    if (!j) {
        perror("parse_line");
        exit(1);
    }
    
    // p = (process*) malloc(sizeof(process));
    // check_malloc(p);
    
    // argv = split_line(line);
    // p->argv = argv;

    // p->next = NULL;

    // j = (job*) malloc(sizeof(job));
    // check_malloc(j);
    
    // j->first_process = p;
    // j->stdin = STDIN_FILENO;
    // j->stdout = STDOUT_FILENO;
    // j->stderr = STDERR_FILENO;
    // j->next = NULL;
    // j->command = argv[0];
    // j->pgid = 0;

    free(jt);
    return j;
}
