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
        
        // cfab file Parser
            PARSER_FAILED_TO_READ_TARGET_NAME = 301,
            PARSER_FAILED_TO_COUNT_DEPENDANCIES_AMOUNT = 302,
            PARSER_FAILED_TO_STORE_DEPENDANCIES_TOKENS = 303,
            PARSER_FAILED_TO_COUNT_CMDS_AMOUNT = 304,
            PARSER_FAILED_TO_STORE_CMDS_TOKENS = 305,
            PARSER_EOF_BEFORE_GRAMMATICAL_TERMINATION = 306,
            PARSER_PARSE_RULE_FAILED = 307,
} StatusCode;

#endif