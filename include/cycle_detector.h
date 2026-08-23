#ifndef CYCLE_DETECTION_H
#define CYCLE_DETECTION_H

#include "status.h"
#include "resolver.h"

#define MAX_RECURSION_DEPTH 950

StatusCode verify_acyclic(ResolvedRule *rules, size_t rules_amount);

#endif