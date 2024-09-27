#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

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
    }
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

/*
 * Store the status of the process pid that was
 * returned by waitpid. 
 * 
 * Return 0 if all went well, nonzero otherwise.
 */
int mark_process_status(pid_t pid, int status) {
    job* j;
    process* p;

    if (pid > 0) {
        // Update the record for the process 
        for (j = first_job; j; j = j->next) {
            for (p = j->first_process; p; p = p->next) {
                if (p->pid == pid) {
                    p->status = status;
                    if (WIFSTOPPED(status)) {
                        p->stopped = 1;
                    } else {
                        p->completed = 1;
                        if (WIFSIGNALED(status)) {
                            fprintf(stderr, "%d: Terminated by signal %d.\n", (int) pid, WTERMSIG(p->status));
                        }
                    }
                    return 0;
                }
            }
        }
        fprintf(stderr, "No child process %d.\n", pid);
        return -1;
    } else if (pid == 0 || errno == ECHILD) {
        // No process ready to report 
        return -1;
    } else {
        // Other weird errors
        perror("waitpid");
        return -1;
    }
}

/*
 * Check for processes that have status information available,
 * blocking until all processes in the given job have reported.
 */
void wait_for_job(job* j) {
    int status;
    pid_t pid;

    do {
        pid = waitpid(WAIT_ANY, &status, WUNTRACED);
    } while ( !mark_process_status(pid, status) && !job_is_stopped(j) && !job_is_completed(j) );
}


/*
 * Check for processes that have status information available,
 * without blocking.
 */
void update_status() {
    int status;
    int pid;

    do {
        pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
    } while (!mark_process_status(pid, status));
}

/*
 * Format information about job status for user to look at.
 */
void format_job_info(job* j, const char* status) {
    fprintf(stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

/*
 * Notify the user about stopped or terminated jobs.
 * Delete terminated jobs from the active job list.
 */
void do_job_notification() {
    job *j, *jprev, *jnext;

    // Update status information for child processes
    update_status();

    jprev = NULL;
    for (j = first_job; j; j = j->next) {
        jnext = j->next;

        // If all processes have completed, tell the user
        // the job has completed and delete it from the list
        // of active jobs
        if (job_is_completed(j)) {
            format_job_info(j, "completed");
            if (jprev) {
                jprev->next = jnext;
            } else {
                first_job = jnext;
            }
        } else if (job_is_stopped(j) && !j->notified) {
            format_job_info(j, "stopped");
            j->notified = 1;
            jprev = j;
        } else {
            jprev = j;
        }
    }
}

/*
 * Mark a stopped job as running 
 */
void mark_job_as_running(job* j) {
    process* p;

    for (p = j->first_process; p; p = p->next) {
        p->stopped = 0;
    }
    j->notified = 0;
}

/*
 * Continue job j 
 */
void continue_job(job* j, int foreground) {
    mark_job_as_running(j);
    if (foreground) {
        put_job_in_foreground(j, 1);
    } else {
        put_job_in_background(j, 1);
    }
}

/*
 * Put job j in the foreground. If const is nonzero, restore the
 * saved terminal modes  and send the process group a SIGCONT signal 
 * to wake it up before we block.
 */
void put_job_in_foreground(job* j, int cont) {
    // Put job in the foreground 
    tcsetpgrp(shell_terminal, j->pgid);

    // Send the job a continue signal, if necessary 
    if (cont) {
        tcsetattr(shell_terminal, TCSADRAIN, &j->tmodes);
        if (kill(- j->pgid, SIGCONT) < 0) {
            perror("kill (SIGCONT)");
        }
    }

    // Wait for job to report 
    wait_for_job(j);

    // Put shell back in foreground 
    tcsetpgrp(shell_terminal, shell_pgid);

    // Restore the shell's terminal modes 
    tcgetattr(shell_terminal, &j->tmodes);
    tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);
}

/*
 * Put a job in the background. If cont is true, send the
 * process group a SIGCONT signal to wake it up.
 */
void put_job_in_background(job* j, int cont) {
    // Send the job a continue signal, if necessary 
    if (cont) {
        if (kill(- j->pgid, SIGCONT) < 0) {
            perror("kill (SIGCONT)");
        }
    }
}


void launch_job(job* j, int foreground) {
    process* p;
    pid_t pid;
    int mypipe[2], infile, outfile;

    infile = j->stdin;
    for (p = j->first_process; p; p = p->next) {
        // Set up pipes if necessary
        if (p->next != NULL) {
            if (pipe(mypipe) < 0) {
                perror("pipe");
                exit(1);
            }
            outfile = mypipe[1];
        } else {
            outfile = j->stdout;
        }
        
        // Check for built-in command
        for (int i = 0; i < num_bi(); i++) {
            if (strcmp(p->argv[0], bi_list[i]) == 0) {
                (*bi_func[i])(p->argv);
                return;
            }
        }

        // Fork the child process 
        pid = fork();
        if (pid == 0) {
            // Child process 
            launch_process(p, j->pgid, infile, outfile, j->stderr, foreground);
        } else if (pid < 0) {
            // Fork failed 
            perror("fork");
            exit(1);
        } else {
            // Parent process 
            p->pid = pid;
            if (shell_is_interactive) {
                if (!j->pgid) {
                    j->pgid = pid;
                }
                setpgid(pid, j->pgid);
            }

            // Clean up pipes 
            if (infile != j->stdin) {
                close(infile);
            }
            if (outfile != j->stdout) {
                close(outfile);
            }
            infile = mypipe[0];
        }
        
        //format_job_info(j, "launched");

        if (!shell_is_interactive) {
            wait_for_job(j);
        } else if (foreground) {
            put_job_in_foreground(j, 0);
        } else {
            put_job_in_background(j,0);
        }
    }

}

/*
 * Free all memory assicated with a process
 */
void free_process(process* p) {
    free(p->argv);
    free(p);
}

/*
 * Free all memory associated with a list of jobs
 */
void free_job_list(job* j) {
    job* tmp;
    job* current_job;

    current_job = j;
    while (current_job) {
        process* p;
        process* current_proc;
        
        current_proc = current_job->first_process;
        while (current_proc) {
            p = current_proc;
            current_proc = current_proc->next;
            free_process(p);
        }

        tmp = current_job;
        current_job = current_job->next;
        free(tmp);
    }
}




