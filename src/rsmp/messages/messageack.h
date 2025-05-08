/**
* @file messageack.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_MESSAGEACK_H
#define RSMP_MESSAGES_MESSAGEACK_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/oMId.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mType.h"
#include "rsmp/messages/messages.h"
/**
 * @defgroup messageack 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_MESSAGES_MESSAGEACK_C
#define MESSAGE_ACK_EXTERN
#define MESSAGE_ACK_PRIVATE
#endif

#ifndef MESSAGE_ACK_EXTERN
#define MESSAGE_ACK_EXTERN extern
#endif

#ifndef MESSAGE_ACK_PRIVATE
#define MESSAGE_ACK_PRIVATE const
#endif

typedef struct rsmp_msg_MessageAck
{
    type_t  type;
    oMId_t  oMId;
    mType_t mType;
} rsmp_msg_MessageAck_t;

rsmp_msg_MessageAck_t *rsmp_msg_MessageAck_to(rsmp_messages_ptr msg);
rsmp_msg_MessageAck_t *rsmp_msg_MessageAck_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_MessageAck_new(rsmp_context_t const *rsmp_context);

void rsmp_msg_MessageAck_free(rsmp_msg_MessageAck_t *item);
void _rsmp_msg_MessageAck_free(void *);

MESSAGE_ACK_EXTERN MESSAGE_ACK_PRIVATE rsmp_field_desc_t _rsmp_msg_MessageAck_desc[];

/**@}*/
#endif //RSMP_MESSAGES_MESSAGEACK_H