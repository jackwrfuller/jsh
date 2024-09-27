#ifndef PARSER_H
#define PARSER_H

#include "commands.h"

extern job_table* jt;

void print_table();
void build_table(job_table** job_table, char* input);

#endif
