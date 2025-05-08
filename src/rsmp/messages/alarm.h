/**
* @file watchdog.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_ALARM_H
#define RSMP_MESSAGES_ALARM_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/cat.h"
#include "rsmp/fields/sS.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/aCId.h"
#include "rsmp/fields/xACId.h"
#include "rsmp/fields/xNACId.h"
#include "rsmp/fields/aS.h"
#include "rsmp/fields/sS.h"
#include "rsmp/fields/aTs.h"
#include "rsmp/fields/pri.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/ack.h"
#include "rsmp/fields/rvs.h"
#include "rsmp/fields/aSp.h"

#include "rsmp/messages/messages.h"
/**
 * @defgroup alarme Alarm message
 * 
 * @brief Description du messages Alarm
 * 
 * @ingroup messages
 * @{
 */

#ifdef RSMP_MESSAGES_ALARM_C
#define ALARM_EXTERN
#define ALARM_PRIVATE
#endif

#ifndef ALARM_EXTERN
#define ALARM_EXTERN extern
#endif

#ifndef ALARM_PRIVATE
#define ALARM_PRIVATE const
#endif

typedef struct rsmp_msg_Alarm
{
    type_t           type;
    mId_t            mId;
    ntsOId_t         ntsOId;
    xNId_t           xNId;
    cId_t            cId;
    aCId_t           aCId;
    xACId_t          xACId;
    xNACId_t         xNACId;
    aSp_t            aSp;
    ack_t            ack;
    aS_t             aS;
    sS_t             sS;
    aTs_t            aTs;
    cat_t            cat;
    pri_t            pri;
    rsmp_array_rvs_t rvs;
    mType_t          mType;
} rsmp_msg_Alarm_t;

rsmp_msg_Alarm_t *rsmp_msg_Alarm_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_Alarm_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_Alarm_free(rsmp_msg_Alarm_t *item);
void _rsmp_msg_Alarm_free(void *);

bool rsmp_msg_Alarm_set(rsmp_msg_Alarm_t *watchdog);
bool rsmp_msg_Alarm_field_set(rsmp_msg_Alarm_t *watchdog, rsmp_fields_t field, char const *str);

ALARM_EXTERN ALARM_PRIVATE rsmp_field_desc_t _rsmp_msg_Alarm_desc[];
/**@}*/
#endif //RSMP_MESSAGES_ALARM_H
