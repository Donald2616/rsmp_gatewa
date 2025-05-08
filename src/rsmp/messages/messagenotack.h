/**
* @file messagenotack.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_MESSAGENOTACK_H
#define RSMP_MESSAGES_MESSAGENOTACK_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/oMId.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/rea.h"
/**
 * @defgroup messagenotack 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_MESSAGES_MESSAGENOTACK_C
#define MESSAGE_NOT_ACK_EXTERN
#define MESSAGE_NOT_ACK_PRIVATE
#endif

#ifndef MESSAGE_NOT_ACK_EXTERN
#define MESSAGE_NOT_ACK_EXTERN extern
#endif

#ifndef MESSAGE_NOT_ACK_PRIVATE
#define MESSAGE_NOT_ACK_PRIVATE const
#endif

typedef struct rsmp_msg_MessageNotAck
{
    type_t  type;
    oMId_t  oMId;
    rea_t   rea;
    mType_t mType;
} rsmp_msg_MessageNotAck_t;

rsmp_msg_MessageNotAck_t *rsmp_msg_MessageNotAck_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_MessageNotAck_new(rsmp_context_t const *rsmp_context);

void rsmp_msg_MessageNotAck_free(rsmp_msg_MessageNotAck_t *item);
void _rsmp_msg_MessageNotAck_free(void *);

rsmp_msg_MessageNotAck_t *rsmp_msg_MessageNotAck_to(rsmp_messages_ptr msg, char const *rea, ...)__attribute__ ((format(printf, 2, 3)));


MESSAGE_NOT_ACK_EXTERN MESSAGE_NOT_ACK_PRIVATE rsmp_field_desc_t _rsmp_msg_MessageNotAck_desc[];

/**@}*/
#endif //RSMP_MESSAGES_MESSAGENOTACK_H