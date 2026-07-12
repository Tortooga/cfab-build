#include "status.h"
#include "cfab_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


int main(void)
{
    StatusCode status;

    char *cwd_buffer;
    size_t cwd_buffer_length;

    status = get_root_project_dir_path(&cwd_buffer, &cwd_buffer_length);

    if (status != SUCCESS)
    {
        printf("%d\n", status);
        return EXIT_FAILURE;
    }

    printf("%s\n", cwd_buffer);

    root_project_dir_path_buffer_destroy(cwd_buffer);

    return EXIT_SUCCESS;
}
 
