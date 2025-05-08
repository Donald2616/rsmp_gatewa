/**
* @file messages.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/

#include "config.h"

#include <stdlib.h>
#include <assert.h>

#define RSMP_MESSAGES_MESSAGES_C

#include "cmd-request.h"
#include "cmd-response.h"
#include "messageack.h"
#include "messagenotack.h"
#include "watchdog.h"
#include "alarm.h"
#include "version.h"
#include "stat-request.h"
#include "stat-response.h"
#include "agg-request.h"
#include "agg-status.h"
#include "stat-subscribe.h"
#include "stat-unsubscribe.h"
#include "stat-update.h"
//#include "status-request-ack-included.h"
//#include "status-response-ack-included.h"
//#include "command-request-ack-included.h"
//#include "command-response-ack-included.h"

#include "messages.h"
#include "rsmp/coder/json.h"
/**
 * @ingroup messages
 * @{
 */

#define rsmp_msg_desc(msg) \
[_rsmp_msg_ ## msg ## _] = {\
    .type    = _rsmp_msg_ ## msg ## _,\
    .fields  = _rsmp_msg_ ## msg ## _desc,\
    ._new    = _rsmp_msg_ ## msg ## _new,\
    ._free   = _rsmp_msg_ ## msg ## _free,\
    .ack     = true,\
    .callback = _rsmp_msg_ ## msg ## _exe,\
}
#define rsmp_msg_desc_noack(msg) \
[_rsmp_msg_ ## msg ## _] = {\
    .type    = _rsmp_msg_ ## msg ## _,\
    .fields  = _rsmp_msg_ ## msg ## _desc,\
    ._new    = _rsmp_msg_ ## msg ## _new,\
    ._free   = _rsmp_msg_ ## msg ## _free,\
    .ack     = false,\
    .callback = _rsmp_msg_ ## msg ## _exe,\
}

#define rsmp_msg_desc_fake(msg) \
[_rsmp_msg_ ## msg ## _] = {\
    .type     = _rsmp_msg_no_,\
    .fields   = NULL,\
    ._new     = NULL,\
    ._free    = NULL,\
    .callback = NULL,\
}

rsmp_msg_desc_t __rsmp_msg_desc[_rsmp_msg_no_]={
    rsmp_msg_desc(Alarm),
    rsmp_msg_desc(AggregatedStatus),
    rsmp_msg_desc(AggregatedStatusRequest),
    rsmp_msg_desc(StatusRequest),
    rsmp_msg_desc(StatusResponse),
    rsmp_msg_desc(StatusSubscribe),
    rsmp_msg_desc(StatusUpdate),
    rsmp_msg_desc(StatusUnsubscribe),
    rsmp_msg_desc(CommandRequest),
    rsmp_msg_desc(CommandResponse),
    rsmp_msg_desc_noack(MessageAck),
    rsmp_msg_desc_noack(MessageNotAck),
    rsmp_msg_desc(Version),
    rsmp_msg_desc(Watchdog),
    /*
    rsmp_msg_desc(CommandRequestAckIncluded),
    rsmp_msg_desc(CommandResponseAckIncluded),
    rsmp_msg_desc(StatusResponseAckIncluded),
    rsmp_msg_desc(StatusRequestAckIncluded),
    */
};

rsmp_messages_ptr rsmp_message_new(rsmp_context_t const *rsmp_context, type_t t)
{
    rsmp_messages_ptr msg = {.type=NULL};
    if (rsmp_type_exits(t)) 
    {
        msg.type = (type_t *)__rsmp_msg_desc[t]._new(rsmp_context);
    }
    return msg;
}

void rsmp_message_free(rsmp_messages_ptr msg)
{
    if (msg.type && rsmp_message_exists(msg)) __rsmp_msg_desc[*(msg.type)]._free((void*)msg.type);  
}


