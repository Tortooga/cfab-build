#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "status.h"
#include "job_analyzer.h"

#include <stddef.h>

StatusCode jobs_exec(Job *jobs, size_t jobs_amount);

#endif