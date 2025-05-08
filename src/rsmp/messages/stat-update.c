/**
* @file agg-request.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"
#define RSMP_MESSAGES_STAT_UPDATE_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"

#include "stat-update.h"

/**
 * @ingroup stat-update
 * @{
 */
static rsmp_field_desc_t _rsmp_msg_sS_desc[] =
{
    rsmp_field_desc(sSs, sCI),
    rsmp_field_desc(sSs, n),
    rsmp_field_desc(sSs, s),
    rsmp_field_desc(sSs, q),
    rsmp_field_desc_end(),
};


rsmp_field_desc_t _rsmp_msg_StatusUpdate_desc[] =
{
    rsmp_field_desc(rsmp_msg_StatusUpdate_t, mType),
    rsmp_field_desc(rsmp_msg_StatusUpdate_t, type),
    rsmp_field_desc(rsmp_msg_StatusUpdate_t, mId),
    rsmp_field_desc(rsmp_msg_StatusUpdate_t, ntsOId),
    rsmp_field_desc(rsmp_msg_StatusUpdate_t, xNId),
    rsmp_field_desc(rsmp_msg_StatusUpdate_t, cId),
    rsmp_field_desc(rsmp_msg_StatusUpdate_t, sTs),
    rsmp_field_desc_array(rsmp_msg_StatusUpdate_t, sS, _rsmp_msg_sS_desc),
   rsmp_field_desc_end(),
};

rsmp_msg_StatusUpdate_t *rsmp_msg_StatusUpdate_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_StatusUpdate_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_StatusUpdate_;
        rsmp_mId_new(item->mId);
        rsmp_sTs_set(&item->sTs);
        rsmp_array_sSs_declare(&item->sS);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(item, rsmp_context);
    }
    return item;
}

void *_rsmp_msg_StatusUpdate_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_StatusUpdate_new(rsmp_context);
}




void rsmp_msg_StatusUpdate_free(rsmp_msg_StatusUpdate_t *item)
{
    if (item)
    {
        rsmp_ntsOId_free(&item->ntsOId);
        rsmp_xNId_free(&item->xNId);
        rsmp_cId_free(&item->cId);
        free(item);
    }
}

void _rsmp_msg_StatusUpdate_free(void *item)
{
    rsmp_msg_StatusUpdate_free((rsmp_msg_StatusUpdate_t *)item);
}



/**@}*/