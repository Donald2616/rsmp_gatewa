/**
* @file pri.c
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
#define RSMP_FIELDS_PRIO_C
#include "pri.h"

/**
 * @ingroup sS
 * @{
 */
const char *_rsmp_pri_lib[_rsmp_pri_no_] =
{
    [_rsmp_pri_1_] = "1",
    [_rsmp_pri_2_] = "2",
    [_rsmp_pri_3_] = "3",
}; 




static inline pri_t rsmp_pri_form_string(char const * str)
{
    return (pri_t)rsmp_lib_find(_rsmp_pri_lib, _rsmp_pri_no_, str);
}

int __rsmp_field_pri_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_pri_lib_get(*((pri_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_pri_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        pri_t pri = rsmp_pri_form_string(var->data.s.t);
        if (pri < _rsmp_pri_no_)
        {
        *((pri_t *)field) = pri;
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