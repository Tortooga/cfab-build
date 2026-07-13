#include "status.h"
#include "cfab_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h> // dont forget to remove


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

    FILE *fd;
    open_cfab_file(cwd_buffer, &fd);

    int byte;

    while (1)
    {
        byte = fgetc(fd);

        if (byte == EOF)
        {
            break;
        }

        printf("%c", byte);
    }

    printf("\n");

    status = close_cfab_file(fd);
    root_project_dir_path_buffer_destroy(cwd_buffer);

    return EXIT_SUCCESS;
}
 
