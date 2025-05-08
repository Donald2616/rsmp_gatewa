/**
* @file rsmp.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_RSMP_H
#define RSMP_FIELDS_RSMP_H
#include "rsmp/vars/array.h"
#include "rsmp/fields/vers.h"
/**
 * @defgroup arg 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_RSMP_C
#define RSMP_EXTERN
#define RSMP_PRIVATE
#endif

#ifndef RSMP_EXTERN
#define RSMP_EXTERN extern
#endif

#ifndef RSMP_PRIVATE
#define RSMP_PRIVATE const
#endif


typedef struct rsmp_msg_RSMP
{
    vers_t vers;
} RSMPs;

rsmp_array_declare_free(RSMPs);

// char *rsmp_array_RSMPs_to_string(rsmp_array_RSMPs_t const *item, char const *sep, char *out);
char * rsmp_array_RSMPs_to_string(rsmp_array_RSMPs_t const *item, char const *sep, char **out);
vers_t rsmp_array_RSMPs_check(rsmp_array_RSMPs_t const *local, rsmp_array_RSMPs_t const *remote);
rsmp_array_RSMPs_t *rsmp_array_RSMPs_dup2(rsmp_array_RSMPs_t *dest, rsmp_array_RSMPs_t const *src);

/**@}*/
#endif //FIELDS_RSMP_H