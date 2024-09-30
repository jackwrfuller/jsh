#ifndef JSH_H
#define JSH_H

#include <stdio.h>
#include <sys/types.h>
#include <termios.h>
#include <stddef.h>


#define RL_BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

#define IN_FOREGROUND 1

/* Stevens Convention Wrappers */
#define CHECK_ALLOC(ptr) \
    if ((ptr) == NULL) { \
        fprintf(stderr, "Memory allocation error at %s:%d\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    }

/* Macro for wrapping malloc with error checking */
#define Malloc(size) ({               \
    void *ptr = malloc(size);         \
    CHECK_ALLOC(ptr);                 \
    ptr;                              \
})

typedef struct process {
    struct process* next;   /* next process in pipeline */ 
    char** argv;            /* for exec */ 
    pid_t pid;              /* process ID */ 
    char completed;         /* true if process has completed */ 
    char stopped;           /* true if process has stopped */ 
    int status;             /* reported status value */
} process;

typedef struct job {
    struct job* next;           /* next active job */  
    char* command;              /* command line, used for messages */ 
    process* first_process;     /* list of processes in this job */ 
    pid_t pgid;                 /* process group ID */ 
    char notified;              /* true if user told about stopped job */ 
    struct termios tmodes;      /* saved terminal modes */ 
    int stdin, stdout, stderr;  /* standard i/o channels */  
} job;

// Global job list 
extern job* first_job;

/* Keep track of attributes of the shell */ 
extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern int shell_is_interactive;


extern char* bi_list[];
extern int (*bi_func[]) (char **);
int num_bi();

void init();
void launch_job(job* j, int foreground);
job* parse_line(char* line);

void free_job_list(job* j);

int job_is_stopped(job* j);
int job_is_completed(job* j);
void put_job_in_foreground(job* j, int cont);
void put_job_in_background(job* j, int cont);

// Utility functions
void check_malloc(void* ptr);

#endif
