/**
* @file sS-array.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-06
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"


#define RSMP_FIELDS_SS_ARRAY_C
#include "sS-array.h"

/**
 * @ingroup sS-array
 * @{
 */


void rsmp_array_sSs_free_item(sSs *item)
{
    if (item->sCI) free(item->sCI);
    if (item->n)   free(item->n);
    if (item->s)   free(item->s);
    if (item->uRt) free(item->uRt);
}



/**@}*/