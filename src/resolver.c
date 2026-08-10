#include "resolver.h"
#include "status.h"

#include <sys/stat.h>
#include <errno.h>

/*
    Path must be a C-string

    Returns success if the path could successfully be resolved by stat().
    Returns CFAB_ATTEMPTED_TO_ACCESS_NON_EXISTENT_PATH if the path does not resolve to an existing node.
    Returns CFAB_PATH_RESOLUTION_ERROR and sets errno if the path could not be resolved for a different reason.
*/
/*static*/ StatusCode verify_path(const char *path)
{
    struct stat info;
    
    // if stat succeeds this implies the file exists and 
    if (stat(path, &info) == 0)
    {
        return SUCCESS;
    }

    if (errno == ENOENT)
    {
        return CFAB_ATTEMPTED_TO_ACCESS_NON_EXISTENT_PATH;
    }

    return CFAB_PATH_RESOLUTION_ERROR;
}

