#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parser.h"
#include "status.h"
#include "preprocessor.h"

/* TODO: MAKE UNRESOLVEDRULE DESTRUCTION FUNCTION */

// Call with parser cursor pointing at the first charecter of a rule
static StatusCode parse_rule(Parser *parser, UnresolvedRule *out_unresolved_rule)
{

} 


/*
    target_unresolved_rule->target_name assumed to be initialised.
    Ends at the NULL terminator of the last dep.
*/
/*static*/ StatusCode parse_cmds(Parser *parser, UnresolvedRule *target_unresolved_rule)
{
    target_unresolved_rule->cmds_amount = 0;

    size_t cmds_amount;

    StatusCode status = count_tokenise_cmds(parser, &cmds_amount, target_unresolved_rule);

    if (status != SUCCESS)
    {
        // count_tokenise_cmds wrote the parser error into the parser struct
        return status;
    }

    /* If there are no cmds to be tokenised then the CMDS_BLOCK_START_OPERATOR
       would not get replaced with a NULL terminator*/
    if (cmds_amount == 0)
    {
        /*// advancing the cursor 
        if (parser->data[parser->cursor] == CMDS_BLOCK_END_OPERATOR)
        {
            parser->cursor++;
        }*/

        // To enforce the invariant of ending at a null terminator at the end of the cmds;
        parser->data[parser->cursor] = '\0';
        return SUCCESS;
    }

    target_unresolved_rule->cmds = malloc(sizeof(char*) * cmds_amount);

    if (target_unresolved_rule->cmds == NULL)
    {
        return CFAB_HEAP_ALLOCATION_FAILED;
    }

    // cmds tokens amount guaranteed to be >= 1
    // cursor is pointing at the first token
    target_unresolved_rule->cmds[0] = &parser->data[parser->cursor];
    size_t cur_cmd_tokens_amount = 1;

    while (parser->cursor < parser->data_length)
    {
        if (cur_cmd_tokens_amount == cmds_amount)
        {
            target_unresolved_rule->cmds_amount = cmds_amount;

            /* Consuming the rest of the last cmd so that after this function exits the
               cursor is pointing at the end of all the cmds */

            if (parser->data[parser->cursor] == '\0')
            {
                return SUCCESS;
            }
        }

        if (parser->data[parser->cursor] == '\0')
        {
            // previous check guarantees there are more cmds. This guarantees there are more tokens which means parser->cursor + 1 is appropriate
            target_unresolved_rule->cmds[cur_cmd_tokens_amount] = &parser->data[parser->cursor + 1];
            cur_cmd_tokens_amount++;
        }

        parser->cursor++;
    }

    // should be impossible
    parser->error_type = EOF_BEFORE_CMDS_TERMINTATION;
    parser->error_target_name = target_unresolved_rule->target_name;
    parser->has_error_target = true;

    target_unresolved_rule->cmds_amount = 0;
    free(target_unresolved_rule->cmds);
    target_unresolved_rule->cmds = NULL;
    return PARSER_FAILED_TO_STORE_CMDS_TOKENS;
}

/*static*/ StatusCode count_tokenise_cmds(Parser *parser, size_t *out_amount, const UnresolvedRule *target_unresolved_rule)
{
    bool no_cmds_flag = true;
    *out_amount = 0;
    size_t counting_cursor = parser->cursor;

    while (counting_cursor < parser->data_length)
    {
        switch (parser->data[counting_cursor])
        {
        case TARGET_NAME_TERMINATION_OPERATOR:
            parser->error_type = UNEXPECTED_TARGET_NAME_TERMINATION_OPERATOR;
            goto failure;
        
        case CMDS_BLOCK_START_OPERATOR:
            parser->error_type = UNEXPECTED_CMD_BLOCK_START_OPERATOR;
            goto failure;

        case DEPENDENCY_DELIMITER:
            parser->error_type = UNEXPTECTED_DEPENDENCY_DELIMITER;
            goto failure;
        
        case CMDS_BLOCK_END_OPERATOR:
            
            if (no_cmds_flag)
            {
                *out_amount = 0;
                return SUCCESS;
            }

            parser->data[counting_cursor] = '\0';

            return SUCCESS;
        
        case CMD_LINE_END_OPERATOR:
            
            parser->data[counting_cursor] = '\0';
            (*out_amount)++;

            counting_cursor++;
            break;
            
        default:
            no_cmds_flag = false;
            counting_cursor++;
            break;
        }
    }

    parser->error_type = RULE_MISSING_CMD_BLOCK_TERMINATOR;

    failure:
        parser->has_error_target = true;
        parser->error_target_name = target_unresolved_rule->target_name;

        return PARSER_FAILED_TO_COUNT_CMDS_AMOUNT;
}


