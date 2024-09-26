%union {
    char* string_value;
}
%token <string_value> WORD
%token SEMICOLON NEWLINE GREAT LESS AMP PIPE GREAT_AMP GREAT_GREAT GREAT_GREAT_AMP


%{
#include <stdlib.h>
#include <stdio.h>
#include "commands.h" 

extern int yylex(void);

void yyerror(char* string) {
    fprintf(stderr, "Syntax error: %s", string);
    exit(1);
}

int input_redirection_used = 0;  // Flag for '<' redirection
int output_redirection_used = 0; // Flag for '>', '>>', etc.

static job_table* jt = NULL;
static cmd_table* cur_cmd_table = NULL;
static simple_cmd* cur_cmd = NULL;




%}

%%
input: 
    job_list { 
        if (jt == NULL) {
            jt = create_job_table();
        }
    }
    | // Empty
    ;

job_list:
    before_job_action job_list_item end_job_action job_list
    | before_job_action job_list_item end_job_action
    ;

before_job_action:
    {
        printf("START JOB %d\n", jt->jobc);
        cur_cmd_table = &jt->jobs[jt->jobc];
        jt->jobc += 1;
    }
    ;

end_job_action:
    {
        
        printf("END JOB\n");
    }
    ;

job_list_item:

    job SEMICOLON 
    {  
        printf("\nSEMICOLON\n");
    }
    | job NEWLINE {  
        printf("\nNEWLINE\n");
    }
    ;

job:
    pipe_list io_modifier_list background_optional {
        input_redirection_used = 0;   // Reset flags after each job
        output_redirection_used = 0;
    }
    ;

before_cmd_action:
    {

    }
    ;

after_cmd_action:
    {

    }
    ;

pipe_list:
    cmd_and_args PIPE { printf("PIPE "); } pipe_list  
    | cmd_and_args
    ;

cmd_and_args:
    WORD { printf("WORD=%s ", $1); } arg_list 
    ;

arg_list:
    WORD { printf("WORD=%s ", $1); } arg_list 
    | // Empty
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
    AMP { printf("AMP "); }
    | // Empty 
    ;
%%

int main() {
    jt = create_job_table();
    yyparse();
    return 0;
}
