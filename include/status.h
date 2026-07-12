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

    /* Domain Specific Status Codes */
        // Cfab Process
            CFAB_FAILED_TO_GET_MAX_PATH_LENGTH = 101,
            CFAB_FAILED_TO_GET_CWD = 102, 
            CFAB_HEAP_ALLOCATION_FAILED = 103,
        // Parser
} StatusCode;

#endif