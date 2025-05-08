/**
* @file command.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_CMD_REQUEST_H
#define RSMP_MESSAGES_CMD_REQUEST_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/arg.h"
#include "rsmp/messages/messages.h"
extern rsmp_field_desc_t _rsmp_msg_arg_desc[];
// #include "rsmp/engine/engine.h"

/**
 * @defgroup cmd-request 
 * 
 * @brief message commande
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_CMD_REQUEST_C
#define CMD_REQUEST_EXTERN
#define CMD_REQUEST_PRIVATE
#endif

#ifndef CMD_REQUEST_EXTERN
#define CMD_REQUEST_EXTERN extern
#endif

#ifndef CMD_REQUEST_PRIVATE
#define CMD_REQUEST_PRIVATE const
#endif


typedef struct rsmp_msg_CommandRequest
{
    type_t            type;
    mId_t             mId;
    ntsOId_t          ntsOId;
    xNId_t            xNId;
    cId_t             cId;
    rsmp_array_args_t arg;
    mType_t           mType;
} rsmp_msg_CommandRequest_t;

rsmp_msg_CommandRequest_t *rsmp_msg_CommandRequest_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_CommandRequest_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_CommandRequest_free(rsmp_msg_CommandRequest_t *item);
void _rsmp_msg_CommandRequest_free(void *);
CMD_REQUEST_EXTERN CMD_REQUEST_PRIVATE rsmp_field_desc_t _rsmp_msg_CommandRequest_desc[];
int _rsmp_msg_CommandRequest_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);
/**@}*/
#endif //RSMP_MESSAGES_CMD_REQUEST_H