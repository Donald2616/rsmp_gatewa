#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"
#include "messageack.h"

#define RSMP_MESSAGES_WATCHDOG_C
#include "watchdog.h"
#include "rsmp/coder/json.h"

rsmp_field_desc_t _rsmp_msg_Watchdog_desc[] =
{
    rsmp_field_desc(rsmp_msg_Watchdog_t, mType),
    rsmp_field_desc(rsmp_msg_Watchdog_t, type),
    rsmp_field_desc(rsmp_msg_Watchdog_t, mId),
    rsmp_field_desc(rsmp_msg_Watchdog_t, wTs),
    rsmp_field_desc_end(),
};


rsmp_msg_Watchdog_t *rsmp_msg_Watchdog_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_Watchdog_t * msg = calloc(1, sizeof(*msg));
    if (msg)
    {
         msg->type = _rsmp_msg_Watchdog_;
        rsmp_msg_Watchdog_set(msg);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(msg, rsmp_context);
    }
    return msg;
}
void *_rsmp_msg_Watchdog_new(rsmp_context_t const *rsmp_context) { return rsmp_msg_Watchdog_new(rsmp_context);}

void rsmp_msg_Watchdog_free(rsmp_msg_Watchdog_t *item)
{
    if (item == NULL) return;
    assert(rsmp_message_is_Watchdog(item));
    free(item);
}

void _rsmp_msg_Watchdog_free(void *item)
{
    rsmp_msg_Watchdog_free((rsmp_msg_Watchdog_t *)item);
}

bool rsmp_msg_Watchdog_field_set(rsmp_msg_Watchdog_t *item, rsmp_fields_t field, char const *str)
{
    bool ret = false;
    if (item != NULL && rsmp_message_is_Watchdog(item))
    {
    switch (field)
    {
        case _rsmp_field_mId_:
            ret = rsmp_mId_from_string(str, item->mId);
            break;
        case _rsmp_field_wTs_:
            ret = rsmp_wTs_from_string(str, &item->wTs);
            break;
        default:
            _log_err("default case");
            break;
    }
    }
    return ret;
}


bool rsmp_msg_Watchdog_set(rsmp_msg_Watchdog_t *watchdog)
{
    if (!watchdog) return false;
    watchdog->type = _rsmp_msg_Watchdog_;
    rsmp_mId_new(watchdog->mId);
    rsmp_wTs_set(&watchdog->wTs);
    return true;
}


/*

  ,ad8888ba,   88888888888  ad88888ba  888888888888  88    ,ad8888ba,    888b      88     I8,        8        ,8I  88888888ba,
 d8"'    `"8b  88          d8"     "8b      88       88   d8"'    `"8b   8888b     88     `8b       d8b       d8'  88      `"8b
d8'            88          Y8,              88       88  d8'        `8b  88 `8b    88      "8,     ,8"8,     ,8"   88        `8b
88             88aaaaa     `Y8aaaaa,        88       88  88          88  88  `8b   88       Y8     8P Y8     8P    88         88
88      88888  88"""""       `"""""8b,      88       88  88          88  88   `8b  88       `8b   d8' `8b   d8'    88         88
Y8,        88  88                  `8b      88       88  Y8,        ,8P  88    `8b 88        `8a a8'   `8a a8'     88         8P
 Y8a.    .a88  88          Y8a     a8P      88       88   Y8a.    .a8P   88     `8888         `8a8'     `8a8'      88      .a8P
  `"Y88888P"   88888888888  "Y88888P"       88       88    `"Y8888Y"'    88      `888          `8'       `8'       88888888Y"'


*/
typedef enum rsmp_wd_sent_state
{
    _wd_sent_wait_for_send_,
    _wd_sent_wait_for_ack_,
}rsmp_wd_sent_state_t;


typedef struct rsmp_wd_ex
{
    HoroDate             last_ts;
    mId_t                last_wd;
    unsigned int         count;
    int                  timeout;
    rsmp_wd_sent_state_t state;  
} rsmp_wd_ex_t;


