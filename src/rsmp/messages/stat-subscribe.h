/**
* @file stat-subscribe.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_STAT_SUBSCRIBE_H
#define RSMP_MESSAGES_STAT_SUBSCRIBE_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/sS-array.h"
#include "rsmp/fields/uRt.h"

/**
 * @defgroup stat-subscribe 
 * 
 * @brief message stat-subscribee
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_STAT_SUBSCRIBE_C
#define STAT_SUBSCRIBE_EXTERN
#define STAT_SUBSCRIBE_PRIVATE
#endif

#ifndef STAT_SUBSCRIBE_EXTERN
#define STAT_SUBSCRIBE_EXTERN extern
#endif

#ifndef STAT_SUBSCRIBE_PRIVATE
#define STAT_SUBSCRIBE_PRIVATE const
#endif


typedef struct rsmp_msg_StatusSubscribe
{
    type_t           type;
    mId_t            mId;
    ntsOId_t         ntsOId;
    xNId_t           xNId;
    cId_t            cId;
    rsmp_array_sSs_t sS;
    mType_t          mType;
} rsmp_msg_StatusSubscribe_t;

rsmp_msg_StatusSubscribe_t *rsmp_msg_StatusSubscribe_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_StatusSubscribe_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_StatusSubscribe_free(rsmp_msg_StatusSubscribe_t *item);
void _rsmp_msg_StatusSubscribe_free(void *);
STAT_SUBSCRIBE_EXTERN STAT_SUBSCRIBE_PRIVATE rsmp_field_desc_t _rsmp_msg_StatusSubscribe_desc[];
/**@}*/
#endif //RSMP_MESSAGES_STAT_SUBSCRIBE_H