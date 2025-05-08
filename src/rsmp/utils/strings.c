/**
* @file strings.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-06-30
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define RSMP_UTILS_STRINGS_C
#include "strings.h"

/**
 * @ingroup strings
 * @{
 */

char *rsmp_util_string_set(char **target, const char *value, const char *defvalue)
{
    if (defvalue && !value)
    {
        if (!*target) *target = strdup(defvalue);
    }
    else
    {
        if (*target) free(*target);
        *target=value?strdup(value):NULL;
    }
    return *target;
}



int rsmp_str_cpy_dup(char const *value, char **str, size_t *len)
{
    assert(str != NULL);
    assert(len != NULL);
    if ( !value )
    {
        *len=0;
        return 1;
    }
    size_t lenmax=strlen(value)+1;
    if (!*str)
    {
        *str = calloc(lenmax, sizeof(**str));
    }
    else if (*str && *len < lenmax)
    {
        return 1;
    }
    if (*str) 
    {
        *len = snprintf(*str, lenmax, "%s", value);
    }
    return 0;
}



char *string_concat(char *dest, char const *src)
{
    if (src)
    {
        if (dest==NULL)
        {
            return strdup(src);
        }
        else
        {
            char *s = realloc(dest, strlen(dest)+strlen(src)+1);
            if (s)
            {
                return strcat(s, src);
            }
        }
    }
    return dest;
}


/**@}*/