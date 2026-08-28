#include "job_analyzer.h"
#include "status.h"
#include "resolver.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

StatusCode scheduled_jobs_init(ResolvedRule *scheduled_rules, size_t scheduled_rules_amount, Job **out_jobs, size_t *out_jobs_amount)
{
    if (!out_jobs_amount)
    {
        return NULL_POINTER_PASSED;
    }

    *out_jobs_amount = 0;

    if (!scheduled_rules || !out_jobs)
    {
        return NULL_POINTER_PASSED;
    }

    if (scheduled_rules_amount == 0)
    {
        return SUCCESS;
    }

    *out_jobs = malloc(sizeof(Job) * scheduled_rules_amount);
    
    if (*out_jobs == NULL)
    {
        return CFAB_HEAP_ALLOCATION_FAILED;
    }

    *out_jobs_amount = scheduled_rules_amount; 

    for (size_t i = 0; i < *out_jobs_amount; i++)
    {
        (*out_jobs)[i].cmds = scheduled_rules[i].cmds;
        (*out_jobs)[i].cmds_amount = scheduled_rules[i].cmds_amount;
    }

    return SUCCESS;
}

void free_jobs(Job **jobs)
{
    if (!jobs)
    {
        return;
    }

    free(*jobs);
    *jobs = NULL;
}

/*static*/ StatusCode get_rule_staleness_compared_to_path_deps(ResolvedRule *rule, StalenessStatus *out_status)
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
        if (rule->deps[i].type == RULE_DEP)
        {
            continue;
        }

        stat_ret = stat(rule->deps[i].dep.path, &child_info);

        /*
            According to the resolver invariants, if a dependancy is labelled as a path dep instead of a rule dep,
            then there does not exist a rule that builds that dep. So if a path dep is not accessable,
            we cant just mark the parent as stale and move on.
        */
        if (stat_ret != 0)
        {
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


StatusCode mark_up_to_date_jobs(ResolvedRule *rules, size_t rules_amount)
{
    if (!rules)
    {
        return NULL_POINTER_PASSED;
    }

    if (rules_amount == 0)
    {
        return SUCCESS;
    }

    for (size_t i = 0; i < rules_amount; i++)
    {
        rules[i].is_up_to_date = false;
    }

    return IMPLEMENTATION_INCOMPLETE;
}

static StatusCode mark_up_to_date_jobs_recursive(ResolvedRule *cur_rule, size_t depth)
{
    if (depth >= UP_TO_DATE_JOBS_DETECTOR_RECUSRION_DEPTH_LIMIT)
    {
        return JOB_ANALYZER_MAX_RECURSION_LIMIT_REACHED;
    }

    for (size_t i = 0; i < cur_rule->deps_amount; i++)
    {
        if (cur_rule->deps[i].type == PATH_DEP)
        {
            // Should check time stamps
            continue;
        }

        // If a rule is up-to-date all of its children are up-to-date
        if (cur_rule->deps[i].dep.resolved_rule->is_up_to_date == true)
        {
            continue;
        }

        // Recursive Call
    }

    return IMPLEMENTATION_INCOMPLETE;
}