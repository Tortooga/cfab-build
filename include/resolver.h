#ifndef RESOLVER_H
#define RESOLVER_H

#include "status.h" 

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
        char *path;
    } dep;

    DepType type;
} ResolvedDep;

typedef struct ResolvedRule
{
    const char *target_name;

    ResolvedDep *deps;
    size_t deps_amount;

    const char **cmds;
    size_t cmds_amount;
} ResolvedRule;

/*static*/ StatusCode verify_path(const char *path);
#endif