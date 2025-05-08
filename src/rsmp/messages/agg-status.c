/**
* @file agg-status.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"
#define RSMP_MESSAGES_AGG_STATUS_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"

#include "agg-status.h"

/**
 * @ingroup agg-status
 * @{
 */


rsmp_field_desc_t _rsmp_msg_AggregatedStatus_desc[] =
{
    rsmp_field_desc(rsmp_msg_AggregatedStatus_t, mType),
    rsmp_field_desc(rsmp_msg_AggregatedStatus_t, type),
    rsmp_field_desc(rsmp_msg_AggregatedStatus_t, mId),
    rsmp_field_desc(rsmp_msg_AggregatedStatus_t, ntsOId),
    rsmp_field_desc(rsmp_msg_AggregatedStatus_t, xNId),
    rsmp_field_desc(rsmp_msg_AggregatedStatus_t, cId),
    rsmp_field_desc(rsmp_msg_AggregatedStatus_t, aSTS),
    rsmp_field_desc(rsmp_msg_AggregatedStatus_t, fP),
    rsmp_field_desc(rsmp_msg_AggregatedStatus_t, fS),
    rsmp_field_desc_table(rsmp_msg_AggregatedStatus_t, se),
   rsmp_field_desc_end(),
};

rsmp_msg_AggregatedStatus_t *rsmp_msg_AggregatedStatus_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_AggregatedStatus_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_AggregatedStatus_;
        rsmp_mId_new(item->mId);
        rsmp_aSTS_set(&item->aSTS);
        rsmp_array_se_declare(&item->se);
        rsmp_array_se_initialize(&item->se, 8);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(item, rsmp_context);
    }
    return item;
}

void *_rsmp_msg_AggregatedStatus_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_AggregatedStatus_new(rsmp_context);
}




void rsmp_msg_AggregatedStatus_free(rsmp_msg_AggregatedStatus_t *item)
{
    if (item)
    {
        rsmp_ntsOId_free(&item->ntsOId);
        rsmp_xNId_free(&item->xNId);
        rsmp_cId_free(&item->cId);
        rsmp_cId_free(&item->fP);
        rsmp_cId_free(&item->fS);
        rsmp_array_se_flush(&item->se);
        free(item);
    }
}

void _rsmp_msg_AggregatedStatus_free(void *item)
{
    rsmp_msg_AggregatedStatus_free((rsmp_msg_AggregatedStatus_t *)item);
}



/**@}*/