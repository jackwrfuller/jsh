#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>
#include <termios.h>
#include <sys/types.h>
#include <wait.h>
#include <pwd.h>

#include "../include/jsh.h"

/* Forward declarations */
int bi_cd(char** args);
int bi_help(char** args);
int bi_exit(char** args);
int bi_clear(char** args);

static char* prev_line;

job* first_job = NULL;

char* bi_list[] = {
    "cd", 
    "help", 
    "exit",
    "clear"
};

int (*bi_func[]) (char **) = {
    &bi_cd,
    &bi_help,
    &bi_exit,
    &bi_clear
};

int num_bi() {
    return sizeof(bi_list) / sizeof(char *);
}

int bi_cd(char** args) {
    int res;
    char* path;

    if (args[1] == NULL) {
        res = chdir(getenv("HOME"));
        if (res != 0) {
            perror("jsh");
        }
    } else {
        // Perform tilde expansion
        wordexp_t exp_res;
        wordexp(args[1], &exp_res, 0);
        path = exp_res.we_wordv[0];

        res = chdir(path); 
        if (res != 0) {
            perror("jsh");
        }

        wordfree(&exp_res);
    }
    return 1;
}

int bi_help(char** args) {
    printf("Jack Fuller's JSH\n");
    printf("Type program names and arguments, and hit enter,\n");
    printf("The following commands are built-ins:\n");

    for (int i = 0; i < num_bi(); i++) {
        printf("  %s\n", bi_list[i]);
    }

    printf("Use the \"man\" command to access the man pages for the other commands.\n");
    return 1;
}


int bi_exit(char** args) {
    exit(0);
}

int bi_clear(char** args) {
    printf("\033[2J\033[1;1H");
    return 1;
}

void check_malloc(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "jsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
}

void check_realloc(void* res, void* ptr) {
    if (ptr == NULL) {
        free(ptr);
        fprintf(stderr, "jsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
}

void enable_raw_mode(struct termios* orig_termios) {
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);
    *orig_termios = raw;

    raw.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(struct termios* orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}



void print_prompt() {
    struct passwd* pass;
    char* username;
    char hostname[64]; 
    
    pass = getpwuid(getuid());
    username = pass->pw_name;

    gethostname(hostname, 64);
    
    printf("%s@%s > ", username, hostname);
}

char* read_line() {
    int bufsize = RL_BUFSIZE;
    int position = 0;
    int len = 0;
    int c;
    struct termios orig_termios;
    
    char* buffer = malloc(sizeof(char) * bufsize);
    check_malloc(buffer);

    enable_raw_mode(&orig_termios);
    
    while (1) {
        c = getchar();   
        // CTRL-D, which is sent as 0x04 in raw mode.
        if (c == '\x04') {
            if (len == 0) {
                exit(EXIT_SUCCESS);
            }
            continue;
        } 
        
        // CTRL-L, which is sent as 0x0C in raw mode.
        if (c == '\x0C') {
            printf("\033[2J\033[1;1H");
            return NULL;
        }
        
        // Escaped characters, such as left and right arrows.
        if (c == '\x1B') {
            char seq[3];
            
            seq[0] = getchar();
            seq[1] = getchar();

            if (seq[0] == '[') {
                // Left arrow
                if (seq[1] == 'D') {
                    if (position > 0) {
                        printf("\b");
                        position -= 1;
                    }
                } else if (seq[1] == 'C') {
                    // Right arrow
                    if (position < len) {
                        printf("%c", buffer[position]);
                        position += 1;
                    }
                } else if (seq[1] == 'A') {
                    // Up arrow, print last command
                    for (int i = 0; i < len; i++) {
                        printf("\b \b");
                    }

                    memset(buffer, 0, bufsize);
                    memcpy(buffer, prev_line, strlen(prev_line));
                    
                    printf("%s", buffer);
                    len = strlen(prev_line);
                    position = strlen(prev_line);

                } else if (seq[1] == '3') {
                    seq[2] = getchar();
                    // DEL key was pressed
                    if (seq[2] == '~') {
                        if (position == len) {
                            continue;
                        }
                        len -= 1;
                        printf(" \b");

                        memmove(&buffer[position], &buffer[position + 1], len - position);

                        fwrite(&buffer[position], len - position, 1, stdout);
                        printf(" ");
                        for (int i = len + 1; i > position; i--) {
                            printf("\b");
                        }
                    }
                }
            }
            continue;
        }
        
        // Backspace, sent 
        if (c == '\x7F') {
            if (position == 0) {
                continue;
            }

            position -= 1;
            len -= 1;
            memmove(&buffer[position], &buffer[position + 1], len - position);
            printf("\b \b");
            
            fwrite(&buffer[position], len - position, 1, stdout);
            printf(" ");

            for (int i = len + 1; i > position; i--) {
                printf("\b");
            }
            continue;
        }

        if (c == '\n') {
            buffer[len] = '\0';
            printf("\n");
            return buffer;
        }

        if (position == len) {
            buffer[position] = c;
            printf("%c", c);
            position += 1;
            len += 1;

        } else {
            memmove(&buffer[position + 1], &buffer[position], len - position);
            buffer[position] = c;
            position += 1;
            len += 1;

            printf("\r");
            print_prompt();
            printf("%s", buffer);

            for (int i = len; i > position; i--) {
                printf("\b");
            }
        }

        if (len >= bufsize) {
            bufsize += RL_BUFSIZE;
            void* res = realloc(buffer, bufsize * sizeof(char));
            check_realloc(res, buffer);
            buffer = res;
        }

    }
    disable_raw_mode(&orig_termios);
}



void main_loop_new() {
    char* line;
    job* jobs;

    while (1) {
        print_prompt();
        line = read_line();
        
        // Clear the previous last command and save the new one.
        if (prev_line) {
            free(prev_line);
        }
        prev_line = strdup(line);

        jobs = parse_line(line);
        launch_job(jobs, IN_FOREGROUND);
        
        // Clean up memory
        free_job_list(jobs);
    }
}


int main(int argc, char* argv[]) {
   
    init();
    main_loop_new();

    return EXIT_SUCCESS;
}
