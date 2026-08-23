#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

#include "preprocessor.h"
#include "status.h"

#define PREPROCESSOR_TEMP_BUFFER_SIZE 1024

static bool char_is_significant(const char target, bool *comment_mode_indicator, bool *command_block_indicator, bool *command_line_indicator);
static void count_buffer_significant_chars(const char *buffer, const size_t buffer_size, size_t *out_significant_chars_count, bool *comment_mode_indicator, bool *command_block_indicator, bool *command_line_indicator);
static StatusCode count_significant_chars(FILE *file, char *buffer, const size_t buffer_size, size_t *out_significant_chars_count);
static StatusCode copy_buffer_significant_chars(const char *buffer, const size_t buffer_size, char **destination_buffer_cursor, const char *destination_buffer_end, bool *comment_mode_indicator, bool *command_block_indicator, bool *command_line_indicator);
StatusCode copy_significant_chars(FILE *target_file, char *destination_buffer, const size_t destination_buffer_size, char *temp_buffer, const size_t temp_buffer_size);

/*static*/ StatusCode preprocess_file(FILE *file, char **out_preprocessed_data, size_t *out_preprocessed_data_size)
{
    *out_preprocessed_data_size = 0;
    *out_preprocessed_data = NULL;
    StatusCode status = SUCCESS;

    // We use a big buffer instead of reading single charecters to avoid repeated function call overhead from read
    char temp_buffer[PREPROCESSOR_TEMP_BUFFER_SIZE];
    size_t significant_chars_count;

    status = count_significant_chars(file, temp_buffer, PREPROCESSOR_TEMP_BUFFER_SIZE, &significant_chars_count);

    if (status != SUCCESS)
    {
        goto failure;
    }

    *out_preprocessed_data = malloc(significant_chars_count);

    if (*out_preprocessed_data == NULL)
    {
        status = CFAB_HEAP_ALLOCATION_FAILED;
        goto failure;
    }

    status = copy_significant_chars(
        file, 
        *out_preprocessed_data,
        significant_chars_count,
        temp_buffer,
        PREPROCESSOR_TEMP_BUFFER_SIZE
    );

    if (status != SUCCESS)
    {
        goto failure;
    }

    *out_preprocessed_data_size = significant_chars_count;

    return SUCCESS;

    failure:
        destroy_preprocessed_data(*out_preprocessed_data);
        *out_preprocessed_data = NULL;
        return status;
}

StatusCode copy_significant_chars(FILE *target_file, char *destination_buffer, const size_t destination_buffer_size, char *temp_buffer, const size_t temp_buffer_size)
{
    if (fseek(target_file, 0, SEEK_SET) != 0)
    {
        return PROCESSOR_SEEK_FAIL;
    }

    StatusCode status;
    bool comment_mode_indicator = false;
    bool command_block_indicator = false;
    bool command_line_indicator = false;

    size_t read_ret;

    const char *destination_buffer_end = destination_buffer + destination_buffer_size;
    char *destination_buffer_cursor = destination_buffer;

    for (;;)
    {
        read_ret = fread(temp_buffer, 1, temp_buffer_size, target_file);

        if (read_ret < temp_buffer_size)
        {   
            if (feof(target_file))
            {
                status = copy_buffer_significant_chars(
                    temp_buffer, 
                    read_ret,
                    &destination_buffer_cursor,
                    destination_buffer_end,
                    &comment_mode_indicator,
                    &command_block_indicator,
                    &command_line_indicator
                );

                if (status != SUCCESS) return status;

                break;
            }

            return PROCESSOR_FILE_READ_ERROR;
        }

        status = copy_buffer_significant_chars(
            temp_buffer, 
            read_ret,
            &destination_buffer_cursor,
            destination_buffer_end,
            &comment_mode_indicator,
            &command_block_indicator,
            &command_line_indicator
        );

        if (status != SUCCESS) return status;
    }

    return SUCCESS; 
} 

static StatusCode copy_buffer_significant_chars(const char *target_buffer, const size_t target_buffer_size, char **destination_buffer_cursor, const char *destination_buffer_end, bool *comment_mode_indicator, bool *command_block_indicator, bool *command_line_indicator)
{
    for (size_t i = 0; i < target_buffer_size; i++)
    {
        if (char_is_significant(target_buffer[i], comment_mode_indicator, command_block_indicator, command_line_indicator))
        {
            if (*destination_buffer_cursor == destination_buffer_end)
            {
                return PROCESSOR_PREPROCESSOR_BUFFER_FULL;
            }

            **destination_buffer_cursor = target_buffer[i];
            (*destination_buffer_cursor)++;
        }
    }

    return SUCCESS;
}

void destroy_preprocessed_data(char *buffer)
{
    free(buffer);
}

// We reuse the buffer from preprocess_file to avoid using too much stack memory
static StatusCode count_significant_chars(FILE *file, char *buffer, const size_t buffer_size, size_t *out_significant_chars_count)
{
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        return PROCESSOR_SEEK_FAIL;
    }

    *out_significant_chars_count = 0;

    bool comment_mode_indicator = false;
    bool command_block_indicator = false;
    bool command_line_indicator = false;

    size_t read_ret;
    
    for (;;) 
    {
        read_ret = fread(buffer, 1, buffer_size, file);

        // Checking whether we hit EOF or have encountered an error
        if (read_ret < buffer_size)
        {
            if (feof(file))
            {
                count_buffer_significant_chars(buffer, read_ret, out_significant_chars_count, &comment_mode_indicator, &command_block_indicator, &command_line_indicator);
                break;
            }

            return PROCESSOR_FILE_READ_ERROR;
        }

        count_buffer_significant_chars(buffer, read_ret, out_significant_chars_count, &comment_mode_indicator, &command_block_indicator, &command_line_indicator);
    } 

    return SUCCESS;
}

static void count_buffer_significant_chars(const char *buffer, const size_t buffer_size, size_t *out_significant_chars_count, bool *comment_mode_indicator, bool *command_block_indicator, bool *command_line_indicator)
{
    for (size_t i = 0; i < buffer_size; i++)
    {
        if (char_is_significant(buffer[i], comment_mode_indicator, command_block_indicator, command_line_indicator))
        {
            (*out_significant_chars_count)++;
        }
    }
}

static bool char_is_significant(const char target, bool *comment_mode_indicator, bool *command_block_indicator, bool *command_line_indicator)
{
    if (*comment_mode_indicator)
    {
        if (target == END_COMMENT_OPERATOR)
        {
            *comment_mode_indicator = false;
        }

        return false;
    }

    if (target == CMD_LINE_END_OPERATOR)
    {
        *command_line_indicator = false;
        return true;
    }

    if (target == CMDS_BLOCK_START_OPERATOR)
    {
        *command_block_indicator = true;
        return true;
    }

    if (target == CMDS_BLOCK_END_OPERATOR)
    {
        *command_block_indicator = false;
        return true;
    }

    if (target == START_COMMENT_OPERATOR)
    {
        *comment_mode_indicator = true;
        return false;
    }

    if (*command_block_indicator && *command_line_indicator)
    {
        return true;
    }
    
    if (isspace((unsigned char)target))
    {
        return false;
    }

    if (*command_block_indicator)
    {
        *command_line_indicator = true;
        return true;
    }

    return true;
}