/*
    Call after parse_target_name. This guarantees target_unresolved_rule->target_name is initialised.
*/
/*static*/ StatusCode parse_deps_names(Parser *parser, UnresolvedRule *target_unresolved_rule)
{
    // Writes the beginning of each dependancies token onto target_unresolved_rule and updates target_unresolved_rule->deps_amount
    
    target_unresolved_rule->deps_amount = 0;

    size_t deps_amount;

    StatusCode status = count_tokenise_deps(parser, &deps_amount, target_unresolved_rule);

    if (status != SUCCESS)
    {
        // count_tokenise_deps wrote the parser error into the parser struct
        return status;
    }

    /* If there are no dependancies to be tokenised then the CMDS_BLOCK_START_OPERATOR
       would not get replaced with a NULL terminator */ 
    if (deps_amount == 0)
    {
        if (parser->data[parser->cursor] == CMDS_BLOCK_START_OPERATOR)
        {
            if (parser->cursor + 1 >= parser->data_length)
            {
                parser->error_target_name = target_unresolved_rule->target_name;
                parser->has_error_target = true;
                parser->error_type = RULE_MISSING_CMD_BLOCK_TERMINATOR;

                return PARSER_EOF_BEFORE_GRAMMATICAL_TERMINATION;
            }
            parser->cursor++;
        }

        return SUCCESS;
    }

    target_unresolved_rule->deps = malloc(sizeof(char*) * deps_amount);

    if (target_unresolved_rule->deps == NULL)
    {
        return CFAB_HEAP_ALLOCATION_FAILED;
    }

    // dependancy tokens amount guaranteed to be >= 1
    // cursor is pointing at first token
    target_unresolved_rule->deps[0] = &parser->data[parser->cursor];
    size_t cur_deps_tokens_amount = 1;

    while (parser->cursor < parser->data_length)
    {
        if (cur_deps_tokens_amount == deps_amount)
        {
            target_unresolved_rule->deps_amount = deps_amount;

            /* Consuming the rest of the last dep so that after this function exits the
               cursor is pointing at the end of all the deps */

            if (parser->data[parser->cursor] == '\0')
            {
                return SUCCESS;
            }
        }

        if (parser->data[parser->cursor] == '\0')
        {
            // previous check guarantees there are more dependancies. This guarantees there are more tokens which means parser->cursor + 1 is appropriate
            target_unresolved_rule->deps[cur_deps_tokens_amount] = &parser->data[parser->cursor + 1];
            cur_deps_tokens_amount++;
        }

        parser->cursor++;
    }

    // should be impossible
    parser->error_type = EOF_BEFORE_DEPENDANCIES_TERMINATION;
    parser->error_target_name = target_unresolved_rule->target_name;
    parser->has_error_target = true;

    target_unresolved_rule->deps_amount = 0;
    free(target_unresolved_rule->deps);
    target_unresolved_rule->deps = NULL;
    return PARSER_FAILED_TO_STORE_DEPENDANCIES_TOKENS;
}



