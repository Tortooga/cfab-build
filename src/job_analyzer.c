#include "job_analyzer.h"
#include "status.h"
#include "resolver.h"

#include <stddef.h>
#include <stdlib.h>

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