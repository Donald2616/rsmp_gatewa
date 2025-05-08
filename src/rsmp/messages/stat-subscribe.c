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
#define RSMP_MESSAGES_STAT_SUBSCRIBE_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"

#include "stat-subscribe.h"

/**
 * @ingroup stat-subscribe
 * @{
 */
static rsmp_field_desc_t _rsmp_msg_sS_desc[] =
{
    rsmp_field_desc(sSs, sCI),
    rsmp_field_desc(sSs, n),
    rsmp_field_desc(sSs, uRt),
    rsmp_field_desc(sSs, sOc),
    rsmp_field_desc_end(),
};


rsmp_field_desc_t _rsmp_msg_StatusSubscribe_desc[] =
{
    rsmp_field_desc(rsmp_msg_StatusSubscribe_t, mType),
    rsmp_field_desc(rsmp_msg_StatusSubscribe_t, type),
    rsmp_field_desc(rsmp_msg_StatusSubscribe_t, mId),
    rsmp_field_desc(rsmp_msg_StatusSubscribe_t, ntsOId),
    rsmp_field_desc(rsmp_msg_StatusSubscribe_t, xNId),
    rsmp_field_desc(rsmp_msg_StatusSubscribe_t, cId),
    rsmp_field_desc_array(rsmp_msg_StatusSubscribe_t, sS, _rsmp_msg_sS_desc),
   rsmp_field_desc_end(),
};

rsmp_msg_StatusSubscribe_t *rsmp_msg_StatusSubscribe_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_StatusSubscribe_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_StatusSubscribe_;
        rsmp_mId_new(item->mId);
        rsmp_array_sSs_declare(&item->sS);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(item, rsmp_context);
    }
    return item;
}

void *_rsmp_msg_StatusSubscribe_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_StatusSubscribe_new(rsmp_context);
}




void rsmp_msg_StatusSubscribe_free(rsmp_msg_StatusSubscribe_t *item)
{
    if (item)
    {
        rsmp_ntsOId_free(&item->ntsOId);
        rsmp_xNId_free(&item->xNId);
        rsmp_cId_free(&item->cId);
        free(item);
    }
}

void _rsmp_msg_StatusSubscribe_free(void *item)
{
    rsmp_msg_StatusSubscribe_free((rsmp_msg_StatusSubscribe_t *)item);
}



/**@}*/