/**
* @file RSMP.c
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

#define RSMP_FIELDS_RSMP_C
#include "RSMP.h"

/**
 * @ingroup RSMP
 * @{
 */


void rsmp_array_RSMPs_free_item(RSMPs *item)
{
    if (item->vers) 
    {
        rsmp_vers_free(&item->vers);
        item->vers = NULL;
    }
}


vers_t rsmp_array_RSMPs_check(rsmp_array_RSMPs_t const *local, rsmp_array_RSMPs_t const *remote)
{
    if (!local || !remote) return NULL;
    if (!local->len || !remote->len) return NULL;
    vers_t vers_remote_max = NULL;
    size_t i=0;
    vers_remote_max = remote->items[i].vers;
    i++;
    while(i<remote->len)
    {
        if (strcmp(vers_remote_max, remote->items[i].vers)<0)
        {
            vers_remote_max = remote->items[i].vers;
        }
        i++;
    }
    // recherche si dans la liste locale...
    i=0;
    while(i<local->len)
    {
        if (strcmp(vers_remote_max, local->items[i].vers)<0) return strdup(vers_remote_max);
        i++;
    }
    return NULL;
}



char* rsmp_array_RSMPs_to_string(rsmp_array_RSMPs_t const *item, char const *sep, char **out)
{
    if (item->len)
    {
        size_t i=0;
        if (!sep) sep=", ";

        while(1)
        {
            *out = string_concat(*out, item->items[i].vers);
            i++;
            if (i>=item->len) break;
            *out = string_concat(*out, sep);
        }
    }
    return *out;
}


rsmp_array_RSMPs_t *rsmp_array_RSMPs_dup2(rsmp_array_RSMPs_t *dest, rsmp_array_RSMPs_t const *src)
{
    assert(dest != NULL);
    assert(src != NULL);
    rsmp_array_RSMPs_initialize(dest, src->len);
    for(size_t i=0; i<src->len; i++)
    {
        rsmp_str_set(&dest->items[i].vers, src->items[i].vers);
    }
    return dest;
}


/**@}*/