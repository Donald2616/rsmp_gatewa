/**
* @file stat-request.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"
#define RSMP_MESSAGES_STAT_REQUEST_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"
#include "messageack.h"

#include "stat-request.h"
#include "stat-response.h"
#include "rsmp/coder/json.h"

/**
 * @ingroup cmd-request
 * @{
 */

static rsmp_field_desc_t _rsmp_msg_sS_desc[] =
{
    rsmp_field_desc(sSs, sCI),
    rsmp_field_desc(sSs, n),
    rsmp_field_desc_end(),
};


rsmp_field_desc_t _rsmp_msg_StatusRequest_desc[] =
{
    rsmp_field_desc(rsmp_msg_StatusRequest_t, mType),
    rsmp_field_desc(rsmp_msg_StatusRequest_t, type),
    rsmp_field_desc(rsmp_msg_StatusRequest_t, mId),
    rsmp_field_desc(rsmp_msg_StatusRequest_t, ntsOId),
    rsmp_field_desc(rsmp_msg_StatusRequest_t, xNId),
    rsmp_field_desc(rsmp_msg_StatusRequest_t, cId),
    rsmp_field_desc_array(rsmp_msg_StatusRequest_t, sS, _rsmp_msg_sS_desc),
   rsmp_field_desc_end(),
};

rsmp_msg_StatusRequest_t *rsmp_msg_StatusRequest_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_StatusRequest_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_StatusRequest_;
        rsmp_mId_new(item->mId);
        rsmp_array_sSs_declare(&item->sS);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(item, rsmp_context);
    }
    return item;
}

void *_rsmp_msg_StatusRequest_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_StatusRequest_new(rsmp_context);
}




void rsmp_msg_StatusRequest_free(rsmp_msg_StatusRequest_t *item)
{
    if (item)
    {
        rsmp_ntsOId_free(&item->ntsOId);
        rsmp_xNId_free(&item->xNId);
        rsmp_cId_free(&item->cId);
        rsmp_array_sSs_flush(&item->sS);
        free(item);
    }
}

void _rsmp_msg_StatusRequest_free(void *item)
{
    rsmp_msg_StatusRequest_free((rsmp_msg_StatusRequest_t *)item);
}

int _rsmp_msg_StatusRequest_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg)
{
    int ret = _rsmp_no_error_;
    rsmp_msg_StatusRequest_t *request=msg.StatusRequest;
    assert(request != NULL);
    assert(rsmp_message_is_StatusRequest(msg));


    // const char *payload = rsmp_json_msg2payload(request);

    // _log_dump_info_in(payload, strlen(payload), "message status request serialized");
    // uuid_str_t mId_str;
    // rsmp_oMId_to_string(request->mId, mId_str);
    //-- Vérification que des données sont biens disponibles
    // if (request->arg.len)
    {
        //-- vérification que tous les arg sont jouables...
        // for(size_t i=0; i<request->arg.len; i++)
        // {
        //     if ((ret = rsmp_sxl_cmd_is_active(request->arg.items[i].cCI)) != _rsmp_no_error_) \todo évaluer cette implémentation (doublon)
        //     {
        //         _rsmp_log_err(ret,"mid:%s, cCI: %s", mId_str, request->arg.items[i].cCI);
        //         rsmp_engine_send_msg(engine, rsmp_msg_MessageNotAck_to(msg, "cCI:%s not allowed/defined", request->arg.items[i].cCI), attrib);
        //         return ret;
        //     }
        // } 
        //--acquittement positif !

            // char *rxedWatchdog = rsmp_json_msg2payload(msg);
            // _log_debug("deserialized done: %s\n", rxedWatchdog);

            // rsmp_messages_ptr msgAck = rsmp_msg_MessageAck_to(msg);
            // char *ackWatchdog = rsmp_json_msg2payload(msgAck);
            // _log_debug("deserialized done: %s\n", ackWatchdog);

        // parse de status request
        _log_info("status request received");
        for(size_t i=0; i<request->sS.len; i++)
        {
            _log_info("sCI: %s, n: %s", request->sS.items[i].sCI, request->sS.items[i].n);
        }

        // créer un message de réponse bouchon en remplissant les champs :
        // sCI et n à l'identique, q avec "recent" et s avec "100101" pour n valant "inputstatus" et s avec "100100101" pour n valant "extendedinputstatus"
        rsmp_messages_ptr msgResponse = rsmp_message_new(NULL, _rsmp_msg_StatusResponse_);
        rsmp_msg_StatusResponse_t *response = msgResponse.StatusResponse;
        
        
        struct json_object *json_sS_array = json_object_new_array();
        for (size_t i = 0; i < request->sS.len; i++)
        {
            for (size_t j = 0; j < 2/*sizeof(callbacks)/sizeof(CallbackEntry)*/; j++)
            {
                // vérifier que le contexte est bien rempli
                if (strcmp(callbacks[j].sigName, request->sS.items[j].sCI) == 0)
                {
                    struct json_object *jobj = NULL;
                    int ret = callbacks[j].fct(context, &request->sS.items[i], &jobj, NULL);
                    if (!ret && jobj)
                    {
                        json_object_array_add(json_sS_array, jobj);
                    }
                    break;
                }
                // \todo : harakiri et nack si un traitement echoue :
                // else
                // {
                //     _log_err("sCI: %s not recognized", request->sS.items[i].sCI);
                //     json_obj_free(json_sS_array);
                //     json_sS_array = NULL;
                //     break;
                // } 
            }
        }
        char *payload = NULL;
        if(json_sS_array) {
            _log_info("sS array created");

            // mettre à jour le resultat avec \todo une accroche à revoir
            payload = rsmp_json_msg2payload_with_inserting_jobj(msgResponse, "sS", json_sS_array);

        } else {
            payload = rsmp_json_msg2payload(msgResponse);
        }
        

        if(payload && engine) {
            engine->send(engine, payload, strlen(payload), attrib);
        } else
        {
            // dans le cadre des tests unitaires 
            // \todo revoir l'interface engine et supprimer ce chemin d'éxécution (engine toujours positionné)
            _log_err("reception msg.type %s TODO reponse message", rsmp_type_to_string(*msg.type));
            _log_dump_info_out(payload, strlen(payload), "reponse");
        }

                //-- Creation du message de response
        // rsmp_msg_CommandResponse_t *response = rsmp_msg_CommandResponse_from_request(request);
        // if (response)
        // {
        //     for(size_t i=0; i<request->arg.len; i++)
        //     {
        //         ret = rsmp_sxl_cmd_exe(context, &request->arg.items[i], &response->rvs.items[i]); \todo évaluer cette implémentation (doublon)
        //     }
        //     rsmp_engine_send_msg(engine, attrib, response);
        // }
        // else
        // {


    }
    // else
    // {
    //     ret = _rsmp_errcod_sxl_no_arg_cmd_;
    //     _rsmp_log_err(ret,"mId:%s", mId_str);
    //     rsmp_engine_send_msg(engine, attrib, rsmp_msg_MessageNotAck_to(msg, "no arg items..."));
    // }
    return ret;
}


/**@}*/