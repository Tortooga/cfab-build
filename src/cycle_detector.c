#include "cycle_detector.h"
#include "resolver.h"

#include <stdbool.h>

static void clear_rules_statuses(ResolvedRule *rules, size_t rules_amount);
static StatusCode verify_acyclic_recursive(ResolvedRule *cur_rule, size_t depth);

/*
    allocate a bool list each index corsponding to the "explored" status of each rule 
*/

StatusCode verify_acyclic(ResolvedRule *rules, size_t rules_amount)
{
    if (!rules)
    {
        return NULL_POINTER_PASSED;
    }

    if (rules_amount == 0)
    {
        return SUCCESS;
    }

    clear_rules_statuses(rules, rules_amount);

    StatusCode status = SUCCESS;

    for (size_t i = 0; i < rules_amount; i++)
    {
        if (rules[i].rule_status == COMPLETE_RULE)
        {
            continue;
        }

        // should be impossible
        if (rules[i].rule_status == PENDING_RULE)
        {
            return CYCLE_DETECTOR_UNEXPECTED_PENDING_RULE_ENCOUNTERED;
        }

        status = verify_acyclic_recursive(rules + i, 0);

        if (status != SUCCESS)
        {
            return status;
        }
    }
    
    return SUCCESS;
}

static StatusCode verify_acyclic_recursive(ResolvedRule *cur_rule, size_t depth)
{
    if (depth >= MAX_RECURSION_DEPTH)
    {
        return CYCLE_DETECTOR_MAX_RECURSION_DEPTH_REACHED;
    }

    StatusCode status;

    cur_rule->rule_status = PENDING_RULE;
    for (size_t i = 0; i < cur_rule->deps_amount; i++)
    {
        if (cur_rule->deps[i].type != RULE_DEP)
        {
            continue;
        }
        
        
        // Encountering a pending rule implies this current path is a cycle
        if (cur_rule->deps[i].dep.resolved_rule->rule_status == PENDING_RULE)
        {
            return CYCLE_DETECTOR_CYCLE_ENCOUNTERED;
        }
        
        // Path already explored
        if (cur_rule->deps[i].dep.resolved_rule->rule_status == COMPLETE_RULE)
        {
            continue;
        }
        
        status = verify_acyclic_recursive(cur_rule->deps[i].dep.resolved_rule, depth + 1);
        
        if (status != SUCCESS)
        {
            return status;
        }
        
        // We have just finished traversing cur_rule->deps[i].dep.resolved_rule
        cur_rule->deps[i].dep.resolved_rule->rule_status = COMPLETE_RULE;    
    }
    
    cur_rule->rule_status = COMPLETE_RULE;
    
    return SUCCESS;
}

static void clear_rules_statuses(ResolvedRule *rules, size_t rules_amount)
{
    for (size_t i = 0; i < rules_amount; i++)
    {
        rules[i].rule_status = INCOMPLETE_RULE;
    }
}