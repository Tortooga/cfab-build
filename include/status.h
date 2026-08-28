#ifndef STATUS_H
#define STATUS_H

/*
    Global StatusCode System.

    Statuscodes accross domains as well as general status codes are grouped under one type.
    This enables status propagation.
*/

typedef enum
{
    /* General Status Codes */
        SUCCESS = 0,
        NULL_POINTER_PASSED = 1,
        IMPLEMENTATION_INCOMPLETE = 2,

    /* Domain Specific Status Codes */
        // Cfab Process
            CFAB_FAILED_TO_GET_MAX_PATH_LENGTH = 101,
            CFAB_FAILED_TO_GET_CWD = 102, 
            CFAB_HEAP_ALLOCATION_FAILED = 103,
            CFAB_FAILED_TO_OPEN_CFAB_FILE = 104,
            CFAB_FILE_DOESNT_EXIST = 105,
            CFAB_FAILED_TO_CLOSE_FILE = 106,
            CFAB_ATTEMPTED_TO_ACCESS_NON_EXISTENT_PATH = 107,
            CFAB_PATH_RESOLUTION_ERROR = 108,

        // cfab File Processor
            PROCESSOR_FILE_READ_ERROR = 201,
            PROCESSOR_PREPROCESSOR_BUFFER_FULL = 202, 
            PROCESSOR_SEEK_FAIL = 203,
        
        // Parser
            PARSER_FAILED_TO_READ_TARGET_NAME = 301,
            PARSER_FAILED_TO_COUNT_DEPENDANCIES_AMOUNT = 302,
            PARSER_FAILED_TO_STORE_DEPENDANCIES_TOKENS = 303,
            PARSER_FAILED_TO_COUNT_CMDS_AMOUNT = 304,
            PARSER_FAILED_TO_STORE_CMDS_TOKENS = 305,
            PARSER_EOF_BEFORE_GRAMMATICAL_TERMINATION = 306,
            PARSER_PARSE_RULE_FAILED = 307,

        // Rule Name Validator
            VALIDATOR_TARGET_NAME_COLLISION = 401,
            VALIDATOR_TARGET_NAME_IS_EMPTY = 402,
            VALIDATOR_RULE_CONTAINS_EMPTY_DEP = 403,

        // Resolver
            RESOLVER_DEP_COULD_NOT_BE_RESOLVED_AS_A_RULE = 501,
            RESOLVER_DEP_COULD_NOT_BE_RESOLVED_AS_A_PATH = 502,
            RESOLVER_DEP_COULD_NOT_BE_RESOLVED = 503,
            RESOLVER_MISSMATCH_IN_DEP_AMOUNT_BETWEEN_RESOLVED_RULE_AND_UNRESOLVED_RULE = 504,

        // Cycle Detector
            CYCLE_DETECTOR_CYCLE_ENCOUNTERED = 601,
            CYCLE_DETECTOR_UNEXPECTED_PENDING_RULE_ENCOUNTERED = 602,
            CYCLE_DETECTOR_MAX_RECURSION_DEPTH_REACHED = 603,

        // Scheduler
            SCHEDULER_MAX_RECURSION_DEPTH_REACHED = 701,
            SCHEDULER_SCHEDULE_INDEX_OUT_OF_BOUNDS = 702,

        // Job Analyzer
            JOB_ANALYZER_MAX_RECURSION_LIMIT_REACHED = 801,
            JOB_ANALYZER_FAILED_TO_GET_TARGET_STATUS = 802,
            JOB_ANALYZER_FAILED_TO_ACCESS_PATH_DEPENDANCY = 803,
        
        // Executor
            EXECUTOR_PROCESS_DUPLICATION_FAILED = 901,
            EXECUTOR_PROCESS_EXEC_FAILED = 902,
} StatusCode;

#endif