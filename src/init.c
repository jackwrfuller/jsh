#include <complex.h>
#include <termios.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/jsh.h"

/* 
 * Make sure the shell is runnning interactively as the
 * foreground job before proceeding.
 */
void init() {
    // See if we are running interactively
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty(shell_terminal);

    if (shell_is_interactive) {
        /* Loop until shell is in foreground */ 
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp())) {
            kill(-shell_pgid, SIGTTIN);
        }

        /* Ignore interactive and job-control signals */ 
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);

        /* Put oursselves in the our own process group */ 
        shell_pgid = getpid();
        if (setpgid(shell_pgid, shell_pgid) < 0) {
            perror("Couldn't put shell in its own process group");
            exit(1);
        }

        // Grab control of the terminal 
        tcsetpgrp(shell_terminal, shell_pgid);

        // Save default terminal attributes for the shell 
        tcgetattr(shell_terminal, &shell_tmodes);
    }
}