char const *rsmp_message_mId2str(rsmp_messages_ptr msg, uuid_str_t str) 
{ 
    if (rsmp_message_is_Alarm(msg))                   return uuid_to_str(msg.Alarm->mId, str);
    if (rsmp_message_is_AggregatedStatus(msg))        return uuid_to_str(msg.AggregatedStatus->mId, str);
    if (rsmp_message_is_AggregatedStatusRequest(msg)) return uuid_to_str(msg.AggregatedStatusRequest->mId, str);
    if (rsmp_message_is_StatusRequest(msg))           return uuid_to_str(msg.StatusRequest->mId, str);
    if (rsmp_message_is_StatusResponse(msg))          return uuid_to_str(msg.StatusResponse->mId, str);
    if (rsmp_message_is_StatusSubscribe(msg))         return uuid_to_str(msg.StatusSubscribe->mId, str);
    if (rsmp_message_is_StatusUpdate(msg))            return uuid_to_str(msg.StatusUpdate->mId, str);
    if (rsmp_message_is_StatusUnsubscribe(msg))       return uuid_to_str(msg.StatusUnsubscribe->mId, str);
    if (rsmp_message_is_CommandRequest(msg))          return uuid_to_str(msg.CommandRequest->mId, str);
    if (rsmp_message_is_CommandResponse(msg))         return uuid_to_str(msg.CommandResponse->mId, str);
    /*
    if (rsmp_message_is_StatusResponseAckIncluded(msg))          return uuid_to_str(msg.StatusResponseAckIncluded->mId, str); // StatusResponseAckIncluded
    if (rsmp_message_is_StatusRequestAckIncluded(msg))           return uuid_to_str(msg.StatusRequestAckIncluded->mId, str); // StatusRequestAckIncluded
    if (rsmp_message_is_CommandResponseAckIncluded(msg))         return uuid_to_str(msg.CommandResponseAckIncluded->mId, str); // CommandResponseAckIncluded
    if (rsmp_message_is_CommandRequestAckIncluded(msg))          return uuid_to_str(msg.CommandRequestAckIncluded->mId, str); // CommandRequestAckIncluded
    */
    if (rsmp_message_is_Version(msg))                 return uuid_to_str(msg.Version->mId, str);
    if (rsmp_message_is_Watchdog(msg))                return uuid_to_str(msg.Watchdog->mId, str);
    return NULL;
}

void rsmp_message_mId_cpy(rsmp_messages_ptr msg, mId_t mId) 
{ 
    if (rsmp_message_is_Alarm(msg))                   rsmp_mId_copy(msg.Alarm->mId, mId);
    else
    if (rsmp_message_is_AggregatedStatus(msg))        rsmp_mId_copy(msg.AggregatedStatus->mId, mId);
    else
    if (rsmp_message_is_AggregatedStatusRequest(msg)) rsmp_mId_copy(msg.AggregatedStatusRequest->mId, mId);
    else
    if (rsmp_message_is_StatusRequest(msg))           rsmp_mId_copy(msg.StatusRequest->mId, mId);
    else
    if (rsmp_message_is_StatusResponse(msg))          rsmp_mId_copy(msg.StatusResponse->mId, mId);
    else
    if (rsmp_message_is_StatusSubscribe(msg))         rsmp_mId_copy(msg.StatusSubscribe->mId, mId);
    else
    if (rsmp_message_is_StatusUpdate(msg))            rsmp_mId_copy(msg.StatusUpdate->mId, mId);
    else
    if (rsmp_message_is_StatusUnsubscribe(msg))       rsmp_mId_copy(msg.StatusUnsubscribe->mId, mId);
    else
    if (rsmp_message_is_CommandRequest(msg))          rsmp_mId_copy(msg.CommandRequest->mId, mId);
    else
    if (rsmp_message_is_CommandResponse(msg))         rsmp_mId_copy(msg.CommandResponse->mId, mId);
    else
    /*
    if (rsmp_message_is_StatusResponseAckIncluded(msg))            rsmp_mId_copy(msg.StatusResponseAckIncluded->mId, mId); // StatusResponseAckIncluded
    else
    if (rsmp_message_is_StatusRequestAckIncluded(msg))            rsmp_mId_copy(msg.StatusRequestAckIncluded->mId, mId); // StatusRequestAckIncluded
    else
    if (rsmp_message_is_CommandResponseAckIncluded(msg))            rsmp_mId_copy(msg.CommandResponseAckIncluded->mId, mId); // CommandResponseAckIncluded
    else
    if (rsmp_message_is_CommandRequestAckIncluded(msg))            rsmp_mId_copy(msg.CommandRequestAckIncluded->mId, mId); // CommandRequestAckIncluded
    else
    */

    if (rsmp_message_is_Version(msg))                 rsmp_mId_copy(msg.Version->mId, mId);
    else
    if (rsmp_message_is_Watchdog(msg))                rsmp_mId_copy(msg.Watchdog->mId, mId);
    else                                              rsmp_mId_clear(mId);
}


int _rsmp_message_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg)
{
    int ret = _rsmp_errcod_gen_generic_;
    (void)context;
    uuid_str_t mId_str;

    if(!rsmp_message_is_MessageAck(msg))
     _rsmp_log_err(ret, "mId:%s, message %s: not executable!", rsmp_message_mId2str(msg, mId_str), rsmp_type_to_string(*msg.type));
    
    rsmp_msg_desc_t *desc = rsmp_message_desc_get(msg);

    if (engine && desc->ack) rsmp_engine_send_msg(engine, rsmp_msg_MessageNotAck_to(msg, "not expected"), attrib);

    if(!engine) {
        _log_err("reception msg.type %s TODO reponse message", rsmp_type_to_string(*msg.type));
    }
    return ret;
}

/**@}*/
