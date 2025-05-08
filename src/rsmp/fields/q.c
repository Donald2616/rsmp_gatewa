/**
* @file q.c
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

#define RSMP_FIELDS_Q_C
#include "q.h"

/**
 * @ingroup q
 * @{
 */
const char *_rsmp_q_lib[_rsmp_q_no_] =
{
    [_rsmp_q_recent_]    = "recent",
    [_rsmp_q_old_]       = "old",
    [_rsmp_q_undefined_] = "undefined",
    [_rsmp_q_unknown_]   = "unknown",
}; 

static inline q_t rsmp_q_form_string(char const * str)
{
    if (str != NULL && *str != '\0')
    {
        for(uint i=0; i<_rsmp_q_no_; i++)
        {
            if (strcmp(str, _rsmp_q_lib[i])==0) return (q_t)i;
        }
    }
    return _rsmp_q_no_;
}

int __rsmp_field_q_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_q_lib_get(*((q_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_q_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        q_t q = rsmp_q_form_string(var->data.s.t);
        if (q < _rsmp_q_no_)
        {
        *((q_t *)field) = q;
        }
        else 
        {
            ret =1;
        }
    }
    return ret;
}



/**@}*/