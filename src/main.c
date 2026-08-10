#include "status.h"
#include "cfab_file.h"
#include "preprocessor.h"
#include "parser.h"
#include "resolver.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

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
    status = open_cfab_file(cwd_buffer, &fd);

    printf("Open cfab File Status: %d\n", status);

    if (status != SUCCESS)
    {
        return EXIT_FAILURE;
    }

    char *preprocessed_data;
    size_t preprocessed_data_size;
    
    status = preprocess_file(fd, &preprocessed_data, &preprocessed_data_size);

    printf("Preprocessor Status: %d\n", status);

    /*for (size_t i = 0; i < preprocessed_data_size; i++)
    {
        printf("%c", preprocessed_data[i]);
    }

    printf("\n");
    printf("\n");
    */
    Parser parser = {
        .data = preprocessed_data,
        .data_length = preprocessed_data_size,
        .cursor = 0,
    };

    UnresolvedRule *unresolved_rules;
    size_t amount;

    status = parse_unresolved_rules(&parser, &unresolved_rules, &amount);

    printf("Parser Rules Status: %d\n", status);

    if (status != SUCCESS)
    {
        goto cleanup;
    }
    /*for (size_t i = 0; i < amount; i++)
    {
        print_unresolved_rule(&unresolved_rules[i]);
        printf("\n");
    }
    */

    status = verify_path("../virtual-file-system-c");
    printf("Path Resolution Status %d\n", status);

    if (status != SUCCESS)
    {
        perror("stat");
        goto cleanup;
    }

    cleanup:
        destroy_preprocessed_data(preprocessed_data);
        close_cfab_file(fd);
        root_project_dir_path_buffer_destroy(cwd_buffer);

    return EXIT_SUCCESS;
}