/*
    Counts and tokenises the dependancies. Call with parser->cursor pointing at 
    the beginning of the dependancies.

    Does not modiefy the parser cursor.

    Since we are gonna linear scan dependancies to find their amount anyways,
    might as well tokenise them at once so that after allocation we can directly
    store pointers to them.

    Although this introduces coupling, it is more effiecient.
*/
/*static*/ StatusCode count_tokenise_deps(Parser *parser, size_t *out_amount, const UnresolvedRule *target_unresolved_rule)
{   
    bool no_dependancies_flag = true;
    *out_amount = 0;
    size_t counting_cursor = parser->cursor;

    while (counting_cursor < parser->data_length)
    {
        // count dep delimiters until block starter is encountered
        // return error if invalid punctiation is encountered

        switch (parser->data[counting_cursor])
        {

            case TARGET_NAME_TERMINATION_OPERATOR:
                parser->error_type = UNEXPECTED_TARGET_NAME_TERMINATION_OPERATOR;
                goto failure;

            case CMDS_BLOCK_END_OPERATOR:
                parser->error_type = UNEXPECTED_CMD_BLOCK_END_OPERATOR;
                goto failure;   
            
            case CMD_LINE_END_OPERATOR:
                parser->error_type = UNEXPECTED_CMD_DELIMITER;
                goto failure;

            case CMDS_BLOCK_START_OPERATOR:
                
                if (no_dependancies_flag)
                {
                    *out_amount = 0;
                    return SUCCESS;
                }

                parser->data[counting_cursor] = '\0';
 
                /*
                    This loop counts occurances of dependancy delimiters.
                    If there are dependancies, their amount will be equivilent to the amount of delimiters + 1
                    e.g dep1,dep2,dep3{...} there are 2 delimiters so the amount of dependancies is 2 + 2 = 3.  
                */
                *out_amount += 1;
                
                return SUCCESS;
            
            case DEPENDENCY_DELIMITER:

                parser->data[counting_cursor] = '\0';
                (*out_amount)++;

                counting_cursor++;
                break;

            default:
                no_dependancies_flag = false;
                counting_cursor++; 
                break;
        }
    }

    parser->error_type = RULE_MISSING_CMD_BLOCK;

    failure:
        parser->has_error_target = true;
        parser->error_target_name = target_unresolved_rule->target_name;

        return PARSER_FAILED_TO_COUNT_DEPENDANCIES_AMOUNT;
}

/*
    Call with with cursor pointing at the beginning of target name

    Since the function consumes the next punctuation into a NULL terminator,
    We output the no dependencies flag to determine whether the next punctuation
    was the start of the commands block
*/    
/*static*/ StatusCode parse_target_name(Parser *parser, UnresolvedRule *target_unresolved_rule, bool *out_no_deps_flag)
{
    *out_no_deps_flag = false;
    parser->has_error_target = false;

    size_t cur_target_name_length = 0;
    char *target_start = parser->data + parser->cursor;

    while (parser->cursor < parser->data_length)
    {
        if (cur_target_name_length > MAX_TARGET_NAME_LENGTH)
        {
            parser->error_type = TARGET_NAME_IS_TOO_LONG;

            // TODO Add the first part to parser->error_target_name
            return PARSER_FAILED_TO_READ_TARGET_NAME;
        }

        switch (parser->data[parser->cursor])
        {
            case CMDS_BLOCK_START_OPERATOR:
                // The occurrence of the cmd block start operator directly after target name implies the rule has no dependencies
                *out_no_deps_flag = true;

               /* FALLTHROUGH */

            case TARGET_NAME_TERMINATION_OPERATOR:
                target_unresolved_rule->target_name = target_start;

                // Adding NULL terminator to the end of the target name so it can be read as a C-Strings
                parser->data[parser->cursor] = '\0';

                // Consuming the NULL Termination operator
                parser->cursor++;

                return SUCCESS;

            case DEPENDENCY_DELIMITER:
                parser->error_type = UNEXPTECTED_DEPENDENCY_DELIMITER;
                goto target_name_preserving_failure;

            case CMDS_BLOCK_END_OPERATOR:
                parser->error_type = UNEXPECTED_CMD_BLOCK_END_OPERATOR;
                goto target_name_preserving_failure;
            
            case CMD_LINE_END_OPERATOR:                
                parser->error_type = UNEXPECTED_CMD_DELIMITER;
                goto target_name_preserving_failure;

            default:
                parser->cursor++;
                cur_target_name_length++;
                break;
        }
    }

    // End of buffer was encountered before target name terminated
    // TODO: try to preserve target name
    parser->error_type = TARGET_NAME_INCOMPLETE;

    return PARSER_FAILED_TO_READ_TARGET_NAME;

    target_name_preserving_failure:
        // NULL terminating target name for error reporting
        parser->data[parser->cursor] = '\0';
        parser->error_target_name = target_start;
        parser->has_error_target = true;
                
        return PARSER_FAILED_TO_READ_TARGET_NAME;
}