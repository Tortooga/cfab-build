#include "rule_name_validator.h"
#include "parser.h"

#include <stdbool.h>
#include <string.h>

// Checks if rule names are unique, non empty and consist of valid chars


/*
    Returns true if the target names in each rule is unique. collision_rule will not be set.
    Returns false if a collision is found. collision_rule will be set.
*/
/*static*/ bool rule_target_names_are_unique(UnresolvedRule *rules, size_t rules_amount, UnresolvedRule *collision_rule)
{
    for (size_t target_rule_index = 0; target_rule_index < rules_amount; target_rule_index++)
    {
        for (size_t i = target_rule_index + 1; i < rules_amount; i++)
        {
            // rule.target_name guaranteed to be NULL terminated by parser.
            if (strcmp(rules[target_rule_index].target_name, rules[i].target_name) == 0)
            {
                *collision_rule = rules[target_rule_index];
                return false;
            }
        }
    }

    return true;
}

/*
    Returns true if the target name and all the dep names are non empty
    Returns false if either the target name or one of the dep names is empty. 
    In that case check the out param has_non_empty_target_name to determine which domain contains empty name.
*/
/*static*/ bool rule_names_are_non_empty(UnresolvedRule *rule, bool *has_non_empty_target_name)
{
    *has_non_empty_target_name = true;

    // rule.target_name guaranteed to be NULL terminated by parser.
    // so checking whether the first element is '\0' is equivilent to checking whether it is empty
    if (rule->target_name[0] == '\0')
    {
        *has_non_empty_target_name = false;
        return false;
    }

    for (size_t i = 0; i < rule->deps_amount; i++)
    {
        // rule.deps[i] guaranteed to be NULL terminated by parser.
        if ((rule->deps[i])[0] == '\0')
        {
            return false;
        }
    }

    return true;
}

StatusCode rule_names_are_valid(UnresolvedRule *rules, size_t rules_amount, UnresolvedRule *invalid_rule)
{
    if (!rules || !invalid_rule)
    {
        return NULL_POINTER_PASSED;
    }

    if (!rule_target_names_are_unique(rules, rules_amount, invalid_rule))
    {
        return VALIDATOR_TARGET_NAME_COLLISION;
    }

    size_t cur_name_length = 0;
    bool cur_rule_has_non_empty_target_name;
    for (size_t i = 0; i < rules_amount; i++)
    {
        if (!rule_names_are_non_empty(rules + i, &cur_rule_has_non_empty_target_name))
        {
            *invalid_rule = rules[i];

            if (cur_rule_has_non_empty_target_name)
            {
                return VALIDATOR_RULE_CONTAINS_EMPTY_DEP;
            }

            return VALIDATOR_TARGET_NAME_IS_EMPTY;
        }
    }

    return IMPLEMENTATION_INCOMPLETE;
}