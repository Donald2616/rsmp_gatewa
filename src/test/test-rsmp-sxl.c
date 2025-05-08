
#include "config.h"
#include "rsmp/vars/timestamp.h"
#include "rsmp/messages/messages.h"
#include "rsmp/messages/watchdog.h"
#include "rsmp/messages/cmd-request.h"
#include "rsmp/messages/cmd-response.h"
#include "rsmp/messages/messageack.h"
#include "rsmp/messages/messagenotack.h"
#include "rsmp/messages/version.h"
#include "rsmp/messages/alarm.h"
#include "rsmp/messages/agg-request.h"
#include "rsmp/messages/agg-status.h"
#include "rsmp/messages/stat-request.h"
#include "rsmp/messages/stat-response.h"
#include "rsmp/messages/stat-subscribe.h"
#include "rsmp/messages/stat-unsubscribe.h"
#include "rsmp/coder/json.h"
#include "sync/queue.h"
#include "sync/ldc-mutex.h"
#include "lock/condition.h"


// pthread_context_t *receive_task;
// static void *external_receive(void *arg);
// queue_t *external_q_in;
// condition_t external_enabled;


static int external_signal_enabled_eval(void *context)
{
    queue_t *external_q = context;
    _log_info("external_signal_enabled_eval : nb ptxt %d", external_q->no(external_q));
    return external_q->no(external_q);
}

int waitForMessage(queue_t *q_ext_in, condition_t *ext_en, rsmp_messages_ptr *msg, void *attrib, size_t to_sec);



void test_rsmp_sxl(void)
{
    


    char * rsmp_sId_Local="sId_Svms";
    char * rsmp_sId_Remote="sId_supervision";

    //-- Log les conditions de démarrage...
    rsmp_context_global_log();
    
    // external_q_in = queue_new();
    // condition_initialize(&external_enabled);
    // condition_check_set(&external_enabled, external_signal_enabled_eval, external_q_in);


    
    
    // construction d'un message de demande de status du rsmp_sId_Remote
    rsmp_messages_ptr msgRequest = {0};
    // ...
    msgRequest = rsmp_message_new(NULL, _rsmp_msg_StatusRequest_);
    rsmp_msg_StatusRequest_t *request = msgRequest.StatusRequest;

    sSs *elt = rsmp_array_sSs_new_item(&request->sS);
    rsmp_str_set(&elt->sCI, "GEN/S1");
    rsmp_str_set(&elt->n, "Manufacturer");

    elt = rsmp_array_sSs_new_item(&request->sS);
    rsmp_str_set(&elt->sCI, "GEN/S1");
    rsmp_str_set(&elt->n, "ProductName");

    const char *payload = rsmp_json_msg2payload(msgRequest);

    _log_dump_info_in(payload, strlen(payload), "message status request serialized");

    // en tant qu'equipement rsmp_sId_Local, réception de la demande de status
    // parse de la requete
    rsmp_messages_ptr msgRxed = {0};
    rsmp_context_t const *rsmp_context = rsmp_context_global_get();
    msgRxed = rsmp_json_payload2msg(rsmp_context, payload);

    if (msgRxed.type == NULL)
    {
        _log_err("failed to decode the message");
        free(payload);
        return _rsmp_errcod_cyber_generic_;
    }
    

    void *attrib = NULL;

    //-- execution de la commande associée...
    const rsmp_msg_desc_t *desc = rsmp_message_desc_get(msgRxed); // \todo déplacer dans la external_task

    if (desc->callback != NULL)
    {
        // typedef int (*rsmp_msg_callback_t)(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);
        int ret = desc->callback(NULL, (const void *)(attrib), NULL /* context */, msgRxed);
    }

    _log_info("login = %s", (char *)attrib);


    // creation d'une réponse à partir de msgRxed
    // ...

}
