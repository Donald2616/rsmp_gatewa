/**
* @file stat-request.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_MESSAGES_STAT_RESPONSE_H
#define RSMP_MESSAGES_STAT_RESPONSE_H

#include "rsmp/fields/fields.h"
#include "rsmp/fields/type.h"
#include "rsmp/fields/mId.h"
#include "rsmp/fields/cId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/sTs.h"
#include "rsmp/fields/sS-array.h"

#include <json-c/json.h>

/**
 * @defgroup stat-request 
 * 
 * @brief message stat-requeste
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_STAT_RESPONSE_C
#define STAT_RESPONSE_EXTERN
#define STAT_RESPONSE_PRIVATE
#endif

#ifndef STAT_RESPONSE_EXTERN
#define STAT_RESPONSE_EXTERN extern
#endif

#ifndef STAT_RESPONSE_PRIVATE
#define STAT_RESPONSE_PRIVATE const
#endif


typedef struct rsmp_msg_StatusResponse
{
    type_t            type;
    mId_t             mId;
    ntsOId_t          ntsOId;
    xNId_t            xNId;
    cId_t             cId;
    sTs_t             sTs;
    rsmp_array_sSs_t  sS;
    mType_t           mType;
} rsmp_msg_StatusResponse_t;

rsmp_msg_StatusResponse_t *rsmp_msg_StatusResponse_new(rsmp_context_t const *rsmp_context);
void *_rsmp_msg_StatusResponse_new(rsmp_context_t const *rsmp_context);
void rsmp_msg_StatusResponse_free(rsmp_msg_StatusResponse_t *item);
void _rsmp_msg_StatusResponse_free(void *);
STAT_RESPONSE_EXTERN STAT_RESPONSE_PRIVATE rsmp_field_desc_t _rsmp_msg_StatusResponse_desc[];
/**@}*/


// Définir le type de fonction de rappel
typedef int (*callback_t)(void *context, sSs *elt, json_object **jobj, char **err);

// Définir une structure pour contenir la chaîne et la fonction de rappel
typedef struct {
    const char *sigName;
    callback_t fct;
} CallbackEntry;

extern CallbackEntry callbacks[];

#endif //RSMP_MESSAGES_STAT_RESPONSE_H