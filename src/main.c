#include "status.h"
#include "cfab_file.h"
#include "preprocessor.h"
#include "parser.h"

#include <stdbool.h>
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

    char *preprocessed_data;
    size_t preprocessed_data_size;
    
    status = preprocess_file(fd, &preprocessed_data, &preprocessed_data_size);

    printf("Preprocessor Status: %d\n", status);

    for (size_t i = 0; i < preprocessed_data_size; i++)
    {
        printf("%c", preprocessed_data[i]);
    }

    printf("\n");
    printf("\n");

    Parser parser = {
        .data = preprocessed_data,
        .data_length = preprocessed_data_size,
        .cursor = 0,
    };

    UnresolvedRule unresolved_rule;
    bool no_deps_flag;
    
    status = parse_target_name(&parser, &unresolved_rule, &no_deps_flag);
    
    printf("Parse Target Name Status: %d\n", status);

    if (status != SUCCESS)
    {
        printf("Parser Error Code: %d\n", parser.error_type);
        if (parser.has_error_target)
        {
            printf("Parser Error Target: %s\n", parser.error_target_name);
        }
        goto cleanup;
    }


    printf("Target Name: %s\n", unresolved_rule.target_name);

    cleanup:
        destroy_preprocessed_data(preprocessed_data);
        close_cfab_file(fd);
        root_project_dir_path_buffer_destroy(cwd_buffer);

    return EXIT_SUCCESS;
}
