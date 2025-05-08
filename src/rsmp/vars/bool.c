/**
* @file bool.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-05
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"


#define RSMP_VARS_BOOL_C
#include "bool.h"

/**
 * @ingroup bool
 * @{
 */

int __rsmp_field_bool_encode(void const *field, rsmp_var_t **var)
{
    return rsmp_var_auto_bool(var, *((bool const *)field)) != NULL ? 0 : 1;
}

int __rsmp_field_bool_decode(rsmp_var_t const *var, void *field)
{
    int ret = 1;
    if (rsmp_var_is_boolean(var))
    {
        *((bool *)field) = var->data.b;
        ret=0;
    }
    return ret;
}




/**@}*/