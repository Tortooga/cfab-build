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

    printf("Parse Rules Status: %d\n", status);

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

    ResolvedRule *resolved_rules;
    size_t resolved_rules_amount;
    ErrorObjectsReporter err;

    status = resolved_rules_get(&resolved_rules, &resolved_rules_amount, unresolved_rules, amount, &err);

    printf("Resolved Rules Get Status: %d\n", status);

    printf("\n\n");
    if (status != SUCCESS)
    {
        if (err.has_error_rule)
        {
            printf("Error Rule: %s\n", err.error_rule->target_name);
        }
        if (err.has_error_dep)
        {
            printf("Error Dep: %s\n", err.error_rule->deps[err.error_dep_index]);
        }

        goto cleanup;
    }

    for (size_t i = 0; i < resolved_rules_amount; i++)
    {
        printf("%s\n", resolved_rules[i].target_name);
        for (size_t j = 0; j < resolved_rules[i].deps_amount; j++)
        {
            if (resolved_rules[i].deps[j].type == PATH_DEP)
            {
                printf("    Path Dep: %s\n", resolved_rules[i].deps[j].dep.path);
                continue;
            }
            
            printf("    Rule Dep: %s\n", resolved_rules[i].deps[j].dep.resolved_rule->target_name);
        }
    }

    cleanup:
        destroy_preprocessed_data(preprocessed_data);
        close_cfab_file(fd);
        root_project_dir_path_buffer_destroy(cwd_buffer);

    return EXIT_SUCCESS;
}
