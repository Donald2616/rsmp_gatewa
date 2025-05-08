/**
* @file agg-status.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_AGG_STATUS_H
#define RSMP_MESSAGES_AGG_STATUS_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/aSTS.h"
#include "rsmp/fields/fP.h"
#include "rsmp/fields/fS.h"
#include "rsmp/fields/se.h"
#include "rsmp/fields/mType.h"

/**
 * @defgroup agg-request 
 * 
 * @brief message agg-status
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_AGG_STATUS_C
#define AGG_STATUS_EXTERN
#define AGG_STATUS_PRIVATE
#endif

#ifndef AGG_STATUS_EXTERN
#define AGG_STATUS_EXTERN extern
#endif

#ifndef AGG_STATUS_PRIVATE
#define AGG_STATUS_PRIVATE const
#endif


typedef struct rsmp_msg_AggregatedStatus
{
    type_t            type;
    mId_t             mId;
    ntsOId_t          ntsOId;
    xNId_t            xNId;
    cId_t             cId;
    aSTS_t            aSTS;
    fP_t              fP;
    fS_t              fS;
    rsmp_array_se_t   se;
    mType_t           mType;
} rsmp_msg_AggregatedStatus_t;

rsmp_msg_AggregatedStatus_t *rsmp_msg_AggregatedStatus_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_AggregatedStatus_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_AggregatedStatus_free(rsmp_msg_AggregatedStatus_t *item);
void _rsmp_msg_AggregatedStatus_free(void *);
AGG_STATUS_EXTERN AGG_STATUS_PRIVATE rsmp_field_desc_t _rsmp_msg_AggregatedStatus_desc[];
/**@}*/
#endif //RSMP_MESSAGES_AGG_STATUS_H