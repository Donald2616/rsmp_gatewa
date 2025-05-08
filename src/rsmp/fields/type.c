/**
* @file type.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define RSMP_FIELDS_TYPE_C
#include "type.h"

/**
 * @ingroup type
 * @{
 */



char const *_rsmp_type_lib[_rsmp_msg_no_] =
{
    [_rsmp_msg_Alarm_]                  ="Alarm",                   // Alarm message
    [_rsmp_msg_AggregatedStatus_]       ="AggregatedStatus",        // Aggregated status message
    [_rsmp_msg_AggregatedStatusRequest_]="AggregatedStatusRequest", // Aggregated status request message
    [_rsmp_msg_StatusRequest_]          ="StatusRequest",           // Status message. Request status
    [_rsmp_msg_StatusResponse_]         ="StatusResponse",          // Status message. Status response
    [_rsmp_msg_StatusSubscribe_]        ="StatusSubscribe",         // Status message. Start subscription
    [_rsmp_msg_StatusUpdate_]           ="StatusUpdate",            // Status message. Update of status
    [_rsmp_msg_StatusUnsubscribe_]      ="StatusUnsubscribe",       // Status message. End subscription
    [_rsmp_msg_CommandRequest_]         ="CommandRequest",          // Command message. Request command
    [_rsmp_msg_CommandRequestAckIncluded_]="CommandRequestAckIncluded",// Command message. Acknowledge command request
    [_rsmp_msg_CommandResponseAckIncluded_]="CommandResponseAckIncluded", // Command message. Acknowledge command response
    [_rsmp_msg_StatusResponseAckIncluded_]="StatusResponseAckIncluded", // Status message. Acknowledge status response
    [_rsmp_msg_StatusRequestAckIncluded_]="StatusRequestAckIncluded", // Status message. Acknowledge status request
    [_rsmp_msg_CommandResponse_]        ="CommandResponse",         // Command message. Response of command
    [_rsmp_msg_MessageAck_]             ="MessageAck",              // Message acknowledegment. Successful
    [_rsmp_msg_MessageNotAck_]          ="MessageNotAck",           // Message acknowledegment. Unsuccessful
    [_rsmp_msg_Version_]                ="Version",                 // RSMP / SXL version message
    [_rsmp_msg_Watchdog_]               ="Watchdog",                // Watchdog message
   
};

type_t rsmp_type_from_string(char const *str)
{
    if (str != NULL && *str != '\0')
    {
        for(uint i=0; i<_rsmp_msg_no_; i++)
        {
            if (strcmp(str, _rsmp_type_lib[i])==0) return (type_t)i;
        }
    }
    return _rsmp_msg_no_;
}

char const * rsmp_type_to_string(type_t type)
{
    return ((type<_rsmp_msg_no_)?_rsmp_type_lib[type]:NULL);
}


int __rsmp_field_type_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, rsmp_type_to_string(*((type_t const *)field))) != NULL ? 0 : 1;
}

int __rsmp_field_type_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    int ret = 0;

    if (rsmp_var_is_string(var))
    {
        type_t type = rsmp_type_from_string(var->data.s.t);
        if (type < _rsmp_msg_no_)
        {
        *((type_t *)field) = type;
        }
        else 
        {
            ret =1;
        }
    }
    return ret;
}


/**@}*/
