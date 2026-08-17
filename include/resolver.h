#ifndef RESOLVER_H
#define RESOLVER_H

#include "status.h" 
#include "parser.h"

#include <stddef.h>


// A dependancy name is valid if it corsponds to a target name for another rule,
// or it represents a valid relative path to an existant file/folder

typedef enum 
{
    PATH_DEP,
    RULE_DEP
} DepType;

typedef struct ResolvedRule ResolvedRule;

// path must be null terminated.
typedef struct
{
    union 
    {
        ResolvedRule *resolved_rule;
        const char *path;
    } dep;

    DepType type;
} ResolvedDep;

typedef struct ResolvedRule
{
    const char *target_name;

    ResolvedDep *deps;
    size_t deps_amount;

    char **cmds;
    size_t cmds_amount;

    UnresolvedRule *unresolved_rule;
} ResolvedRule;

typedef struct 
{
    bool has_error_rule;
    UnresolvedRule *error_rule;

    bool has_error_dep;
    size_t error_dep_index;
} ErrorObjectsReporter;

StatusCode resolved_rules_get(ResolvedRule **out_resolved_rules, size_t *out_resolved_rules_amount, UnresolvedRule *unresolved_rules, const size_t unresolved_rules_amount, ErrorObjectsReporter *error_object_reporter);
void resolved_rules_free(ResolvedRule *resolved_rules, size_t resolved_rules_amount);

#endif