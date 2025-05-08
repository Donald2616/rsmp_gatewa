/**
* @file sg.c
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

#define RSMP_FIELDS_SG_C
#include "sg.h"
#include "eSXLG.h"
/**
 * @ingroup sg
 * @{
 */


char const *_rsmp_sg_lib[_rsmp_sg_no_] =
{
    [_rsmp_sg_GEN_] = "GEN",  // GEN message
    [_rsmp_sg_SLT_] = "SLT",  // SLT message
    [_rsmp_sg_IO_]  = "IO",   // IO message
    [_rsmp_sg_VAR_] = "VAR",  // VAR message
};

sg_t rsmp_sg_from_string(char const *str)
{
    if (str != NULL && *str != '\0')
    {
        for(uint i=0; i<_rsmp_sg_no_; i++)
        {
            if (strcmp(str, _rsmp_sg_lib[i])==0) return (sg_t)i;
        }
    }
    return _rsmp_sg_no_;
}

char const * rsmp_sg_to_string(sg_t sg)
{
    return ((sg<_rsmp_sg_no_)?_rsmp_sg_lib[sg]:NULL);
}


int __rsmp_field_sg_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_sg_to_string(*((sg_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_sg_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        sg_t sg = rsmp_sg_from_string(var->data.s.t);
        if (sg < _rsmp_sg_no_)
        {
        // ((eSXLGs *)field)->sg = sg;
        // *((sg_t *)field) = sg;
        }
        else 
        {
            ret =1;
        }
    }
    return ret;
}


/**@}*/
