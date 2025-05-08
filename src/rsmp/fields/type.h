/**
* @file type.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_TYPE_H
#define RSMP_FIELDS_TYPE_H
#include <stdlib.h>
#include "rsmp/coder/vars.h"
/**
 * @defgroup type 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_TYPE_C
#define TYPE_EXTERN
#define TYPE_PRIVATE
#endif

#ifndef TYPE_EXTERN
#define TYPE_EXTERN extern
#endif

#ifndef TYPE_PRIVATE
#define TYPE_PRIVATE const
#endif


typedef enum {
    _rsmp_msg_Alarm_ = 0,               // Alarm message
    _rsmp_msg_AggregatedStatus_,        // Aggregated status message
    _rsmp_msg_AggregatedStatusRequest_, // Aggregated status request message
    _rsmp_msg_StatusRequest_,           // Status message. Request status
    _rsmp_msg_StatusResponse_,          // Status message. Status response
    _rsmp_msg_StatusSubscribe_,         // Status message. Start subscription
    _rsmp_msg_StatusUpdate_,            // Status message. Update of status
    _rsmp_msg_StatusUnsubscribe_,       // Status message. End subscription
    _rsmp_msg_CommandRequest_,          // Command message. Request command
    _rsmp_msg_CommandRequestAckIncluded_,// Command message with Ack included
    _rsmp_msg_CommandResponseAckIncluded_,// Command message with Ack included
    _rsmp_msg_StatusResponseAckIncluded_,// Status message with Ack included
    _rsmp_msg_StatusRequestAckIncluded_,// Status message with Ack included
    _rsmp_msg_CommandResponse_,         // Command message. Response of command
    _rsmp_msg_MessageAck_,              // Message acknowledegment. Successful
    _rsmp_msg_MessageNotAck_,           // Message acknowledegment. Unsuccessful
    _rsmp_msg_Version_,                 // RSMP / SXL version message
    _rsmp_msg_Watchdog_,                // Watchdog message

    _rsmp_msg_no_,                      // # messages or error....
} type_t;

extern char const *_rsmp_type_lib[_rsmp_msg_no_];
#define rsmp_type_exits(_t_) ((_t_) < _rsmp_msg_no_)


char const * rsmp_type_to_string(type_t type);
type_t rsmp_type_from_string(char const *str);
// bool rsmp_type_lib(type_t *type, char * const *val);

int __rsmp_field_type_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_type_decode(rsmp_var_t const *var, void *field);
/**@}*/
#endif //RSMP_FIELDS_TYPE_H
