#ifndef CFAB_FILE_H
#define CFAB_FILE_H

#include "status.h"

#include <stddef.h>
#include <stdio.h>

#define CFAB_FILE_NAME "cfabfile"

// -1 to not count null terminator
#define CFAB_FILE_NAME_LENGTH (sizeof(CFAB_FILE_NAME) - 1)

/*
    Returns the path to the current working directory of the procces.
*/
StatusCode get_root_project_dir_path(char **out_buffer, size_t *out_buffer_length);


/*
    Destroys the root project path buffer.
*/
void root_project_dir_path_buffer_destroy(char *buffer);

/*
    Opens cfab file on read mode.
    parent_dir_path must be a C-string containing the path to the parent directory of the cfab file.
    The path must not terminate with a '/'.
*/
StatusCode open_cfab_file(const char *parent_dir_path, FILE **out_fd);

/*
    Closes cfab File
*/
StatusCode close_cfab_file(FILE *fd);

#endif