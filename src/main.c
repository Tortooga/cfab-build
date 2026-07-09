#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#include "status.h"

static StatusCode get_root_project_dir_path(char *buffer, size_t buffer_length);

int main(void)
{
    StatusCode status;

    char cwd[PATH_MAX]; 
     
    status = get_root_project_dir_path(cwd, PATH_MAX);

    //TODO: validate cwd

    printf("%s\n", cwd);
}

//TODO: Maybe Remove this function
/* 
    Wraps getcwd to return appropriate StatusCode
    path_buffer must be of length PATH_MAX at least
*/
static StatusCode get_root_project_dir_path(char *buffer, size_t buffer_length)
{
    /* getcwd already validates buffer_length is sufficient and inserts null terminator */
    char *ret = getcwd(buffer, buffer_length);

    if (!ret)
    {
        return CFAB_FAILED_TO_GET_CWD;
    }

    return SUCCESS;
}