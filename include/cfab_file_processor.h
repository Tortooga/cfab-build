#ifndef CFAB_FILE_PROCESSOR_H
#define CFAB_FILE_PROCESSOR_H

#include <stddef.h>

#define MAX_DEP_SIZE 

#define START_COMMENT_OPERATOR '#'
#define END_COMMENT_OPERATOR '\n'

typedef struct 
{
    char *target;

    char **deps;
    size_t deps_count;

    char **cmds;
    size_t cmds_count;
} TokenizedRule;

static const char INSIGNIFICANT_CHARS[] = {' ', '\n'}; 
static const size_t INSIGNIFICANT_CHARS_COUNT = sizeof(INSIGNIFICANT_CHARS);

#endif