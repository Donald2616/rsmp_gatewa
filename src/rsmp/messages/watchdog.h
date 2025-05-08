/**
* @file watchdog.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_WATCHDOG_H
#define RSMP_MESSAGES_WATCHDOG_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/wTs.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mType.h"
// #include "rsmp/engine/engine.h"
//#include "rsmp/messages/messages.h"
/**
 * @defgroup watchdog WatchDog message
 * 
 * @brief Description du messages watchdog
 * 
 * @{
 */

#ifdef RSMP_MESSAGES_WATCHDOG_C
#define WATCHDOG_EXTERN
#define WATCHDOG_PRIVATE
#endif

#ifndef WATCHDOG_EXTERN
#define WATCHDOG_EXTERN extern
#endif

#ifndef WATCHDOG_PRIVATE
#define WATCHDOG_PRIVATE const
#endif

typedef struct rsmp_msg_Watchdog
{
    type_t  type;
    mId_t   mId;
    wTs_t   wTs;
    mType_t mType;
} rsmp_msg_Watchdog_t;

rsmp_msg_Watchdog_t *rsmp_msg_Watchdog_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_Watchdog_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_Watchdog_free(rsmp_msg_Watchdog_t *item);
void _rsmp_msg_Watchdog_free(void *);

bool rsmp_msg_Watchdog_set(rsmp_msg_Watchdog_t *watchdog);
bool rsmp_msg_Watchdog_field_set(rsmp_msg_Watchdog_t *watchdog, rsmp_fields_t field, char const *str);

WATCHDOG_EXTERN WATCHDOG_PRIVATE rsmp_field_desc_t _rsmp_msg_Watchdog_desc[];


int rsmp_Watchdog_check(rsmp_engine_t *engine, char *attrib, void *context);
int rsmp_Watchdog_timeout_get(void *context);
void *rsmp_watchdog_context_new(void);//
int _rsmp_msg_Watchdog_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);

/**@}*/
#endif //RSMP_MESSAGES_WATCHDOG_H
