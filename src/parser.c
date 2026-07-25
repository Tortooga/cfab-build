#include <stddef.h>
#include <stdbool.h>

#include "parser.h"
#include "status.h"

/*static void parse_rule(char *preproccessed_data, size_t preproccessed_data_length, size_t cursor, UnresolvedRule *out_unresolved_rule, char *error_buffer, size_t error_buffer_length)
{
    
}*/


/*
    Since the function consumes the next punctuation into a NULL terminator,
    We output the no dependencies flag to determine whether the next punctuation
    was the start of the commands block
*/    
/*static*/ StatusCode parse_target_name(Parser *parser, UnresolvedRule *out_unresolved_rule, bool *out_no_deps_flag)
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
                out_unresolved_rule->target_name = target_start;

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
            
            case CMD_DELIMITER:                
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