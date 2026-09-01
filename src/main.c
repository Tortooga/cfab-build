#include "status.h"
#include "cfab_file.h"
#include "preprocessor.h"
#include "parser.h"
#include "resolver.h"
#include "rule_name_validator.h"
#include "cycle_detector.h"
#include "scheduler.h"
#include "job_analyzer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>


/*
    WARNING: THIS IS A TESTING PLAYGROUND. SOME BUGS ARE LEFT INTENTIONALLY.
*/


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

    UnresolvedRule *err_rule;

    status = rules_names_are_valid(unresolved_rules, amount, &err_rule);

    printf("Rule Name Validator Status: %d\n", status);

    if (status != SUCCESS)
    {
        printf("Invalid Rule: %s\n", err_rule->target_name);
        goto cleanup;
    }

    ResolvedRule *resolved_rules;
    size_t resolved_rules_amount;
    ErrorObjectsReporter err;

    status = resolved_rules_get(&resolved_rules, &resolved_rules_amount, unresolved_rules, amount, &err);

    printf("Resolved Rules Get Status: %d\n", status);

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

    status = verify_acyclic(resolved_rules, resolved_rules_amount);

    printf("Cycle Detection Status: %d\n", status);

    if (status != SUCCESS)
    {
        for (size_t i = 0; i < resolved_rules_amount; i++)
        {
            if (resolved_rules[i].rule_status == PENDING_RULE)
            {
                printf("%s\n", resolved_rules[i].target_name);
            }
        }

        goto cleanup;
    }

    ResolvedRule **schedule;
    size_t schedule_length;

    status = get_rules_schedule(resolved_rules, resolved_rules, resolved_rules_amount, &schedule, &schedule_length);

    printf("Scheduler Status: %d\n", status);

    if (status != SUCCESS)
    {
        goto cleanup;
    }

    for (size_t i = 0; i < schedule_length; i++)
    {
        for (size_t c = 0; c < schedule[i]->cmds_amount; c++)
        {
            printf("%s\n", schedule[i]->cmds[c]);
            system(schedule[i]->cmds[c]);
        }
    }

    Job *jobs;
    size_t jobs_amount;

    status = scheduled_jobs_init(*schedule, schedule_length, &jobs, &jobs_amount);

    printf("Job Initialiser Status: %d\n", status);

    if (status != SUCCESS)
    {
        goto cleanup;
    }

    printf("Jobs Amount: %zu\n", jobs_amount);

    status = mark_stale_rules(schedule, schedule_length);

    if (status != SUCCESS)
    {
        goto cleanup;
    }

    for (size_t i = 0; i < schedule_length; i++)
    {
        printf("%s: %d\n", schedule[i]->target_name, schedule[i]->is_stale);
    }

    cleanup:
        destroy_preprocessed_data(preprocessed_data);
        close_cfab_file(fd);
        root_project_dir_path_buffer_destroy(cwd_buffer);

    return EXIT_SUCCESS;
}
