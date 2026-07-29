#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stddef.h>
#include <stdio.h>

#include "status.h"
#define MAX_DEP_SIZE 

#define START_COMMENT_OPERATOR '#'
#define END_COMMENT_OPERATOR '\n'

#define CMDS_BLOCK_START_OPERATOR '{'
#define CMDS_BLOCK_END_OPERATOR '}'
#define CMD_LINE_END_OPERATOR ';'

static const char INSIGNIFICANT_CHARS[] = {' ', '\n'}; 
static const size_t INSIGNIFICANT_CHARS_COUNT = sizeof(INSIGNIFICANT_CHARS);

StatusCode preprocess_file(FILE *file, char **out_preprocessed_data, size_t *out_preprocessed_data_size);
void destroy_preprocessed_data(char *buffer);

#endif