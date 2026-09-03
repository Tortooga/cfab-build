#include "job_analyzer.h"
#include "status.h"
#include "resolver.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

/*
    Rules must be scheduled and their staleness status must be marked.
*/
    StatusCode scheduled_jobs_init(ResolvedRule **scheduled_rules, size_t scheduled_rules_amount, Job **out_jobs, size_t *out_jobs_amount)
    {
        if (!out_jobs_amount)
        {
            return NULL_POINTER_PASSED;
        }

        *out_jobs_amount = 0;

        if (!out_jobs)
        {
            return NULL_POINTER_PASSED;
        }

        *out_jobs = NULL;

        if (!scheduled_rules)
        {
            return NULL_POINTER_PASSED;
        }

        if (scheduled_rules_amount == 0)
        {
            return SUCCESS;
        }

        for (size_t i = 0; i < scheduled_rules_amount; i++)
        {
            if (scheduled_rules[i]->is_stale)
            {
                (*out_jobs_amount)++;
            }
        }

        if (*out_jobs_amount == 0)
        {
            return SUCCESS;
        }

        *out_jobs = malloc(sizeof(Job) * (*out_jobs_amount));
        
        if (*out_jobs == NULL)
        {
            *out_jobs_amount = 0;
            return CFAB_HEAP_ALLOCATION_FAILED;
        }

        // NULLing pids so free on failure doesnt crash
        for (size_t i = 0; i < *out_jobs_amount; i++)
        {
            (*out_jobs)[i].pids = NULL;
            (*out_jobs)[i].rule = NULL;
            (*out_jobs)[i].pids_amount = 0;
        }

        StatusCode status = SUCCESS;
        size_t cur_job_index = 0;
        for (size_t i = 0; i < scheduled_rules_amount; i++)
        {
            if (cur_job_index >= *out_jobs_amount)
            {
                // This guarantees we have stored all the stale rules
                break;
            }

            if (scheduled_rules[i]->is_stale)
            {
                (*out_jobs)[cur_job_index].rule = scheduled_rules[i];

                if ((*out_jobs)[cur_job_index].rule->cmds_amount == 0)
                {
                    (*out_jobs)[cur_job_index].pids_amount = 0;
                    continue;
                }

                (*out_jobs)[cur_job_index].pids = malloc(sizeof(pid_t) * scheduled_rules[i]->cmds_amount);

                if (!(*out_jobs)[cur_job_index].pids)
                {
                    status = CFAB_HEAP_ALLOCATION_FAILED;
                    goto failure;
                }

                (*out_jobs)[cur_job_index].pids_amount = scheduled_rules[i]->cmds_amount;

                cur_job_index++;
            }
        }

        return SUCCESS;

        failure:
            free_jobs(out_jobs, *out_jobs_amount);
            return status;
    }

    void free_jobs(Job **jobs, size_t jobs_amount)
    {
        if (!jobs)
        {
            return;
        }

        for (size_t i = 0; i < jobs_amount; i++)
        {
            free((*jobs)[i].pids);
        }

        free(*jobs);
        *jobs = NULL;
    }

/*static*/ StatusCode get_rule_staleness_compared_to_deps(ResolvedRule *rule, StalenessStatus *out_status)
{
    struct stat rule_info;

    int stat_ret = stat(rule->target_name, &rule_info);

    if (stat_ret != 0)
    {   
        // If a component of the target path doesnt exist
        // We mark the rule as stale so it can be built
        if (errno == ENOENT)
        {
            *out_status = STALE;
            return SUCCESS;
        }

        return JOB_ANALYZER_FAILED_TO_GET_TARGET_STATUS;
    }

    struct stat child_info;
    for (size_t i = 0; i < rule->deps_amount; i++)
    {
        if (rule->deps[i].type == PATH_DEP)
        {
            stat_ret = stat(rule->deps[i].dep.path, &child_info);
        }
        else
        {
            stat_ret = stat(rule->deps[i].dep.resolved_rule->target_name, &child_info);
        }

        /*
            According to the resolver invariants, if a dependancy is labelled as a path dep instead of a rule dep,
            then there does not exist a rule that builds that dep. So if a path dep is not accessable,
            we cant just mark the parent as stale and move on.
        */
        if (stat_ret != 0)
        {
            if (rule->deps[i].type == RULE_DEP)
            {
                *out_status = STALE;
                return SUCCESS;
            }

            return JOB_ANALYZER_FAILED_TO_ACCESS_PATH_DEPENDANCY;
        }

        // If the last time the child was modified is greater than the last time the rule was modified in then the rule is stale.
        if (child_info.st_mtime >= rule_info.st_mtime)
        {
            *out_status = STALE;
            return SUCCESS;
        }
    }

    *out_status = UP_TO_DATE;
    return SUCCESS;
}

/*
    Rules must be schuled in the correct order
*/
StatusCode mark_stale_rules(ResolvedRule **schedule, size_t schedule_length)
{
    if (!schedule)
    {
        return NULL_POINTER_PASSED;
    }

    if (schedule_length == 0)
    {
        return SUCCESS;
    }

    StatusCode status;
    StalenessStatus cur_rule_staleness_status;

    for (size_t i = 0; i < schedule_length; i++)
    {
        status = get_rule_staleness_compared_to_deps(schedule[i], &cur_rule_staleness_status);

        if (status != SUCCESS)
        {
            return status;
        }

        if (cur_rule_staleness_status == STALE)
        {
            schedule[i]->is_stale = true;
            continue;
        }

        schedule[i]->is_stale = false;
    }

    // Scheduled rules cannot precede their dependancies
    for (size_t i = 0; i < schedule_length; i++)
    {
        for (size_t dep_index = 0; dep_index < schedule[i]->deps_amount; dep_index++)
        {
            if (schedule[i]->deps[dep_index].type == PATH_DEP)
            {
                continue;
            }

            if (schedule[i]->deps[dep_index].dep.resolved_rule->is_stale)
            {
                schedule[i]->is_stale = true;
                break;
            }
        }
    }

    return SUCCESS;
}