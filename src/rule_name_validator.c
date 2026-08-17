#include "rule_name_validator.h"
#include "parser.h"

#include <stdbool.h>
#include <string.h>


/*
    Returns true if the target names in each rule is unique. collision_name will not be set.
    Returns false if a collision is found. collision_name will be set.
*/
/*static*/ bool rule_names_are_unique(UnresolvedRule *rules, size_t rules_amount, char **collision_name)
{
    *collision_name = NULL;
    
    for (size_t target_rule_index = 0; target_rule_index < rules_amount; target_rule_index++)
    {
        for (size_t i = target_rule_index + 1; i < rules_amount; i++)
        {
            // rule.target_name guaranteed to be NULL terminated by parser.
            if (strcmp(rules[target_rule_index].target_name, rules[i].target_name) == 0)
            {
                *collision_name = rules[target_rule_index].target_name;
                return false;
            }
        }
    }

    return true;
}