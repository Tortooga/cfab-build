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


/*static*/ StatusCode verify_path(const char *path);
/*static*/ StatusCode resolve_dep(const char *dep, ResolvedDep *out_resolved_dep, ResolvedRule *resolved_rules, const size_t resolved_rules_amount);
/*static*/ StatusCode resolved_rules_init(UnresolvedRule *unresolved_rules, size_t unresolved_rules_amount, ResolvedRule **out_resolved_rules);
#endif