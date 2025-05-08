/**
* @file mType.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define RSMP_FIELDS_MTYPE_C
#include "mType.h"

/**
 * @ingroup mType
 * @{
 */

static const char *mType_value="rSMsg";


bool rsmp_mType_check(char const *value)
{
    return (value)?(strcmp(value, mType_value)==0):false;
}


int __rsmp_field_mType_encode(void const *field, rsmp_var_t **var)
{
    assert(var != NULL);
    assert(field != NULL);
    return rsmp_var_auto_string(var, mType_value) != NULL ? 0 : 1;
}

int __rsmp_field_mType_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;
    if (rsmp_var_is_string(var))
    {
        if (strcmp(var->data.s.t, mType_value)) 
        {
            ret = 1;
        }
    }
    return ret;
}




/**@}*/