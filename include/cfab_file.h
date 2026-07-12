#ifndef CFAB_FILE_H
#define CFAB_FILE_H

#include "status.h"

#include <stddef.h>

/*
    Returns the path to the current working directory of the procces.
*/
StatusCode get_root_project_dir_path(char **out_buffer, size_t *out_buffer_length);


/*
    Destroys the root project path buffer.
*/
void root_project_dir_path_buffer_destroy(char *buffer);


#endif