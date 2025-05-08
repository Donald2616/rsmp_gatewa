/**
* @file messages.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-14
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_MESSAGES_H
#define RSMP_MESSAGES_MESSAGES_H

#include <stdbool.h>
#include "cyber/cyber.h"
#define rsmp_engine_t cyber_t
#include "rsmp/env/rsmp-context.h"
#include "rsmp/fields/fields.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"

/**
 * @defgroup messages 
 * 
 * @brief 
 * 
 * @ingroup rsmp
 * @{
 */

#ifdef RSMP_MESSAGES_MESSAGES_C
#define MESSAGES_EXTERN
#define MESSAGES_PRIVATE
#endif

#ifndef MESSAGES_EXTERN
#define MESSAGES_EXTERN extern
#endif

#ifndef MESSAGES_PRIVATE
#define MESSAGES_PRIVATE const
#endif

/**
 * @brief pré déclaration des structures de messages
 * 
 * La macro permet mettre en place une déclaration
 * de type ( \c typedef) issu d'une structure ( \c struct) 
 * sans indiquer le contenu. 
 * 
 * Cela permet de déclarer des pointeur sur ce type avant
 * d'avoir leur contenu.
 * 
 * La macro 
 * - ajoute les elements de syntaxe prefix ( \c rsmp_msg_ )
 *   et suffix ( \c _t ) autour du mon du message RSMP
 * - rédige la ligne de ddéclaration du type.
 * 
 * l'appel suivant
~~~~.c
message_pre_declaration(MonMessageRsmp);
~~~~
 * donne
~~~~.c
 typedef struct rsmp_msg_MonMessageRsmp  rsmp_msg_MonMessageRsmp_t;
~~~~
 * 
 * @param msg  nom du message RSMP dans sa syntaxe exacte
 *             exemple, \c Alarm, \c 
 */
#define message_pre_declaration(msg) typedef struct rsmp_msg_ ## msg  rsmp_msg_ ## msg ## _t
#define message_pointer(msg) rsmp_msg_ ## msg ## _t *   msg

typedef void *(*msg_new_t)(void);
/**
 * Construct a new message pre declaration object
 * 
 */
message_pre_declaration(Watchdog);
message_pre_declaration(CommandResponse);
message_pre_declaration(CommandRequest);
message_pre_declaration(MessageAck);
message_pre_declaration(MessageNotAck);
message_pre_declaration(Version);
message_pre_declaration(Alarm);
message_pre_declaration(StatusRequest);
message_pre_declaration(StatusRequestAckIncluded); // StatusRequestAckIncluded
message_pre_declaration(StatusResponseAckIncluded); // StatusResponseAckIncluded
message_pre_declaration(CommandResponseAckIncluded); // CommandResponseAckIncluded
message_pre_declaration(CommandRequestAckIncluded); // CommandRequestAckIncluded
message_pre_declaration(StatusResponse);
message_pre_declaration(AggregatedStatusRequest);
message_pre_declaration(AggregatedStatus);
message_pre_declaration(StatusSubscribe);
message_pre_declaration(StatusUnsubscribe);
message_pre_declaration(StatusUpdate);

/**
 * @brief type générique pour le passage des messages
 * 
 * @attention __TOUTES__ les structures de messages
 *            __DOIVENT__ avoir comme premier champ
 *            __type__.
 * 
 * Cela permet, en ajoutant dans l'union un champ _type_t_ de
 * disposer d'un moyen d'identifier le type de message déposé
 * dans l'union sans le connaître (règle de la superposition
 * en C).
 * 
 * @note L'ordre des champs dans l'encodage est contôlé par les
 *       tableaux de type _rsmp_msg_xxxx_desc[]. _Même si l'ordre
 *       des champs dans objet json n'a pas d'importance..._
 * 
 * @remark il sera aussi bon de verifier les alignements internes
 *         pour minimiser la taille si l'on est puristes... 
 * 
 */
typedef union rsmp_messages_ptr
{
    type_t         *type;
    message_pointer(Watchdog);
    message_pointer(CommandResponse);
    message_pointer(CommandRequest); 
    message_pointer(CommandResponseAckIncluded);
    message_pointer(CommandRequestAckIncluded);
    message_pointer(StatusRequestAckIncluded);
    message_pointer(StatusResponseAckIncluded);
    message_pointer(MessageAck);
    message_pointer(MessageNotAck);
    message_pointer(Version);
    message_pointer(Alarm);
    message_pointer(StatusRequest);
    message_pointer(StatusResponse);
    message_pointer(AggregatedStatusRequest);
    message_pointer(AggregatedStatus);
    message_pointer(StatusSubscribe);
    message_pointer(StatusUnsubscribe);
    message_pointer(StatusUpdate);
} rsmp_messages_ptr __attribute__((__transparent_union__));

static inline type_t rsmp_message_type_get(rsmp_messages_ptr msg) {return *msg.type;}

static inline bool rsmp_message_is(rsmp_messages_ptr msg, type_t type) { return (*(msg.type)==type);}

#define rsmp_message_is_Alarm(msg)                   rsmp_message_is(msg,_rsmp_msg_Alarm_)
#define rsmp_message_is_AggregatedStatus(msg)        rsmp_message_is(msg,_rsmp_msg_AggregatedStatus_)
#define rsmp_message_is_AggregatedStatusRequest(msg) rsmp_message_is(msg,_rsmp_msg_AggregatedStatusRequest_)
#define rsmp_message_is_StatusRequest(msg)           rsmp_message_is(msg,_rsmp_msg_StatusRequest_)
#define rsmp_message_is_StatusResponse(msg)          rsmp_message_is(msg,_rsmp_msg_StatusResponse_)

