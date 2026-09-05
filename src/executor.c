#include "executor.h"
#include "status.h"
#include "job_analyzer.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

static void report_failure_status(int status);
static void report_command_to_be_executed(char *cmd);

/*
    Jobs must be stale and in schedule order.
*/
StatusCode jobs_exec(Job *jobs, size_t jobs_amount)
{
    if (jobs_amount == 0)
    {
        return SUCCESS;
    }

    if (!jobs)
    {
        return NULL_POINTER_PASSED;
    }

    /*  Nonlinear execution will be added in the future  */

    for (size_t job_index = 0; job_index < jobs_amount; job_index++)
    {
        for (size_t cmd_index = 0; cmd_index < jobs[job_index].rule->cmds_amount; cmd_index++)
        {
            report_command_to_be_executed(jobs[job_index].rule->cmds[cmd_index]);
            
            jobs[job_index].pids[cmd_index] = fork();

            if (jobs[job_index].pids[cmd_index] == 0)
            {

                execl("/usr/bin/env", "env", "bash", "-c", jobs[job_index].rule->cmds[cmd_index], (char *) NULL);

                // Impossible to reach if exec succeeded
                _exit(EXECUTOR_PROCESS_EXEC_FAILED);
            }
            else if (jobs[job_index].pids[cmd_index] > 0)
            {
                int process_status;
                pid_t wait_ret = waitpid(jobs[job_index].pids[cmd_index], &process_status, 0);
                
                if (wait_ret == -1)
                {
                    return EXECUTOR_WAIT_FAILED;
                }

                if (WIFEXITED(process_status) && WEXITSTATUS(process_status) == 0)
                {
                    continue;
                }

                report_failure_status(process_status);
                return EXECUTOR_CHILD_PROCCESS_FAILED;
            }
            else 
            {
                return EXECUTOR_PROCESS_DUPLICATION_FAILED;
            }
        }
    }

    return SUCCESS;
}

static void report_failure_status(int status)
{
    if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status) == 0)
        {
            // Process did not fail
            return;
        }

        printf("Command Failed. Status %d.\n", WEXITSTATUS(status));
        return;
    }

    if (WIFSIGNALED(status))
    {
        printf("Command Failed. Killed By Signal %d.\n", WTERMSIG(status));
        return;
    }

    printf("Execution Error. Status %d.\n", status);
}

/*
    Made to decouple execution logic from reporting
*/
static void report_command_to_be_executed(char *cmd)
{
    printf("%s\n", cmd);
}