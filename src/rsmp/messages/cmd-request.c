/**
* @file cmd-request.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"
#define RSMP_MESSAGES_CMD_REQUEST_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"
#include "messageack.h"
#include "messagenotack.h"
#include "cmd-response.h"
#include "rsmp/sxl/sxl.h"

#include "cmd-request.h"
#include "rsmp/coder/json.h"


/**
 * @ingroup cmd-request
 * @{
 */

rsmp_field_desc_t _rsmp_msg_arg_desc[] =
{
    rsmp_field_desc(args, cCI),
    rsmp_field_desc(args, n),
    rsmp_field_desc(args, cO),
    rsmp_field_desc(args, v),
    rsmp_field_desc_end(),
};


rsmp_field_desc_t _rsmp_msg_CommandRequest_desc[] =
{
    rsmp_field_desc(rsmp_msg_CommandRequest_t, mType),
    rsmp_field_desc(rsmp_msg_CommandRequest_t, type),
    rsmp_field_desc(rsmp_msg_CommandRequest_t, mId),
    rsmp_field_desc(rsmp_msg_CommandRequest_t, ntsOId),
    rsmp_field_desc(rsmp_msg_CommandRequest_t, xNId),
    rsmp_field_desc(rsmp_msg_CommandRequest_t, cId),
    rsmp_field_desc_array(rsmp_msg_CommandRequest_t, arg, _rsmp_msg_arg_desc),
   rsmp_field_desc_end(),
};

rsmp_msg_CommandRequest_t *rsmp_msg_CommandRequest_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_CommandRequest_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_CommandRequest_;
        rsmp_mId_new(item->mId);
        rsmp_array_args_declare(&item->arg);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(item, rsmp_context);
    }
    return item;
}

void *_rsmp_msg_CommandRequest_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_CommandRequest_new(rsmp_context);
}




void rsmp_msg_CommandRequest_free(rsmp_msg_CommandRequest_t *item)
{
    if (item)
    {
        rsmp_ntsOId_free(&item->ntsOId);
        rsmp_xNId_free(&item->xNId);
        rsmp_cId_free(&item->cId);
        rsmp_array_args_flush(&item->arg);
        free(item);
    }
}

void _rsmp_msg_CommandRequest_free(void *item)
{
    rsmp_msg_CommandRequest_free((rsmp_msg_CommandRequest_t *)item);
}


int _rsmp_msg_CommandRequest_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg)
{
    int ret = _rsmp_no_error_;
    rsmp_msg_CommandRequest_t *request=msg.CommandRequest;
    assert(request != NULL);
    assert(rsmp_message_is_CommandRequest(msg));
    uuid_str_t mId_str;
       rsmp_oMId_to_string(request->mId, mId_str);
        //-- Vérification que des données sont biens disponibles
        if (request->arg.len)
        {
            //-- vérification que tous les arg sont jouables...
            for(size_t i=0; i<request->arg.len; i++)
            {
                if ((ret = rsmp_sxl_cmd_is_active(request->arg.items[i].cCI)) != _rsmp_no_error_)
                {
                    _rsmp_log_err(ret,"mid:%s, cCI: %s", mId_str, request->arg.items[i].cCI);
                    rsmp_messages_ptr msgNAck = (rsmp_messages_ptr)rsmp_msg_MessageNotAck_to(msg, "cCI:%s not allowed/defined", request->arg.items[i].cCI);

                    return rsmp_engine_send_msg(engine, msgNAck, attrib);
                }
            } 
            //--acquittement positif !
            rsmp_msg_MessageAck_t *ack = rsmp_msg_MessageAck_to(msg);
            rsmp_engine_send_msg(engine, ack, attrib); 
            //-- Creation du message de response
            rsmp_msg_CommandResponse_t *response = rsmp_msg_CommandResponse_from_request(request);
            if (response)
            {
                for(size_t i=0; i<request->arg.len; i++)
                {
                    ret = rsmp_sxl_cmd_exe(context, &request->arg.items[i], &response->rvs.items[i]);
                }
                ret = rsmp_engine_send_msg(engine, response, attrib);
            }
            else
            {

            }

    }
    else
    {
        ret = _rsmp_errcod_sxl_no_arg_cmd_;
        _rsmp_log_err(ret,"mId:%s", mId_str);
        rsmp_messages_ptr msgNAck = (rsmp_messages_ptr)rsmp_msg_MessageNotAck_to(msg, "no arg items...");
        ret = rsmp_engine_send_msg(engine, msgNAck, attrib);
    }
    return ret;
}

/**@}*/