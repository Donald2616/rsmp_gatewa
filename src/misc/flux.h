/**
* @file flux.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-11
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef MISC_FLUX_H
#define MISC_FLUX_H
#include "flux/flux.h"
/**
 * @defgroup flux 
 * 
 * @brief ajout de méthodes aux flux
 * 
 * @{
 */

int fluxreadterm_with_realloc(FLUX *flux, char **buf, size_t *len, int to, char const *term, size_t after);

/**@}*/
#endif //MISC_FLUX_H