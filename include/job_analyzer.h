#ifndef JOB_ANALYZER_H
#define JOB_ANALYZER_H

#include "status.h"
#include "resolver.h"

#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct 
{
    char **cmds;
    size_t cmds_amount;
    
    pid_t pid;

    bool up_to_date;
} Job;

StatusCode scheduled_jobs_init(ResolvedRule *scheduled_rules, size_t scheduled_rules_amount, Job **out_jobs, size_t *out_jobs_amount);
void free_jobs(Job **jobs);


#endif