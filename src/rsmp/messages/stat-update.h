/**
* @file stat-update.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_STAT_UPDATE_H
#define RSMP_MESSAGES_STAT_UPDATE_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/sS-array.h"
#include "rsmp/fields/sTs.h"

/**
 * @defgroup stat-update 
 * 
 * @brief message stat-update
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_STAT_UPDATE_C
#define STAT_UPDATE_EXTERN
#define STAT_UPDATE_PRIVATE
#endif

#ifndef STAT_UPDATE_EXTERN
#define STAT_UPDATE_EXTERN extern
#endif

#ifndef STAT_UPDATE_PRIVATE
#define STAT_UPDATE_PRIVATE const
#endif


typedef struct rsmp_msg_StatusUpdate
{
    type_t           type;
    mId_t            mId;
    ntsOId_t         ntsOId;
    xNId_t           xNId;
    cId_t            cId;
    sTs_t            sTs;
    rsmp_array_sSs_t sS;
    mType_t          mType;
} rsmp_msg_StatusUpdate_t;

rsmp_msg_StatusUpdate_t *rsmp_msg_StatusUpdate_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_StatusUpdate_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_StatusUpdate_free(rsmp_msg_StatusUpdate_t *item);
void _rsmp_msg_StatusUpdate_free(void *);
STAT_UPDATE_EXTERN STAT_UPDATE_PRIVATE rsmp_field_desc_t _rsmp_msg_StatusUpdate_desc[];
/**@}*/
#endif //RSMP_MESSAGES_STAT_UPDATE_H