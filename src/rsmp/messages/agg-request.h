/**
* @file agg-request.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_AGG_REQUEST_H
#define RSMP_MESSAGES_AGG_REQUEST_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/mType.h"

/**
 * @defgroup agg-request 
 * 
 * @brief message agg-requeste
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_AGG_REQUEST_C
#define AGG_REQUEST_EXTERN
#define AGG_REQUEST_PRIVATE
#endif

#ifndef AGG_REQUEST_EXTERN
#define AGG_REQUEST_EXTERN extern
#endif

#ifndef AGG_REQUEST_PRIVATE
#define AGG_REQUEST_PRIVATE const
#endif


typedef struct rsmp_msg_AggregatedStatusRequest
{
    type_t            type;
    mId_t             mId;
    ntsOId_t          ntsOId;
    xNId_t            xNId;
    cId_t             cId;
    mType_t           mType;
} rsmp_msg_AggregatedStatusRequest_t;

rsmp_msg_AggregatedStatusRequest_t *rsmp_msg_AggregatedStatusRequest_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_AggregatedStatusRequest_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_AggregatedStatusRequest_free(rsmp_msg_AggregatedStatusRequest_t *item);
void _rsmp_msg_AggregatedStatusRequest_free(void *);
AGG_REQUEST_EXTERN AGG_REQUEST_PRIVATE rsmp_field_desc_t _rsmp_msg_AggregatedStatusRequest_desc[];
/**@}*/
#endif //RSMP_MESSAGES_AGG_REQUEST_H