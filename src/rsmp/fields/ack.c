/**
* @file ack.c
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
#define RSMP_FIELDS_ACK_C
#include "ack.h"

/**
 * @ingroup ack
 * @{
 */
const char *_rsmp_ack_lib[_rsmp_ack_no_] =
{
    [_rsmp_ack_notAcknowledged_] = "notAcknowledged",
    [_rsmp_ack_Acknowledged_]    = "Acknowledged",
}; 




static inline ack_t rsmp_ack_form_string(char const * str)
{
    return (ack_t)rsmp_lib_find(_rsmp_ack_lib, _rsmp_ack_no_, str);
}


int __rsmp_field_ack_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_ack_lib_get(*((ack_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_ack_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        ack_t ack = rsmp_ack_form_string(var->data.s.t);
        if (ack < _rsmp_ack_no_)
        {
        *((ack_t *)field) = ack;
        }
        else 
        {
            ret =1;
        }
    }
    return ret;
}




/**@}*/