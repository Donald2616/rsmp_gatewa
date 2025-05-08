/**
* @file arg.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-06
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"


#define RSMP_FIELDS_ARG_C
#include "arg.h"

/**
 * @ingroup arg
 * @{
 */


void rsmp_array_args_free_item(args *item)
{
    if (item->cCI) free(item->cCI);
    if (item->n)   free(item->n);
    if (item->cO)  free(item->cO);
    rsmp_array_char_flush(&item->v);
}



/**@}*/