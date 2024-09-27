%union {
    char* string_value;
}
%token <string_value> WORD
%token SEMICOLON NEWLINE GREAT LESS AMP PIPE GREAT_AMP GREAT_GREAT GREAT_GREAT_AMP


%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commands.h" 

extern int yylex(void);

void yyerror(char* string) {
    fprintf(stderr, "Syntax error: %s", string);
    exit(1);
}

int input_redirection_used = 0;  // Flag for '<' redirection
int output_redirection_used = 0; // Flag for '>', '>>', etc.

static job_table* jt = NULL;
static proc_table* cur_proc_table = NULL;
static proc* cur_proc = NULL;
static char* cur_arg = NULL;

void print_index() {
    printf("(%i, %i, %i)", jt->jobc, cur_proc_table->procc, cur_proc->argc);
}

void print_word() {
    printf(" %s ", cur_proc->argv[cur_proc->argc]);
}

void print_table() {
    printf("\nPRINTING RESULTING TABLE:\n");
    proc_table* pt;
    for (int i = 0; i < jt->jobc; i++) {
        pt = &jt->jobs[i];

        proc* p;
        for (int j = 0; j < pt->procc; j++) {
            p = &pt->procs[j];

            char* arg;
            for (int k = 0; k < p->argc; k++) {
                arg = p->argv[k];
                printf("%s, ", arg);
            }
            printf(" | ");
        }
        printf("\n");
    }
}

%}

%%
input: 
    before_input job_list { print_table(); }
    | // Empty
    ;

before_input:
    {
        jt = create_job_table();
    }
    ;

job_list:
    bja job_list_item afa job_list
    | bja job_list_item afa
    ;

bja:
    {
        printf("\nSTART JOB %d\n", jt->jobc);
        cur_proc_table = &jt->jobs[jt->jobc];
    }
    ;

afa:
    {
        jt->jobc += 1;
        printf("END JOB\n");
        printf("Table at this point:\n");
        print_table();
    }
    ;

job_list_item:

    job SEMICOLON { printf("\nSEMICOLON\n"); }
    | job NEWLINE { printf("\nNEWLINE\n");   }
    ;

job:
    pipe_list io_modifier_list background_optional {
        input_redirection_used = 0;   // Reset flags after each job
        output_redirection_used = 0;
    }
    ;

pipe_list:
    bpa proc apa PIPE { printf("PIPE "); } pipe_list  
    | bpa proc apa
    ;

bpa:
    {
        printf("\nSTART PROC %d\n", cur_proc_table->procc);
        cur_proc = &cur_proc_table->procs[cur_proc_table->procc];
    }
    ;

apa:
    {
        cur_proc_table->procc += 1;
        printf("\nEND PROC\n");
    }
    ;



proc:
    baa WORD { insert(jt, jt->jobc, cur_proc_table->procc, cur_proc->argc, $2); } aaa arg_list 
    ;

arg_list:
    baa WORD { insert(jt, jt->jobc, cur_proc_table->procc, cur_proc->argc, $2); } aaa arg_list 
    | // Empty
    ;

baa:
    {
        //cur_arg = cur_proc->argv[cur_proc->argc];
    }
    ;

aaa:
    {
        print_index();
        print_word();
        cur_proc->argc += 1;
    }
    ;

io_modifier_list:
    io_modifier_list io_modifier
    | // Empty
    ;


io_modifier:
    GREAT { 
        if (output_redirection_used) {
            yyerror("Multiple output redirections not allowed");
            YYERROR;
        } else {
            output_redirection_used = 1;
            printf("> ");
        }
    } WORD { printf("WORD=%s ", $3); }              
    | GREAT_AMP { 
        if (output_redirection_used) {
            yyerror("Multiple output redirections not allowed");
            YYERROR;
        } else {
            output_redirection_used = 1;
            printf(">& ");
        }
    } WORD { printf("WORD=%s ", $3); }
    | GREAT_GREAT { 
        if (output_redirection_used) {
            yyerror("Multiple output redirections not allowed");
            YYERROR;
        } else {
            output_redirection_used = 1;
            printf(">> ");
        }
    } WORD { printf("WORD=%s ", $3); }
    | GREAT_GREAT_AMP { 
        if (output_redirection_used) {
            yyerror("Multiple output redirections not allowed");
            YYERROR;
        } else {
            output_redirection_used = 1;
            printf(">>& ");
        }
    } WORD { printf("WORD=%s ", $3); }
    | LESS { 
        if (input_redirection_used) {
            yyerror("Multiple input redirections not allowed");
            YYERROR;
        } else {
            input_redirection_used = 1;
            printf("< ");
        }
    } WORD { printf("WORD=%s ", $3); } 
    ;


background_optional:
    AMP 
    { 
        printf("AMP ");
        cur_proc_table->foreground = 0; 
    }
    | // Empty
    {
        cur_proc_table->foreground = 1;
    }
    ;
%%



int main() {
    yyparse();
    return 0;
}
