#ifndef JSH_H
#define JSH_H

#include <sys/types.h>
#include <termios.h>
#include <stddef.h>

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


/* Keep track of attributes of the shell */ 
extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern int shell_is_interactive;

void init();

#endif
