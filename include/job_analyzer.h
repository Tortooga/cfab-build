#ifndef JOB_ANALYZER_H
#define JOB_ANALYZER_H

#include "status.h"
#include "resolver.h"

#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>

#define UP_TO_DATE_JOBS_DETECTOR_RECUSRION_DEPTH_LIMIT 950

typedef struct 
{
    char **cmds;
    size_t cmds_amount;
    
    pid_t pid;

    bool up_to_date;
} Job;

typedef enum
{
    STALE,
    UP_TO_DATE
} StalenessStatus;

/*static*/ StatusCode get_rule_staleness_compared_to_path_deps(ResolvedRule *rule, StalenessStatus *out_status);

/*
    Rules must be schuled in the correct order
*/
StatusCode mark_stale_rules(ResolvedRule **schedule, size_t rules_amount);

StatusCode scheduled_jobs_init(ResolvedRule *scheduled_rules, size_t scheduled_rules_amount, Job **out_jobs, size_t *out_jobs_amount);
void free_jobs(Job **jobs);


#endif