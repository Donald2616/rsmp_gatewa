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
#define RSMP_FIELDS_CAT_C
#include "cat.h"

/**
 * @ingroup sS
 * @{
 */
const char *_rsmp_cat_lib[_rsmp_cat_no_] =
{
    [_rsmp_cat_T_] = "T",
    [_rsmp_cat_D_] = "D",
}; 




static inline cat_t rsmp_cat_form_string(char const * str)
{
    return (cat_t)rsmp_lib_find(_rsmp_cat_lib, _rsmp_cat_no_, str);
}

int __rsmp_field_cat_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_cat_lib_get(*((cat_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_cat_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        cat_t cat = rsmp_cat_form_string(var->data.s.t);
        if (cat < _rsmp_cat_no_)
        {
        *((cat_t *)field) = cat;
        }
        else 
        {
            ret =1;
        }
    }
    return ret;
}



/**@}*/