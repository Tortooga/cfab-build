#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>

#include "status.h"

#define MAX_TARGET_NAME_LENGTH 64

#define PARSER_ERROR_BUFFER_LENGTH 256

#define TARGET_NAME_TERMINATION_OPERATOR ':'
#define DEPENDENCY_DELIMITER ','


/*  
    Rule struct where dependancies are only stored as textual names 
    and are not linked to their corseponding nodes.
    all character sequences are null terminated. 
*/
typedef struct 
{
    char *target_name;

    char **deps;
    size_t deps_amount;

    char **cmds;
    size_t cmds_amount;
} UnresolvedRule;


/*
    Types of parser errors
*/
typedef enum 
{
    TARGET_NAME_IS_TOO_LONG,
    TARGET_NAME_INCOMPLETE,
    UNEXPECTED_TARGET_NAME_TERMINATION_OPERATOR,
    UNEXPTECTED_DEPENDENCY_DELIMITER,
    UNEXPECTED_CMD_DELIMITER,
    UNEXPECTED_CMD_BLOCK_END_OPERATOR,
    UNEXPECTED_CMD_BLOCK_START_OPERATOR,
    RULE_MISSING_CMD_BLOCK,
    RULE_MISSING_CMD_BLOCK_TERMINATOR,
    EOF_BEFORE_DEPENDANCIES_TERMINATION,
} ErrorType;

/*  
    cfabfile Parser. 
    data must be mutable preproccessed cfabfile text.
*/
typedef struct 
{
    // Data must be mutable, Parser tokenizes in place
    char *data;
    size_t data_length;
    size_t cursor;

    // C-String
    ErrorType error_type;

    // error_target_name can only be read if has_error_target is true
    char *error_target_name;
    bool has_error_target;
} Parser;

/*static*/ StatusCode parse_deps_names(Parser *parser, UnresolvedRule *target_unresolved_rule);
/*static*/ StatusCode parse_target_name(Parser *parser, UnresolvedRule *target_unresolved_rule, bool *out_no_deps_flag);
/*static*/ StatusCode count_tokenise_deps(Parser *parser, size_t *out_amount, const UnresolvedRule *target_unresolved_rule);
/*static*/ StatusCode count_tokenise_cmds(Parser *parser, size_t *out_amount, const UnresolvedRule *target_unresolved_rule);
#endif