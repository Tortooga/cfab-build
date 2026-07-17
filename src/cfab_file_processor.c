#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include "cfab_file_processor.h"
#include "status.h"

#define PREPROCESSOR_BUFFER_SIZE 1024

static bool char_is_significant(const char target, bool *comment_mode_indicator);
static void count_buffer_significant_chars(const char *buffer, const size_t buffer_size, size_t *out_significant_chars_count, bool *comment_mode_indicator);
static StatusCode count_significant_chars(FILE *file, char *buffer, const size_t buffer_size, size_t *out_significant_chars_count);

StatusCode preprocess_file(FILE *file, char **out_preprocessed_data)
{
    // We use a big buffer instead of reading single charecters to avoid repeated function call overhead from read

    char prepro_buffer[PREPROCESSOR_BUFFER_SIZE];
    size_t significant_chars_count;

    StatusCode status = count_significant_chars(file, prepro_buffer, PREPROCESSOR_BUFFER_SIZE, &significant_chars_count);

    if (status != SUCCESS)
    {
        return status;
    }

    printf("significant chars count: %zu\n", significant_chars_count);
    return IMPLEMENTATION_INCOMPLETE;
}

// We reuse the buffer from preprocess_file to avoid using too much stack memory
static StatusCode count_significant_chars(FILE *file, char *buffer, const size_t buffer_size, size_t *out_significant_chars_count)
{
    *out_significant_chars_count = 0;

    bool comment_mode_indicator = false;
    size_t read_ret;
    
    for (;;) 
    {
        read_ret = fread(buffer, 1, buffer_size, file);

        // Checking whether we hit EOF or have encountered an error
        if (read_ret < buffer_size)
        {
            if (feof(file))
            {
                count_buffer_significant_chars(buffer, read_ret, out_significant_chars_count, &comment_mode_indicator);
                break;
            }

            return PROCESSOR_FILE_READ_ERROR;
        }

        count_buffer_significant_chars(buffer, read_ret, out_significant_chars_count, &comment_mode_indicator);
    } 

    return SUCCESS;
}

static void count_buffer_significant_chars(const char *buffer, const size_t buffer_size, size_t *out_significant_chars_count, bool *comment_mode_indicator)
{
    for (size_t i = 0; i < buffer_size; i++)
    {
        if (char_is_significant(buffer[i], comment_mode_indicator))
        {
            (*out_significant_chars_count)++;
        }
    }
}

static bool char_is_significant(const char target, bool *comment_mode_indicator)
{
    if (*comment_mode_indicator)
    {
        if (target == END_COMMENT_OPERATOR)
        {
            *comment_mode_indicator = false;
        }

        return false;
    }

    if (target == START_COMMENT_OPERATOR)
    {
        *comment_mode_indicator = true;
        return false;
    }

    for (size_t i = 0; i < INSIGNIFICANT_CHARS_COUNT; i++)
    {
        if (target == INSIGNIFICANT_CHARS[i])
        {
            return false;
        }
    }

    return true;
}