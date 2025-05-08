/**
* @file messageack.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"
#define RSMP_MESSAGES_MESSAGEACK_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "rsmp/fields/mId.h"

#include "messages.h"

#include "messageack.h"

/**
 * @ingroup messageack
 * @{
 */

rsmp_field_desc_t _rsmp_msg_MessageAck_desc[] =
{
    rsmp_field_desc(rsmp_msg_MessageAck_t, mType),
    rsmp_field_desc(rsmp_msg_MessageAck_t, type),
    rsmp_field_desc(rsmp_msg_MessageAck_t, oMId),
    rsmp_field_desc_end(),
};

rsmp_msg_MessageAck_t *rsmp_msg_MessageAck_new(rsmp_context_t const *rsmp_context)
{
    (void)rsmp_context;
    rsmp_msg_MessageAck_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_MessageAck_;
    }
    return item;
}

void *_rsmp_msg_MessageAck_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_MessageAck_new(rsmp_context);
}

void rsmp_msg_MessageAck_free(rsmp_msg_MessageAck_t *item)
{
    if (item)
    {
        free(item);
    }
}

void _rsmp_msg_MessageAck_free(void *item)
{
    rsmp_msg_MessageAck_free((rsmp_msg_MessageAck_t *)item);
}


rsmp_msg_MessageAck_t *rsmp_msg_MessageAck_to(rsmp_messages_ptr msg)
{
    assert(msg.type != NULL);
    rsmp_msg_MessageAck_t *item = NULL;
    //-- validation des ack suivant le type de message
    if (rsmp_message_desc_ack_get(msg))
    {
        rsmp_field_desc_t const *field = rsmp_message_desc_find_field_by_type(rsmp_message_desc_get(msg)->fields, _rsmp_field_mId_);
        if (field)
        {
            mId_t *id = ((mId_t*)(((char *)msg.type) + field->offset));
            item = rsmp_msg_MessageAck_new(NULL);
            if (item)
            {
                rsmp_oMId_copy(item->oMId, *id);
            }

        }
    }
    return item;
}



/**@}*/