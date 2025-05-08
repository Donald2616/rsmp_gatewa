/**
* @file eSXLG.c
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

#define RSMP_FIELDS_ESXLG_C
#include "eSXLG.h"

/**
 * @ingroup eSXLG
 * @{
 */


void rsmp_array_eSXLGs_free_item(eSXLGs *item)
{
    if (item->vers)
    {
        rsmp_vers_free(&item->vers);
        item->vers = NULL;
    }
    item->sg = _rsmp_sg_no_;
}

// \todo check d'eSXLG
vers_t rsmp_array_eSXLGs_check(rsmp_array_eSXLGs_t const *local, rsmp_array_eSXLGs_t const *remote)
{
    if (!local || !remote) return NULL;
    if (!local->len || !remote->len) return NULL;
    vers_t vers_cumulative = NULL;

    size_t i=0;
    for(size_t i_sg = 0; i_sg<_rsmp_sg_no_; i_sg++)
    {
        vers_t vers_remote_max = NULL;
        for(size_t i_remote = 0; i_remote<remote->len; i_remote++)
        {
            if(remote->items[i].sg == i_sg)
            {
                if (strcmp(vers_remote_max, remote->items[i].vers) < 0) // \todo ce choix du plus petit vers a t il un sens ?
                {
                    vers_remote_max = remote->items[i].vers;
                }
            }

            // par analogie à RSMPs_check
            for (size_t i_local = 0; i_local < local->len; i_local++)
            {
                if (local->items[i].sg == i_sg)
                {
                    if (strcmp(vers_remote_max, local->items[i].vers) < 0) // cela devrait prendre la version la plus courte de tous les locals \todo ce choix du plus petit vers a t il un sens ?
                    {
                        if (vers_remote_max)
                        { // \todo test du check
                            if (vers_cumulative)
                                string_concat(vers_cumulative, " ");
                            string_concat(vers_cumulative, rsmp_sg_to_string(i_sg));
                            string_concat(vers_cumulative, "/");
                            string_concat(vers_cumulative, vers_remote_max);
                        }
                    }
                }
            }
        }
    }

    return vers_cumulative;
}

rsmp_array_eSXLGs_t *rsmp_array_eSXLGs_dup2(rsmp_array_eSXLGs_t *dest, rsmp_array_eSXLGs_t const *src)
{
    assert(dest != NULL);
    assert(src != NULL);
    rsmp_array_eSXLGs_initialize(dest, src->len);
    for(size_t i=0; i<src->len; i++)
    {
        dest->items[i].sg = src->items[i].sg;
        rsmp_str_set(&dest->items[i].vers, src->items[i].vers);
    }
    return dest;
}


char *rsmp_array_eSXLGs_to_string(rsmp_array_eSXLGs_t const *item, char const *sep, char **out)
{
    if (item->len)
    {
        size_t i=0;
        if (!sep) sep=", ";

        while(1)
        {
            *out = string_concat(*out, rsmp_sg_to_string(item->items[i].sg));
            *out = string_concat(*out, sep);
            *out = string_concat(*out, item->items[i].vers);
            i++;
            if (i>=item->len) break;
            *out = string_concat(*out, sep);
        }
    }
    return *out;
}

/**@}*/