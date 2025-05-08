/**
 * @file uuid.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "rsmp/coder/vars.h"
#define RSMP_VARS_UUID_C
#include "uuid.h"

/**
 * @ingroup uuid
 * @{
 */

int __rsmp_field_uuid_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    uuid_t const *id = field;
    char str[UUID_STR_LEN];
    uuid_unparse(*id, str);
    return rsmp_var_auto_string(var, str) != NULL ? 0 : 1;
}

int __rsmp_field_uuid_decode(rsmp_var_t const *var, void *field)
{
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        uuid_t *id = field;
        if (var->data.s.t == NULL || *var->data.s.t == '\0')
        {
            uuid_clear(*id);
        }
        else
        {
            ret = uuid_parse(var->data.s.t, *id);
        }
    }
    return ret;
}

/**@}*/