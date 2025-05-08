/**
* @file aS.c
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
#define RSMP_FIELDS_AS_C
#include "aS.h"

/**
 * @ingroup aS
 * @{
 */
const char *_rsmp_aS_lib[_rsmp_aS_no_] =
{
    [_rsmp_aS_inActive_] = "inActive",
    [_rsmp_aS_Active_]   = "Active",
}; 




static inline aS_t rsmp_aS_form_string(char const * str)
{
    return (aS_t)rsmp_lib_find(_rsmp_aS_lib, _rsmp_aS_no_, str);
}

int __rsmp_field_aS_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_aS_lib_get(*((aS_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_aS_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        aS_t aS = rsmp_aS_form_string(var->data.s.t);
        if (aS < _rsmp_aS_no_)
        {
        *((aS_t *)field) = aS;
        }
        else 
        {
            ret =1;
        }
    }
    return ret;
}



/**@}*/