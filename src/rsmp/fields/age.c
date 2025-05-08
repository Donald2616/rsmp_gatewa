/**
* @file age.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-04
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define RSMP_FIELDS_AGE_C
#include "age.h"

/**
 * @ingroup age
 * @{
 */
const char *_rsmp_age_lib[_rsmp_age_no_] =
{
    [_rsmp_age_recent_]    = "recent",
    [_rsmp_age_old_]       = "old",
    [_rsmp_age_undefined_] = "undefined",
    [_rsmp_age_unknown_]   = "unknown",
}; 

static inline age_t rsmp_age_form_string(char const * str)
{
    if (str != NULL && *str != '\0')
    {
        for(uint i=0; i<_rsmp_age_no_; i++)
        {
            if (strcmp(str, _rsmp_age_lib[i])==0) return (age_t)i;
        }
    }
    return _rsmp_age_no_;
}

int __rsmp_field_age_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_age_lib_get(*((age_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_age_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        age_t age = rsmp_age_form_string(var->data.s.t);
        if (age < _rsmp_age_no_)
        {
        *((age_t *)field) = age;
        }
        else 
        {
            ret =1;
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}


/**@}*/