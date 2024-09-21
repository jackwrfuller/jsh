#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "../include/jsh.h"

void launch_process(process* p, pid_t pgid, int infile, int outfile, int errfile, int foreground) {
    pid_t pid;

    if (shell_is_interactive) {
        // Put the process into the process group and give the process
        // group the terminal, if appriopriate. This has to be done
        // by both the shell and in the individual child processes because
        // of potential race conditions.
        pid = getpid();
        if (pgid == 0) {
            pgid = pid;
        }
        setpgid(pid, pgid);
        if (foreground) {
            tcsetpgrp(shell_terminal, pgid);
        }

        // Set the handling for job control signals back to default
        signal (SIGINT, SIG_DFL);
        signal (SIGQUIT, SIG_DFL);
        signal (SIGTSTP, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        signal (SIGTTOU, SIG_DFL);
        signal (SIGCHLD, SIG_DFL);

        // Set the standard i/o channels of the new process 
        if (infile != STDIN_FILENO) {
            dup2(infile, STDIN_FILENO);
            close(infile);
        }
        if (outfile != STDOUT_FILENO) {
            dup2(outfile, STDOUT_FILENO);
            close(outfile);
        }
        if (errfile != STDERR_FILENO) {
            dup2(errfile, STDOUT_FILENO);
            close(errfile);
        }

        // Exec the new process 
        execvp(p->argv[0], p->argv);
        perror("execvp");
        exit(1);
    }
}
