/**
* @file aSp.c
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
#define RSMP_FIELDS_ASP_C
#include "aSp.h"

/**
 * @ingroup aSp
 * @{
 */
const char *_rsmp_aSp_lib[_rsmp_aSp_no_] =
{
    [_rsmp_aSp_Issue_]       = "Issue",
    [_rsmp_aSp_Request_]     = "Request",
    [_rsmp_aSp_Acknowledge_] = "Acknowledge",
    [_rsmp_aSp_Suspend_]     = "Suspend",
    [_rsmp_aSp_Resume_]      = "Resume",
}; 




static inline aSp_t rsmp_aSp_form_string(char const * str)
{
    return (aSp_t)rsmp_lib_find(_rsmp_aSp_lib, _rsmp_aSp_no_, str);
}

int __rsmp_field_aSp_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_aSp_lib_get(*((aSp_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_aSp_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        aSp_t aSp = rsmp_aSp_form_string(var->data.s.t);
        if (aSp < _rsmp_aSp_no_)
        {
        *((aSp_t *)field) = aSp;
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