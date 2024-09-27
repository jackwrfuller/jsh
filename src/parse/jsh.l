%option noyywrap

%{
#include <string.h>
#include <stdio.h>
#include "jsh.tab.h"
%}

%option header="lexer.h"

%%
";"                           { return SEMICOLON;         }
\n                            { return NEWLINE;           }
[ \t]                         {;                          }
">"                           { return GREAT;             }
"<"                           { return LESS;              }
">>"                          { return GREAT_GREAT;       }
">>&"                         { return GREAT_GREAT_AMP;   }
">&"                          { return GREAT_AMP;         }
"&"                           { return AMP;               }
"|"                           { return PIPE;              }
[^ \t\n;|&><][^ \t\n;|&><]*   { yylval.string_value = strdup(yytext); return WORD; }
%%
