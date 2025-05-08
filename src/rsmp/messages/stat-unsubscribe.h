/**
* @file stat-unsubscribe.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_STAT_UNSUBSCRIBE_H
#define RSMP_MESSAGES_STAT_UNSUBSCRIBE_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/sS-array.h"
#include "rsmp/fields/sCI.h"
#include "rsmp/fields/n.h"

/**
 * @defgroup stat-unsubscribe 
 * 
 * @brief message stat-unsubscribe
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_STAT_UNSUBSCRIBE_C
#define STAT_UNSUBSCRIBE_EXTERN
#define STAT_UNSUBSCRIBE_PRIVATE
#endif

#ifndef STAT_UNSUBSCRIBE_EXTERN
#define STAT_UNSUBSCRIBE_EXTERN extern
#endif

#ifndef STAT_UNSUBSCRIBE_PRIVATE
#define STAT_UNSUBSCRIBE_PRIVATE const
#endif


typedef struct rsmp_msg_StatusUnsubscribe
{
    type_t           type;
    mId_t            mId;
    ntsOId_t         ntsOId;
    xNId_t           xNId;
    cId_t            cId;
    rsmp_array_sSs_t sS;
    mType_t          mType;
} rsmp_msg_StatusUnsubscribe_t;

rsmp_msg_StatusUnsubscribe_t *rsmp_msg_StatusUnsubscribe_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_StatusUnsubscribe_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_StatusUnsubscribe_free(rsmp_msg_StatusUnsubscribe_t *item);
void _rsmp_msg_StatusUnsubscribe_free(void *);
STAT_UNSUBSCRIBE_EXTERN STAT_UNSUBSCRIBE_PRIVATE rsmp_field_desc_t _rsmp_msg_StatusUnsubscribe_desc[];
/**@}*/
#endif //RSMP_MESSAGES_STAT_UNSUBSCRIBE_H