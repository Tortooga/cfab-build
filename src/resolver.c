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

static StatusCode resolve_dep_as_rule(const char *dep, ResolvedDep *out_resolved_dep, ResolvedRule *resolved_rules, const size_t resolved_rules_amount);
static StatusCode resolve_dep_as_path(const char *dep, ResolvedDep *out_resolved_dep);
static void resolved_rule_free_internal_allocations(ResolvedRule *resolved_rule);
void resolved_rules_free(ResolvedRule *resolved_rules, size_t resolved_rules_amount);
static StatusCode verify_path(const char *path);
static StatusCode resolve_dep(const char *dep, ResolvedDep *out_resolved_dep, ResolvedRule *resolved_rules, const size_t resolved_rules_amount);
static StatusCode resolve_deps(ResolvedRule *target_resolved_rule, ResolvedRule *resolved_rules, const size_t resolved_rules_amount, ErrorObjectsReporter *error_objects_reporter);
static StatusCode resolved_rules_allocate_deps(ResolvedRule *resolved_rules, size_t resolved_rules_amount);
static StatusCode resolved_rules_init(UnresolvedRule *unresolved_rules, size_t unresolved_rules_amount, ResolvedRule **out_resolved_rules);


static StatusCode resolve_deps(ResolvedRule *target_resolved_rule, ResolvedRule *resolved_rules, const size_t resolved_rules_amount, ErrorObjectsReporter *error_objects_reporter);

/*
    Heap allocates and initialises resolved rules.
    If the function succeeds the amount of resolved rules initialised is equal to the amount of unresolved rules.
    If it fails the function does not guarantee any initialisation. 
*/
static StatusCode resolved_rules_init(UnresolvedRule *unresolved_rules, size_t unresolved_rules_amount, ResolvedRule **out_resolved_rules)
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
    Caller is responsible for deallocating deps even upon failure.
*/
static StatusCode resolved_rules_allocate_deps(ResolvedRule *resolved_rules, size_t resolved_rules_amount)
{
    for (size_t i = 0; i < resolved_rules_amount; i++)
    {
        if (resolved_rules[i].unresolved_rule->deps_amount == 0)
        {
            continue;
        }

        resolved_rules[i].deps = malloc(resolved_rules[i].unresolved_rule->deps_amount * sizeof(ResolvedDep));

        if (resolved_rules[i].deps == NULL)
        {
            return CFAB_HEAP_ALLOCATION_FAILED;
        }

        resolved_rules[i].deps_amount = resolved_rules[i].unresolved_rule->deps_amount;
    }

    return SUCCESS;
}


/*
    Calls resolved rules initialiser.
    Resolves dependancies.
    Upon failure resolved rules will be deallocated. Check the error object reporter.
    Upon success caller is responsible for deallocation.
*/
StatusCode resolved_rules_get(ResolvedRule **out_resolved_rules, size_t *out_resolved_rules_amount, UnresolvedRule *unresolved_rules, const size_t unresolved_rules_amount, ErrorObjectsReporter *error_object_reporter)
{
    if (!out_resolved_rules)
    {
        return NULL_POINTER_PASSED;
    }

    *out_resolved_rules = NULL;

    if (!out_resolved_rules_amount || !unresolved_rules || !error_object_reporter)
    {
        return NULL_POINTER_PASSED;
    }

    *out_resolved_rules_amount = 0;

    error_object_reporter->has_error_rule = false;
    error_object_reporter->has_error_dep = false;

    StatusCode status = resolved_rules_init(unresolved_rules, unresolved_rules_amount, out_resolved_rules);

    if (status != SUCCESS)
    {
        // Failure implies no allocation has been made
        return status;
    }

    // Upon succes the amount of resolved_rules returned by resolve_rules_init is equivilent to unresolved_rules_amount
    const size_t resolved_rules_amount = unresolved_rules_amount;

    status = resolved_rules_allocate_deps(*out_resolved_rules, resolved_rules_amount);

    if (status != SUCCESS)
    {
        goto cleanup;
    }

    for (size_t i = 0; i < resolved_rules_amount; i++)
    {
        // Sets error_object_reporter
        status = resolve_deps((*out_resolved_rules) + i, *out_resolved_rules, resolved_rules_amount, error_object_reporter);
        
        if (status != SUCCESS)
        {
            goto cleanup;
        }
    }

    *out_resolved_rules_amount = resolved_rules_amount;
    return SUCCESS;

    cleanup:
        // Can only be invoked after resolved_rules_init is called and resolved_rules_amount is defined
        resolved_rules_free(*out_resolved_rules, resolved_rules_amount);
        *out_resolved_rules = NULL;
        return status;
}

