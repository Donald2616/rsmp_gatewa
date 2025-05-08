/**
* @file str.c
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "errcode/errcode.h"

#include "rsmp/coder/vars.h"

#define RSMP_VARS_STR_C
#include "str.h"
#ifdef LOG_SECTION
#undef LOG_SECTION
#endif
#define LOG_SECTION "STR"

/**
 * @ingroup str
 * @{
 */

int rsmp_str_set(str_t *item, char const *value)
{
    assert(item != NULL);
    if (*item)
    {
        rsmp_str_free(item);
        // *item = NULL;
    }
    if (value)
    {
        *item = strdup(value);
    }
    return _rsmp_no_error_;
}

// void rsmp_str_free(str_t *item) 
// { 
//     if (*item)
//     {
//         free(*item);
//         *item=NULL;
//     }
// };


int __rsmp_field_str_encode(void const *field, rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    // const str_t *str = field;
    return rsmp_var_auto_string(var, *((str_t *)field)) != NULL ? _rsmp_no_error_ : _rsmp_errcod_gen_encode_;
}

int __rsmp_field_str_decode(rsmp_var_t const *var, void *field)
{
    assert(field != NULL);
    str_t *_str = field;
    int ret = _rsmp_no_error_;
    if (rsmp_var_is_string(var))
    {
        rsmp_str_set(_str, var->data.s.t);
    }
    else
    {
       ret=_rsmp_errcod_gen_decode_;
    }
    return ret;
}



/**@}*/