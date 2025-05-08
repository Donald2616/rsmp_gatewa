/**
* @file arraystr.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-06-28
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_VARS_ARRAYSTR_H
#define RSMP_VARS_ARRAYSTR_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "rsmp/vars/array.h"
/**
 * @defgroup arraystr 
 * 
 * @brief 
 * 
 * @{
 */

rsmp_array_declare(char);

#define rsmp_array_char_from_str(array, str) rsmp_array_char_from_str_len(array, str, strlen(str))

rsmp_array_char_t *rsmp_array_char_from_str_len(rsmp_array_char_t *array, char const *str, size_t len);

char *rsmp_array_char_to_str(rsmp_array_char_t *array, char **str, size_t len);

#define rsmp_array_char_declare(var) rsmp_array_char_t var = {.len=0, .items=NULL, .item_len=sizeof(char),};



/**@}*/
#endif //VARS_ARRAYSTR_H