/**
* @file siteId.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-06
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include "rsmp/utils/strings.h"
#include "rsmp/vars/arraystr.h"
#include "rsmp/utils/strings.h"

#define RSMP_FIELDS_SITEID_C
#include "siteId.h"

/**
 * @ingroup siteId
 * @{
 */


void rsmp_array_siteIds_free_item(siteIds *item)
{
    if (item->sId) 
    {
        rsmp_sId_free(&item->sId);
        item->sId = NULL;
    }
}

rsmp_array_siteIds_t *rsmp_array_siteIds_dup2(rsmp_array_siteIds_t *dest, rsmp_array_siteIds_t const *src)
{
    assert(dest != NULL);
    assert(src != NULL);
    rsmp_array_siteIds_initialize(dest, src->len);
    for(size_t i=0; i<src->len; i++)
    {
        rsmp_str_set(&dest->items[i].sId, src->items[i].sId);
    }
    return dest;
}


char *rsmp_array_siteIds_to_string(rsmp_array_siteIds_t const *item, char const *sep, char **out)
{
    if (item->len)
    {
        size_t i=0;
        if (!sep) sep=", ";

        while(1)
        {
            *out = string_concat(*out, item->items[i].sId);
            i++;
            if (i>=item->len) break;
            *out = string_concat(*out, sep);
        }
    }
    return *out;
}

/**@}*/