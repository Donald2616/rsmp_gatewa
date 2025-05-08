/**
* @file alarme.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"
#define RSMP_MESSAGES_ALARM_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"

#include "alarm.h"

/**
 * @ingroup alarme
 * @{
 */
static rsmp_field_desc_t _rsmp_msg_rvs_desc[] =
{
    rsmp_field_desc(rvs, n),
    rsmp_field_desc(rvs, v),
    rsmp_field_desc_end(),
};


rsmp_field_desc_t _rsmp_msg_Alarm_desc[] =
{
    rsmp_field_desc(rsmp_msg_Alarm_t, mType),
    rsmp_field_desc(rsmp_msg_Alarm_t, type),
    rsmp_field_desc(rsmp_msg_Alarm_t, mId),
    rsmp_field_desc(rsmp_msg_Alarm_t, ntsOId),
    rsmp_field_desc(rsmp_msg_Alarm_t, xNId),
    rsmp_field_desc(rsmp_msg_Alarm_t, cId),
    rsmp_field_desc(rsmp_msg_Alarm_t, aCId),
    rsmp_field_desc(rsmp_msg_Alarm_t, xACId),
    rsmp_field_desc(rsmp_msg_Alarm_t, xNACId),
    rsmp_field_desc(rsmp_msg_Alarm_t, aSp),
    rsmp_field_desc(rsmp_msg_Alarm_t, ack),
    rsmp_field_desc(rsmp_msg_Alarm_t, aS),
    rsmp_field_desc(rsmp_msg_Alarm_t, sS),
    rsmp_field_desc(rsmp_msg_Alarm_t, aTs),
    rsmp_field_desc(rsmp_msg_Alarm_t, cat),
    rsmp_field_desc(rsmp_msg_Alarm_t, pri),
    rsmp_field_desc_array(rsmp_msg_Alarm_t, rvs, _rsmp_msg_rvs_desc),
   rsmp_field_desc_end(),
};

rsmp_msg_Alarm_t *rsmp_msg_Alarm_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_Alarm_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_Alarm_;
        rsmp_mId_new(item->mId);
        rsmp_aTs_set(&item->aTs);
        rsmp_array_rvs_declare(&item->rvs);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(item, rsmp_context);
    }
    return item;
}

void *_rsmp_msg_Alarm_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_Alarm_new(rsmp_context);
}




void rsmp_msg_Alarm_free(rsmp_msg_Alarm_t *item)
{
    if (item)
    {
        rsmp_ntsOId_free(&item->ntsOId);
        rsmp_xNId_free(&item->xNId);
        rsmp_cId_free(&item->cId);
        rsmp_aCId_free(&item->aCId);
        rsmp_xACId_free(&item->xACId);
        rsmp_xNACId_free(&item->xNACId);
        rsmp_array_rvs_flush(&item->rvs);
        free(item);
    }
}

void _rsmp_msg_Alarm_free(void *item)
{
    rsmp_msg_Alarm_free((rsmp_msg_Alarm_t *)item);
}



/**@}*/