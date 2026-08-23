#include "scheduler.h"
#include "status.h"
#include "resolver.h"

#include <stdlib.h>
#include <stddef.h>

static void mark_all_rules_as_incomplete(ResolvedRule *rules, size_t rules_amount);
static StatusCode schedule_rules_recursive(ResolvedRule *cur_rule, ResolvedRule **schedule, const size_t schedule_size, size_t *cur_schedule_index, size_t depth);
void free_schedule(ResolvedRule ***schedule);


StatusCode get_rules_schedule(ResolvedRule *target_rule, ResolvedRule *rules, size_t rules_amount, ResolvedRule ***out_schedule, size_t *out_schedule_length)
{
    
    if (!out_schedule)
    {
        return NULL_POINTER_PASSED;
    }
    
    *out_schedule = NULL;
    
    if (!out_schedule_length)
    {
        return NULL_POINTER_PASSED;
    }

    *out_schedule_length = 0;

    if (!target_rule || !rules)
    {
        return NULL_POINTER_PASSED;
    }
    
    if (rules_amount == 0)
    {
        return SUCCESS;
    }

    *out_schedule = malloc(sizeof(ResolvedRule *) * rules_amount);

    if (!*out_schedule)
    {
        return CFAB_HEAP_ALLOCATION_FAILED;
    }

    mark_all_rules_as_incomplete(rules, rules_amount);  

    size_t cur_schedule_index = 0;

    StatusCode status = schedule_rules_recursive(target_rule, *out_schedule, rules_amount, &cur_schedule_index, 0);

    if (status != SUCCESS)
    {
        goto failure;
    }
    
    *out_schedule_length = cur_schedule_index;

    return SUCCESS;

    failure:
        free_schedule(out_schedule);
        return status;
}

void free_schedule(ResolvedRule ***schedule)
{
    if (!schedule)
    {
        return;
    }

    free(*schedule);
    *schedule = NULL;
}

static StatusCode schedule_rules_recursive(ResolvedRule *cur_rule, ResolvedRule **schedule, const size_t schedule_size, size_t *cur_schedule_index, size_t depth)
{
    if (depth >= MAX_SCHEDULER_RECURSION_DEPTH)
    {
        return SCHEDULER_MAX_RECURSION_DEPTH_REACHED;
    }

    if (*cur_schedule_index >= schedule_size)
    {
        return SCHEDULER_SCHEDULE_INDEX_OUT_OF_BOUNDS;
    }

    StatusCode status;

    for (size_t i = 0; i < cur_rule->deps_amount; i++)
    {
        // Path dependancies do not get built
        if (cur_rule->deps[i].type == PATH_DEP)
        {
            continue;
        }

        if (cur_rule->deps[i].dep.resolved_rule->rule_status == COMPLETE_RULE)
        {
            continue;
        }

        status = schedule_rules_recursive(cur_rule->deps[i].dep.resolved_rule, schedule, schedule_size, cur_schedule_index, depth + 1);

        if (status != SUCCESS)
        {
            return status;
        }
    }


    schedule[*cur_schedule_index] = cur_rule;
    (*cur_schedule_index)++;

    cur_rule->rule_status = COMPLETE_RULE;

    return SUCCESS;
}

static void mark_all_rules_as_incomplete(ResolvedRule *rules, size_t rules_amount)
{
    for (size_t i = 0; i < rules_amount; i++)
    {
        rules[i].rule_status = INCOMPLETE_RULE;
    }
}