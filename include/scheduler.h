#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "resolver.h"
#include "status.h"

#include <stddef.h>

#define MAX_SCHEDULER_RECURSION_DEPTH 950



StatusCode get_rules_schedule(ResolvedRule *target_rule, ResolvedRule *rules, size_t rules_amount, ResolvedRule ***out_schedule, size_t *out_schedule_length);
void free_schedule(ResolvedRule ***schedule);


#endif