/**
* @file arraystr.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-06-28
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#define RSMP_VARS_ARRAYSTR_C
#include "arraystr.h"

/**
 * @ingroup arraystr
 * @{
 */

rsmp_array_char_t * rsmp_array_char_from_str_len(rsmp_array_char_t *array, char const *str, size_t len)
{
    assert(str != NULL);
    if (array == NULL) 
    {
        array=rsmp_array_char_new(len);
    }
    else
    {              
        //if (array->len) rsmp_array_char_flush(array);
        rsmp_array_char_initialize(array, len);
    }
    assert(array != NULL);
    if (!(*str=='\0' || len==0)) memcpy(array->items, str, sizeof(*array->items)*len);
    return array;
}


char * rsmp_array_char_to_str(rsmp_array_char_t *array, char **str, size_t len)
{
    assert(array!=0);
    if (*str==NULL)
    {
        asprintf(str, "%*.*s", (int)array->len, (int)array->len, array->items);
    }
    else
    {
        snprintf(*str, len, "%s", array->items);
    }
    return *str;
}
/**@}*/