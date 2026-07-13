#include "status.h"
#include "cfab_file.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

static void get_cfab_file_path(const char *parent_dir_path, const size_t parent_dir_path_length, const size_t cfab_file_path_length, char *out_cfab_file_path);

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

/*
    parent_dir_path must be a C string. We don't take a length argument as the parent_dir_buffer is usually larger than the actual path.
    So it is more memory effecient to count the length using strlen
    parent_dir_path must not have a trailing '/'
*/
StatusCode open_cfab_file(const char *parent_dir_path, FILE **out_fd)
{
    if (!parent_dir_path || !out_fd)
    {
        return NULL_POINTER_PASSED;
    }

    const size_t parent_dir_path_length = strlen(parent_dir_path); 

    // +2 to accommodate for null terminator and extra path delimiter '/'
    const size_t cfab_file_path_length  = parent_dir_path_length + CFAB_FILE_NAME_LENGTH + 2;

    char cfab_file_path[cfab_file_path_length];

    get_cfab_file_path(parent_dir_path, parent_dir_path_length, cfab_file_path_length, cfab_file_path);

    *out_fd = fopen(cfab_file_path, "r");

    if (!(*out_fd))
    {
        if (errno == ENOENT)
        {
            printf("hello\n");
            return CFAB_FILE_DOESNT_EXIST;
        }

        return CFAB_FAILED_TO_OPEN_CFAB_FILE;
    }

    return SUCCESS;
}

// parent_dir_path must be a C string
static void get_cfab_file_path(const char *parent_dir_path, const size_t parent_dir_path_length, const size_t cfab_file_path_length, char *out_cfab_file_path)
{
    strcpy(out_cfab_file_path, parent_dir_path);

    // Inserting path delimiter directly after the end of the parent dir path
    out_cfab_file_path[parent_dir_path_length] = '/';

    memcpy(out_cfab_file_path + parent_dir_path_length + 1, CFAB_FILE_NAME, CFAB_FILE_NAME_LENGTH);

    out_cfab_file_path[cfab_file_path_length - 1] = '\0';
}

StatusCode close_cfab_file(FILE *file)
{
    if (!file)
    {
        return NULL_POINTER_PASSED;
    }

    int ret = fclose(file);

    if (ret != 0)
    {
        return CFAB_FAILED_TO_CLOSE_FILE;
    }
    
    return SUCCESS;
}