typedef struct rsmp_Watchdog
{
    HoroDate receive_at;
    rsmp_wd_ex_t sent;
    int          period;
} rsmp_Watchdog_t;

static int _rsmp_watchdog_period= 60*1000;
static int _rsmp_watchdog_ack_to= 5*1000;

void *rsmp_watchdog_context_new(void)
{
    rsmp_Watchdog_t *item = calloc(1, sizeof(*item));
    if (item)
    {
        setSecond(&item->receive_at, -1);
        setSecond(&item->sent.last_ts, -1);
        rsmp_mId_clear(item->sent.last_wd);
        item->sent.timeout = _rsmp_watchdog_ack_to;
        item->sent.state =  _wd_sent_wait_for_send_;
        item->period = _rsmp_watchdog_period;
    }
    return item;
}


int rsmp_Watchdog_timeout_get(void *context)
{
    rsmp_Watchdog_t *wd = context;
    int ret = 0;
    HoroDate now;
    setHorodate(&now);
    switch(wd->sent.state)
    {//int rsmp_Watchdog_check(rsmp_engine_t *engine, char *attrib, void *context, mId_t *mId)

        case _wd_sent_wait_for_send_:
            ret = wd->period - diffHorodateMs(&now, &wd->sent.last_ts);
            _log_debug("ret = %d\n", ret);
            break;
        case _wd_sent_wait_for_ack_:
            ret = wd->sent.timeout - diffHorodateMs(&now, &wd->sent.last_ts);
            break;
    }
    return ret;
}

//int rsmp_Watchdog_check(rsmp_engine_t *engine, char *attrib, void *context, mId_t *mId)
int rsmp_Watchdog_check(rsmp_engine_t *engine, char *attrib, void *context)
{
    rsmp_Watchdog_t *wd = context;
    HoroDate now;
    setHorodate(&now);
    int ret = 0;

    //-- Status de l'état d'émission 
    switch(wd->sent.state)
    {
        case _wd_sent_wait_for_send_:
        {
            if (getSecond(&wd->sent.last_ts)<0 || diffHorodateMs(&now, &wd->sent.last_ts) >= wd->period)
            {
                //-- Envoi d'un watchdog
                rsmp_msg_Watchdog_t *msg_wd = rsmp_msg_Watchdog_new(NULL);
                rsmp_engine_send_msg(engine, msg_wd, attrib);

            }
        }
        break;
    }

    return ret;
}

int _rsmp_msg_Watchdog_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg)
{
    int ret = _rsmp_no_error_;
    rsmp_msg_Watchdog_t *request=msg.Watchdog;
    assert(request != NULL);
    assert(rsmp_message_is_Watchdog(msg));
    // uuid_str_t mId_str;
    // rsmp_oMId_to_string(request->mId, mId_str);
    //-- Vérification que des données sont biens disponibles
    // if (request->arg.len)
    {
        //-- vérification que tous les arg sont jouables...
        // for(size_t i=0; i<request->arg.len; i++)
        // {
        //     if ((ret = rsmp_sxl_cmd_is_active(request->arg.items[i].cCI)) != _rsmp_no_error_)
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

        rsmp_engine_send_msg(engine, rsmp_msg_MessageAck_to(msg), attrib);

        //-- Creation du message de response
        // rsmp_msg_CommandResponse_t *response = rsmp_msg_CommandResponse_from_request(request);
        // if (response)
        // {
        //     for(size_t i=0; i<request->arg.len; i++)
        //     {
        //         ret = rsmp_sxl_cmd_exe(context, &request->arg.items[i], &response->rvs.items[i]);
        //     }
        //     rsmp_engine_send_msg(engine, attrib, response);
        // }
        // else
        // {

        // }

    }
    // else
    // {
    //     ret = _rsmp_errcod_sxl_no_arg_cmd_;
    //     _rsmp_log_err(ret,"mId:%s", mId_str);
    //     rsmp_engine_send_msg(engine, attrib, rsmp_msg_MessageNotAck_to(msg, "no arg items..."));
    // }
    return ret;
}
