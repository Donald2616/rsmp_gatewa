/**
* @file sS.c
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

#include "rsmp/utils/misc.h"
#include "rsmp/utils/strings.h"
#define RSMP_FIELDS_SS_C
#include "sS.h"

/**
 * @ingroup sS
 * @{
 */
const char *_rsmp_sS_lib[_rsmp_sS_no_] =
{
    [_rsmp_sS_Suspended_]    = "Suspended",
    [_rsmp_sS_notSuspended_] = "notSuspended",
}; 




static inline sS_t rsmp_sS_form_string(char const * str)
{
    return (sS_t)rsmp_lib_find(_rsmp_sS_lib, _rsmp_sS_no_, str);
}


int __rsmp_field_sS_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_sS_lib_get(*((sS_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_sS_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        sS_t sS = rsmp_sS_form_string(var->data.s.t);
        if (sS < _rsmp_sS_no_)
        {
        *((sS_t *)field) = sS;
        }
        else 
        {
            ret =1;
        }
    }
    return ret;
}


/**@}*/