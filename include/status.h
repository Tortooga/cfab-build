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

    /* Domain Specific Status Codes */
        // Cfab Process
            CFAB_FAILED_TO_GET_CWD = 100, 
        // Parser
} StatusCode;

#endif