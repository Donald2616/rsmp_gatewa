/**
* @file rsmp-context.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-06
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_ENV_RSMP_CONTEXT_H
#define RSMP_ENV_RSMP_CONTEXT_H

#include "rsmp/rsmp.h"

#include "rsmp/fields/fields.h"
#include "rsmp/fields/RSMP.h"
#include "rsmp/fields/SXL.h"
#include "rsmp/fields/eSXLG.h"
#include "rsmp/fields/siteId.h"
#include "rsmp/fields/xNId.h"
#include "rsmp/fields/ntsOId.h"
#include "rsmp/fields/cId.h"
/**
 * @defgroup rsmp-context 
 * 
 * @brief 
 * 
 * @ingroup rsmp
  * @{
 */

typedef struct rsmp_context
{
    rsmp_array_RSMPs_t   RSMP;
    rsmp_array_siteIds_t siteId;
    SXL_t                SXL;
    rsmp_array_eSXLGs_t  eSXLG;
    ntsOId_t             ntsOId;
    cId_t                cId;
    xNId_t               xNId;
    sId_t                sId;
} rsmp_context_t;


rsmp_context_t *rsmp_context_new(void);
void rsmp_context_free(rsmp_context_t *item);

void rsmp_context_RSMP_add(rsmp_context_t *item, const vers_t value);
void rsmp_context_siteId_add(rsmp_context_t *item, const sId_t value);
void rsmp_context_SXL_set(rsmp_context_t *item, SXL_t value);
void rsmp_context_eSXLG_add(rsmp_context_t *item, sg_t sg_val, const vers_t vers_val);
void rsmp_context_ntsOId_set(rsmp_context_t *item, ntsOId_t value);
void rsmp_context_cId_set(rsmp_context_t *item, cId_t value);
void rsmp_context_xNId_set(rsmp_context_t *item, xNId_t value);
void rsmp_context_sId_set(rsmp_context_t *item, sId_t value);

#define rsmp_context_global_RSMP_add(_v_)   rsmp_context_RSMP_add(NULL, (_v_))
#define rsmp_context_global_siteId_add(_v_) rsmp_context_siteId_add(NULL, (_v_))
#define rsmp_context_global_SXL_set(_v_)    rsmp_context_SXL_set(NULL, (_v_))
#define rsmp_context_global_eSXLG_add(_v_, _w_) rsmp_context_eSXLG_add(NULL, _v_, _w_)
#define rsmp_context_global_ntsOId_set(_v_) rsmp_context_ntsOId_set(NULL, (_v_))
#define rsmp_context_global_cId_set(_v_)    rsmp_context_cId_set(NULL, (_v_))
#define rsmp_context_global_xNId_set(_v_)   rsmp_context_xNId_set(NULL, (_v_))
#define rsmp_context_global_sId_set(_v_)    rsmp_context_sId_set(NULL, (_v_))

rsmp_context_t const *rsmp_context_global_get(void);
void rsmp_context_set(void *msg, rsmp_context_t const *item);

int rsmp_context_from_str(rsmp_context_t *item, const char *options);
#define rsmp_context_global_from_str(_o_)  rsmp_context_from_str(NULL, (_o_))
void rsmp_context_log(rsmp_context_t const *item);
#define rsmp_context_global_log() rsmp_context_log(NULL)

void rsmp_context_initialize(void);
/**@}*/
#endif //ENV_RSMP-CONTEXT_H