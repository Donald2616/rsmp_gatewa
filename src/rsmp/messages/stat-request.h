/**
* @file stat-request.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_STAT_REQUEST_H
#define RSMP_MESSAGES_STAT_REQUEST_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/sS-array.h"

/**
 * @defgroup stat-request 
 * 
 * @brief message stat-requeste
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_STAT_REQUEST_C
#define STAT_REQUEST_EXTERN
#define STAT_REQUEST_PRIVATE
#endif

#ifndef STAT_REQUEST_EXTERN
#define STAT_REQUEST_EXTERN extern
#endif

#ifndef STAT_REQUEST_PRIVATE
#define STAT_REQUEST_PRIVATE const
#endif


typedef struct rsmp_msg_StatusRequest
{
    type_t            type;
    mId_t             mId;
    ntsOId_t          ntsOId;
    xNId_t            xNId;
    cId_t             cId;
    rsmp_array_sSs_t  sS;
    mType_t           mType;
} rsmp_msg_StatusRequest_t;

rsmp_msg_StatusRequest_t *rsmp_msg_StatusRequest_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_StatusRequest_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_StatusRequest_free(rsmp_msg_StatusRequest_t *item);
void _rsmp_msg_StatusRequest_free(void *);
STAT_REQUEST_EXTERN STAT_REQUEST_PRIVATE rsmp_field_desc_t _rsmp_msg_StatusRequest_desc[];
int _rsmp_msg_StatusRequest_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);

/**@}*/
#endif //RSMP_MESSAGES_STAT_REQUEST_H