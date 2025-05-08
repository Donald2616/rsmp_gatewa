/**
* @file cmd-response.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_CMD_RESPONSE_H
#define RSMP_MESSAGES_CMD_RESPONSE_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/rvs.h"
#include "rsmp/fields/cTS.h"
#include "rsmp/messages/cmd-request.h"
extern rsmp_field_desc_t _rsmp_msg_rvs_desc[];

/**
 * @defgroup cmd-response 
 * 
 * @brief 
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_CMD_RESPONSE_C
#define CMD_RESPONSE_EXTERN
#define CMD_RESPONSE_PRIVATE
#endif

#ifndef CMD_RESPONSE_EXTERN
#define CMD_RESPONSE_EXTERN extern
#endif

#ifndef CMD_RESPONSE_PRIVATE
#define CMD_RESPONSE_PRIVATE const
#endif


typedef struct rsmp_msg_CommandResponse
{
    type_t            type;
    mId_t             mId;
    ntsOId_t          ntsOId;
    xNId_t            xNId;
    cId_t             cId;
    cTS_t             cTS;
    rsmp_array_rvs_t  rvs;
    mType_t           mType;
} rsmp_msg_CommandResponse_t;

rsmp_msg_CommandResponse_t *rsmp_msg_CommandResponse_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_CommandResponse_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_CommandResponse_free(rsmp_msg_CommandResponse_t *item);
void _rsmp_msg_CommandResponse_free(void *);

rsmp_msg_CommandResponse_t *rsmp_msg_CommandResponse_from_request(rsmp_msg_CommandRequest_t const *request);


CMD_RESPONSE_EXTERN CMD_RESPONSE_PRIVATE rsmp_field_desc_t _rsmp_msg_CommandResponse_desc[];

/**@}*/
#endif //RSMP_MESSAGES_CMD_RESPONSE_H