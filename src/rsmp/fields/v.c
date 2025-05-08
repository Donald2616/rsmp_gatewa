/**
* @file v.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define RSMP_FIELDS_V_C
#include "v.h"

/**
 * @ingroup v
 * @{
 */

int __rsmp_field_v_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    v_t const *v=field;
    return rsmp_var_auto_string_len(var, v->items, rsmp_array_char_len(v) * rsmp_array_char_item_sizeof()) != NULL ? 0 : 1;
}

int __rsmp_field_v_decode(rsmp_var_t const *var, void *field)
{
    assert(field != NULL);
    assert(var != NULL);
    if (rsmp_var_is_string(var))
    {
        v_t *v=field;
        rsmp_array_char_initialize(v, var->data.s.l);
        if (var->data.s.l) memcpy(v->items, var->data.s.t, var->data.s.l);
    }
    return 0;
}



/**@}*/