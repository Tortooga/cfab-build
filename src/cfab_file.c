#include "status.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>

void root_project_dir_path_buffer_destroy(char *buffer);

/* 
    Wraps getcwd to return appropriate StatusCode
*/
StatusCode get_root_project_dir_path(char **out_buffer, size_t *out_buffer_length)
{
    if (!out_buffer || !out_buffer_length)
    {
        return NULL_POINTER_PASSED;
    }

    StatusCode status = SUCCESS;
    *out_buffer = NULL;
    *out_buffer_length = 0;

    // Attempting to get the maximum absolute path length
    long max_path_length = pathconf("/", _PC_PATH_MAX);

    // pathconf reports error by returning negative value
    if (max_path_length < 0)
    {
        status = CFAB_FAILED_TO_GET_MAX_PATH_LENGTH;
        goto failure;
    }

    // Guaranteed to be >= 0
    *out_buffer_length = (size_t)max_path_length;

    // Extra byte for null terminator if needed
    *out_buffer = malloc(*out_buffer_length + 1);

    if (!(*out_buffer))
    {
        status = CFAB_HEAP_ALLOCATION_FAILED;
        goto failure;
    }

    /* getcwd already validates buffer_length is sufficient and inserts null terminator */
    char *ret = getcwd(*out_buffer, *out_buffer_length);

    if (!ret)
    {
        status = CFAB_FAILED_TO_GET_CWD;
        goto failure;
    }

    return SUCCESS;

    failure:
        root_project_dir_path_buffer_destroy(*out_buffer);
        *out_buffer = NULL;
        *out_buffer_length = 0;
        return status;
}

void root_project_dir_path_buffer_destroy(char *buffer)
{
    free(buffer);
}