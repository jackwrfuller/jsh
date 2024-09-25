
typedef struct __simple_command {
    int MAX;
    int argc;
    char** argv;
} simple_command;

void insert_arg(simple_command* cmd, char* arg);

typedef struct __command {
    int max_simple_cmd;
    int commmandc;
    simple_command** simple_cmds;
    char* outfile;
    char* infile;
    char* errfile;
    int foreground;
} command;

void insert_simple_cmd(command* cmd, simple_command* simple_cmd);

static command* current_cmd;
static simple_command* current_simple_cmd;


