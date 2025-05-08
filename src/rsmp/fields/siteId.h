/**
* @file siteId.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_SITEID_H
#define RSMP_FIELDS_SITEID_H
#include "rsmp/vars/array.h"
#include "rsmp/fields/sId.h"
/**
 * @defgroup siteId 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_SITEID_C
#define SITEID_EXTERN
#define SITEID_PRIVATE
#endif

#ifndef SITEID_EXTERN
#define SITEID_EXTERN extern
#endif

#ifndef SITEID_PRIVATE
#define SITEID_PRIVATE const
#endif


typedef struct rsmp_msg_siteId
{
    sId_t sId;
} siteIds;

rsmp_array_declare_free(siteIds);

rsmp_array_siteIds_t *rsmp_array_siteIds_dup2(rsmp_array_siteIds_t *dest, rsmp_array_siteIds_t const *src);

char *rsmp_array_siteIds_to_string(rsmp_array_siteIds_t const *item, char const *sep, char **out);

/**@}*/
#endif //FIELDS_SITEID_H