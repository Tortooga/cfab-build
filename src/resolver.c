#include "resolver.h"
#include "status.h"
#include "parser.h"

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

/*
    Iterate over rules.
    For a given rule, iterate over its dependancies.
    If a dependancy can be resolved as a different rule in our set of rules we store a pointer to that different rule.
    Otherwise, we verify if the dependancy is resolvable in the file system.
    If not then we report that a rule could not be resolved and we terminate.
    Success does not imply the validity of the rule graph.
*/

 
/*static*/ StatusCode resolved_rules_init(UnresolvedRule *unresolved_rules, size_t unresolved_rules_amount, ResolvedRule **out_resolved_rules)
{
     if (unresolved_rules_amount == 0)
     {
        return SUCCESS;
     }

     *out_resolved_rules = malloc(sizeof(ResolvedRule) * unresolved_rules_amount);

     if (*out_resolved_rules == NULL)
     {
        return CFAB_HEAP_ALLOCATION_FAILED;
     }

     // initialising resolved rules
     for (size_t i = 0; i < unresolved_rules_amount; i++)
     {
        (*out_resolved_rules)[i].target_name = unresolved_rules[i].target_name;
        
        (*out_resolved_rules)[i].cmds = unresolved_rules[i].cmds;
        (*out_resolved_rules)[i].cmds_amount = unresolved_rules[i].cmds_amount;

        // We initialise the deps to 0 pending resolution
        (*out_resolved_rules)[i].deps = NULL;
        (*out_resolved_rules)[i].deps_amount = 0;

        (*out_resolved_rules)[i].unresolved_rule = &(unresolved_rules[i]);
     }

     return SUCCESS;
}

/*
    Path must be a C-string

    Returns success if the path could successfully be resolved by stat().
    Returns CFAB_ATTEMPTED_TO_ACCESS_NON_EXISTENT_PATH if the path does not resolve to an existing node.
    Returns CFAB_PATH_RESOLUTION_ERROR and sets errno if the path could not be resolved for a different reason.
*/
/*static*/ StatusCode verify_path(const char *path)
{ 
    struct stat info;
    
    // if stat succeeds this implies the file exists and 
    if (stat(path, &info) == 0)
    {
        return SUCCESS;
    }

    if (errno == ENOENT)
    {
        return CFAB_ATTEMPTED_TO_ACCESS_NON_EXISTENT_PATH;
    }

    return CFAB_PATH_RESOLUTION_ERROR;
}

// dep has to be a C-string indicating the target name of the dependacny rule.
// a dependancy rule will successfully be returned if dep is a string equivilent to its target name.
/*static*/ StatusCode resolve_dep_as_rule(const char *dep, ResolvedDep *out_resolved_dep, const UnresolvedRule *unresolved_rules, const size_t unresolved_rules_amount)
{
    (void)dep;
    (void)out_resolved_dep;
    (void)unresolved_rules;
    (void)unresolved_rules_amount;
    return IMPLEMENTATION_INCOMPLETE;
}