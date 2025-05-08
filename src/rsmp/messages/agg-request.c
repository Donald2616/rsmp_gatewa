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
#define RSMP_MESSAGES_AGG_REQUEST_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"

#include "agg-request.h"

/**
 * @ingroup cmd-request
 * @{
 */


rsmp_field_desc_t _rsmp_msg_AggregatedStatusRequest_desc[] =
{
    rsmp_field_desc(rsmp_msg_AggregatedStatusRequest_t, mType),
    rsmp_field_desc(rsmp_msg_AggregatedStatusRequest_t, type),
    rsmp_field_desc(rsmp_msg_AggregatedStatusRequest_t, mId),
    rsmp_field_desc(rsmp_msg_AggregatedStatusRequest_t, ntsOId),
    rsmp_field_desc(rsmp_msg_AggregatedStatusRequest_t, xNId),
    rsmp_field_desc(rsmp_msg_AggregatedStatusRequest_t, cId),
   rsmp_field_desc_end(),
};

rsmp_msg_AggregatedStatusRequest_t *rsmp_msg_AggregatedStatusRequest_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_AggregatedStatusRequest_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_AggregatedStatusRequest_;
        rsmp_mId_new(item->mId);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(item, rsmp_context);
    }
    return item;
}

void *_rsmp_msg_AggregatedStatusRequest_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_AggregatedStatusRequest_new(rsmp_context);
}




void rsmp_msg_AggregatedStatusRequest_free(rsmp_msg_AggregatedStatusRequest_t *item)
{
    if (item)
    {
        rsmp_ntsOId_free(&item->ntsOId);
        rsmp_xNId_free(&item->xNId);
        rsmp_cId_free(&item->cId);
        free(item);
    }
}

void _rsmp_msg_AggregatedStatusRequest_free(void *item)
{
    rsmp_msg_AggregatedStatusRequest_free((rsmp_msg_AggregatedStatusRequest_t *)item);
}



/**@}*/