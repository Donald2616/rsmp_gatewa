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
#ifndef RSMP_MESSAGES_VERSION_H
#define RSMP_MESSAGES_VERSION_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/RSMP.h"
#include "rsmp/fields/siteId.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/SXL.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/eSXLG.h"

//#include "rsmp/messages/messages.h"
/**
 * @defgroup watchdog WatchDog message
 * 
 * @brief Description du messages watchdog
 * 
 * @{
 */

#ifdef RSMP_MESSAGES_VERSION_C
#define VERSION_EXTERN
#define VERSION_PRIVATE
#endif

#ifndef VERSION_EXTERN
#define VERSION_EXTERN extern
#endif

#ifndef VERSION_PRIVATE
#define VERSION_PRIVATE const
#endif

typedef struct rsmp_msg_Version
{
    type_t               type;
    mId_t                mId;
    rsmp_array_RSMPs_t   RSMP;
    rsmp_array_siteIds_t siteId;
    SXL_t                SXL;
    rsmp_array_eSXLGs_t  eSXLG;
    mType_t              mType;
} rsmp_msg_Version_t;

rsmp_msg_Version_t *rsmp_msg_Version_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_Version_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_Version_free(rsmp_msg_Version_t *item);
void _rsmp_msg_Version_free(void *);

bool rsmp_msg_Version_set(rsmp_msg_Version_t *version);
bool rsmp_msg_Version_field_set(rsmp_msg_Version_t *version, rsmp_fields_t field, char const *str);

VERSION_EXTERN VERSION_PRIVATE rsmp_field_desc_t _rsmp_msg_Version_desc[];
/**@}*/
#endif //VERSION_MESSAGES_VERSION_H
