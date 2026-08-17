#ifndef RULE_NAME_VALIDATOR_H
#define RULE_NAME_VALIDATOR_H

#include "parser.h"

#include <stdbool.h>

/*static*/ bool rule_names_are_unique(UnresolvedRule *rules, size_t rules_amount, char **collision_name);

#endif