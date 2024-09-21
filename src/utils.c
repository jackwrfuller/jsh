#include "../include/jsh.h"


extern job* first_job;

/* Find active job with given pgid */
job* find_active_job(pid_t pgid) {
    job* j;

    for (j = first_job; j; j = j->next) {
        if (j->pgid == pgid) {
            return j;
        }
    }
    return NULL;
}

/* Return true if all processes in a job have stopped or completed */
int job_is_finished(job* j) {
    process* p;

    for (p = j->first_process; p; p = p->next) {
        if (!p->completed && !p->stopped) {
            return 0;
        }
    }
    return 1;
}

int job_is_completed(job* j) {
    process* p;

    for (p = j->first_process; p; p = p->next) {
        if (!p->completed) {
            return 0;
        }
    }
    return 1;
}
