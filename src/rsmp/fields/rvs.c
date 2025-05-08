/**
* @file rvs.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-06
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"


#define RSMP_FIELDS_RVS_C
#include "rvs.h"

/**
 * @ingroup rvs
 * @{
 */

void rsmp_array_rvs_free_item(rvs *item)
{
    if (item->cCI) free(item->cCI);
    if (item->n)   free(item->n);
    rsmp_array_char_flush(&item->v);
}




/**@}*/