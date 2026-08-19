#ifndef RULE_NAME_VALIDATOR_H
#define RULE_NAME_VALIDATOR_H

#include "parser.h"

#include <stdbool.h>

/*
    Returns SUCCESS if all the names are valid. invalid_rule will not be set.
    Returns NULL_POINTER_PASSED if one of the pointer args is NULL.
    Returns an error status code and sets invalid_rule if an invalid rule is encountered.
*/
StatusCode rules_names_are_valid(UnresolvedRule *rules, size_t rules_amount, UnresolvedRule **invalid_rule);
#endif