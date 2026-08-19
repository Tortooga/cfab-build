#ifndef RULE_NAME_VALIDATOR_H
#define RULE_NAME_VALIDATOR_H

#include "parser.h"

#include <stdbool.h>

/*static*/ bool rule_target_names_are_unique(UnresolvedRule *rules, size_t rules_amount, UnresolvedRule *collision_rule);
/*static*/ bool rule_names_are_non_empty(UnresolvedRule *rule, bool *has_non_empty_target_name);

#endif