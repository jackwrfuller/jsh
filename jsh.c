#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>
#include <termios.h>


#define RL_BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

/* Forward declarations */
int bi_cd(char** args);
int bi_help(char** args);
int bi_exit(char** args);
int bi_clear(char** args);

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
    return 0;
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
    char username[64];
    char hostname[64]; 
    
    getlogin_r(username, 64);
    gethostname(hostname, 64);
    
    printf("%s@%s > ", username, hostname);
}

char* read_line() {
    int bufsize = RL_BUFSIZE;
    int position = 0;
    int len = 0;
    char* buffer = malloc(sizeof(char) * bufsize);
    int c;
    struct termios orig_termios;
    check_malloc(buffer);

    while (1) {

        enable_raw_mode(&orig_termios);
        
        c = getchar();

        if (c == '\x1B') {
            char seq[3];
            /*if (read(STDIN_FILENO, &seq[0], 1) == 0) {*/
            /*    continue;*/
            /*}*/
            /**/
            /*if (read(STDIN_FILENO, &seq[1], 1) == 0) {*/
            /*    continue;*/
            /*}*/
            
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
                }
            }
            continue;
        }

        if (c == '\x7F' && position > 0) {
            position -= 1;
            len -= 1;
            memmove(&buffer[position], &buffer[position + 1], len - position);
            printf("\b \b");
            
            printf("%s ", &buffer[position]);

            for (int i = len + 1; i > position; i--) {
                printf("\b");
            }
            continue;
        }


        if (c == EOF) {
            if (position == 0) {
                exit(EXIT_SUCCESS);
            }
            continue;
        } 

        if (c == '\n') {
            buffer[position] = '\0';
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
            buffer = realloc(buffer, bufsize * sizeof(char));
            check_malloc(buffer);
        }

        disable_raw_mode(&orig_termios);
    }
}

char** split_line(char* line) {
    int bufsize = TOK_BUFSIZE;
    int position = 0;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;

    check_malloc(tokens);

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

int launch(char** args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("jsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("jsh");
    } else {
        // Parent
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int execute(char** args) {
    if (args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < num_bi(); i++) {
        if (strcmp(args[0], bi_list[i]) == 0) {
            return (*bi_func[i])(args);
        }
    }

    return launch(args);
}





void main_loop() {
    char* line;
    char** args;
    int status;


    do {
        //printf("\n");
        print_prompt();
        line = read_line();
        printf("Input: %s\n", line);
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    } while (status);

}


int main(int argc, char* argv[]) {
    
    main_loop();

    return EXIT_SUCCESS;
}