#define rsmp_message_is_StatusSubscribe(msg)         rsmp_message_is(msg,_rsmp_msg_StatusSubscribe_)
#define rsmp_message_is_StatusUpdate(msg)            rsmp_message_is(msg,_rsmp_msg_StatusUpdate_)
#define rsmp_message_is_StatusUnsubscribe(msg)       rsmp_message_is(msg,_rsmp_msg_StatusUnsubscribe_)
#define rsmp_message_is_CommandRequest(msg)          rsmp_message_is(msg,_rsmp_msg_CommandRequest_)
#define rsmp_message_is_CommandResponse(msg)         rsmp_message_is(msg,_rsmp_msg_CommandResponse_)
#define rsmp_message_is_StatusResponseAckIncluded(msg)          rsmp_message_is(msg,_rsmp_msg_StatusResponseAckIncluded_)
#define rsmp_message_is_StatusRequestAckIncluded(msg)           rsmp_message_is(msg,_rsmp_msg_StatusRequestAckIncluded_)
#define rsmp_message_is_CommandRequestAckIncluded(msg)          rsmp_message_is(msg,_rsmp_msg_CommandRequestAckIncluded_)
#define rsmp_message_is_CommandResponseAckIncluded(msg)         rsmp_message_is(msg,_rsmp_msg_CommandResponseAckIncluded_)
#define rsmp_message_is_MessageAck(msg)              rsmp_message_is(msg,_rsmp_msg_MessageAck_)
#define rsmp_message_is_MessageNotAck(msg)           rsmp_message_is(msg,_rsmp_msg_MessageNotAck_)
#define rsmp_message_is_Version(msg)                 rsmp_message_is(msg,_rsmp_msg_Version_)
#define rsmp_message_is_Watchdog(msg)                rsmp_message_is(msg,_rsmp_msg_Watchdog_)

typedef void *(*rsmp_msg_new_t)(rsmp_context_t const *context);
typedef void (*rsmp_msg_free_t)(void *item);
// typedef struct rsmp_engine rsmp_engine_t; // \todo voir pour remonter l'objet cyber_t dans un (e)rsmp_engine_t





typedef int (*rsmp_msg_callback_t)(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);
typedef  struct rsmp_msg_desc
{
    type_t  const      type;
    rsmp_field_desc_t const *fields;
    rsmp_msg_new_t           _new;
    rsmp_msg_free_t          _free;
    bool                     ack;
    rsmp_msg_callback_t      callback;
} rsmp_msg_desc_t;

MESSAGES_EXTERN MESSAGES_PRIVATE rsmp_msg_desc_t __rsmp_msg_desc[_rsmp_msg_no_];

int _rsmp_message_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);

// #define _rsmp_msg_Watchdog_exe        _rsmp_message_exe
#define _rsmp_msg_CommandResponse_exe _rsmp_message_exe
// #define _rsmp_msg_CommandRequest_exe  _rsmp_message_exe
#define _rsmp_msg_MessageAck_exe      _rsmp_message_exe
#define _rsmp_msg_MessageNotAck_exe   _rsmp_message_exe
#define _rsmp_msg_Version_exe         _rsmp_message_exe
#define _rsmp_msg_Alarm_exe           _rsmp_message_exe
// #define _rsmp_msg_StatusRequest_exe   _rsmp_message_exe
#define _rsmp_msg_StatusResponse_exe   _rsmp_message_exe
#define _rsmp_msg_AggregatedStatusRequest_exe _rsmp_message_exe
#define _rsmp_msg_AggregatedStatus_exe  _rsmp_message_exe
#define _rsmp_msg_StatusSubscribe_exe   _rsmp_message_exe
#define _rsmp_msg_StatusUnsubscribe_exe _rsmp_message_exe
#define _rsmp_msg_StatusUpdate_exe      _rsmp_message_exe

//#define _rsmp_msg_CommandResponseAckIncluded_exe _rsmp_message_exe
//#define _rsmp_msg_CommandRequestAckIncluded_exe _rsmp_message_exe

//#define _rsmp_msg_StatusResponseAckIncluded_exe   _rsmp_message_exe
//#define _rsmp_msg_StatusRequestAckIncluded_exe   _rsmp_message_exe


static inline bool rsmp_message_exists(rsmp_messages_ptr msg) 
{ 
    return msg.type && rsmp_type_exits(*(msg.type));
}

static inline bool rsmp_message_is_active(rsmp_messages_ptr msg) 
{ 
    return (rsmp_message_exists(msg) && rsmp_type_exits(__rsmp_msg_desc[*(msg.type)].type));
}


static inline const rsmp_msg_desc_t *rsmp_message_desc_get(rsmp_messages_ptr msg) 
{ 
    return rsmp_message_is_active(msg) ? __rsmp_msg_desc + *(msg.type) : NULL;
}


static inline bool rsmp_message_desc_ack_get(rsmp_messages_ptr msg) 
{ 
    return rsmp_message_is_active(msg) ? __rsmp_msg_desc[*(msg.type)].ack : false;
}


char const *rsmp_message_mId2str(rsmp_messages_ptr msg, uuid_str_t str);
void rsmp_message_mId_cpy(rsmp_messages_ptr msg, mId_t uuid);

rsmp_messages_ptr rsmp_message_new(rsmp_context_t const *rsmp_context, type_t type);
void rsmp_message_free(rsmp_messages_ptr msg);

/**@}*/
#endif