void resolved_rules_free(ResolvedRule *resolved_rules, size_t resolved_rules_amount)
{
    if (resolved_rules_amount == 0 || resolved_rules == NULL)
    {
        return;
    }

    for (size_t i = 0; i < resolved_rules_amount; i++)
    {
        resolved_rule_free_internal_allocations(&(resolved_rules[i]));
    }

    free(resolved_rules);
}

static void resolved_rule_free_internal_allocations(ResolvedRule *resolved_rule)
{
    free(resolved_rule->deps);
    
    resolved_rule->deps = NULL;

    resolved_rule->deps_amount = 0;
}

static StatusCode resolve_deps(ResolvedRule *target_resolved_rule, ResolvedRule *resolved_rules, const size_t resolved_rules_amount, ErrorObjectsReporter *error_objects_reporter)
{
    error_objects_reporter->has_error_rule = false;
    error_objects_reporter->has_error_dep = false;

    if (target_resolved_rule->deps_amount != target_resolved_rule->unresolved_rule->deps_amount)
    {
        return RESOLVER_MISSMATCH_IN_DEP_AMOUNT_BETWEEN_RESOLVED_RULE_AND_UNRESOLVED_RULE;
    }

    StatusCode status;

    for (size_t i = 0; i < target_resolved_rule->unresolved_rule->deps_amount; i++)
    {
        status = resolve_dep(target_resolved_rule->unresolved_rule->deps[i], &(target_resolved_rule->deps[i]), resolved_rules, resolved_rules_amount);
        
        if (status != SUCCESS)
        {
            error_objects_reporter->has_error_rule = true;
            error_objects_reporter->error_rule = target_resolved_rule->unresolved_rule;

            error_objects_reporter->has_error_dep = true;
            error_objects_reporter->error_dep_index = i;

            return status;
        }
    }

    return SUCCESS;
}

static StatusCode resolve_dep(const char *dep, ResolvedDep *out_resolved_dep, ResolvedRule *resolved_rules, const size_t resolved_rules_amount)
{
    StatusCode status = resolve_dep_as_rule(dep, out_resolved_dep, resolved_rules, resolved_rules_amount);

    if (status == SUCCESS)
    {
        return SUCCESS;
    }

    if (status != RESOLVER_DEP_COULD_NOT_BE_RESOLVED_AS_A_RULE)
    {
        return status;
    }

    status = resolve_dep_as_path(dep, out_resolved_dep);

    if (status == RESOLVER_DEP_COULD_NOT_BE_RESOLVED_AS_A_PATH)
    {
        return RESOLVER_DEP_COULD_NOT_BE_RESOLVED;
    }

    return status;
}


/*
    Path must be a C-string

    Returns success if the path could successfully be resolved by stat().
    Returns CFAB_ATTEMPTED_TO_ACCESS_NON_EXISTENT_PATH if the path does not resolve to an existing node.
    Returns CFAB_PATH_RESOLUTION_ERROR and sets errno if the path could not be resolved for a different reason.
*/
static StatusCode verify_path(const char *path)
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


/*
    dep must be a valid C-String.
    Sets out_resolved_rule with the path in the dep if successful.
    Upon failure nothing is initialised.
*/
static StatusCode resolve_dep_as_path(const char *dep, ResolvedDep *out_resolved_dep)
{
    StatusCode status = verify_path(dep);

    if (status != SUCCESS)
    {
        return RESOLVER_DEP_COULD_NOT_BE_RESOLVED_AS_A_PATH;
    }

    out_resolved_dep->type = PATH_DEP;
    out_resolved_dep->dep.path = dep;

    return SUCCESS;
}

/*
    dep has to be a C-string indicating the target name of the dependacny rule.
    a dependancy rule will successfully be returned if dep is a string equivilent to its target name.
*/
static StatusCode resolve_dep_as_rule(const char *dep, ResolvedDep *out_resolved_dep, ResolvedRule *resolved_rules, const size_t resolved_rules_amount)
{
    for (size_t i = 0; i < resolved_rules_amount; i++)
    {
        // resolved_rules[i]unresolved_rules->target_name guaranteed by parser to be NULL terminated.
        if (strcmp(dep, resolved_rules[i].unresolved_rule->target_name) != 0)
        {
            continue;
        }

        out_resolved_dep->type = RULE_DEP;
        out_resolved_dep->dep.resolved_rule = &resolved_rules[i];
        
        return SUCCESS;
    }

    return RESOLVER_DEP_COULD_NOT_BE_RESOLVED_AS_A_RULE;
}