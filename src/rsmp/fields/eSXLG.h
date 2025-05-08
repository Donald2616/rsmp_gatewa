/**
* @file eSXLG.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_ESXLG_H
#define RSMP_FIELDS_ESXLG_H
#include "rsmp/vars/array.h"
#include "rsmp/fields/sg.h"
#include "rsmp/fields/vers.h"
/**
 * @defgroup eSXLG 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_ESXLG_C
#define ESXLG_EXTERN
#define ESXLG_PRIVATE
#endif

#ifndef ESXLG_EXTERN
#define ESXLG_EXTERN extern
#endif

#ifndef ESXLG_PRIVATE
#define ESXLG_PRIVATE const
#endif


typedef struct rsmp_msg_eSXLG
{
    vers_t vers;
    sg_t sg;
} eSXLGs;

rsmp_array_declare_free(eSXLGs);

rsmp_array_eSXLGs_t *rsmp_array_eSXLGs_dup2(rsmp_array_eSXLGs_t *dest, rsmp_array_eSXLGs_t const *src);

// vers_t rsmp_array_eSXLGs_check(rsmp_array_eSXLGs_t const *local, rsmp_array_eSXLGs_t const *remote);
char *rsmp_array_eSXLGs_to_string(rsmp_array_eSXLGs_t const *item, char const *sep, char **out);

/**@}*/
#endif //FIELDS_ESXLG_H