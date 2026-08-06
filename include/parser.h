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
    EOF_BEFORE_CMDS_TERMINTATION,
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

// Frees all heap based memory taken up by unresolved_rule
void free_unresolved_rule(UnresolvedRule *unresolved_rule);

// Frees all heap based memory taken up by all the unresolved_rules
void free_unresolved_rules(UnresolvedRule **unresolved_rules, size_t amount);

/*
    Parses the rules in parser.data starting at parser.cursor and ending at parser.
    Parser errors reported in the parser
    parser.cursor must be set to the position where you wish to start, and parser.data_length must be set to the position where you wish to end
*/
StatusCode parse_unresolved_rules(Parser *parser, UnresolvedRule **out_unresolved_rules, size_t *out_unresolved_rules_amount);

/*
    Prints the unresolved rules attributes.
*/
StatusCode print_unresolved_rule(UnresolvedRule *unresolved_rule);

#endif