/**
* @file strings.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-06-30
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_UTILS_STRINGS_H
#define RSMP_UTILS_STRINGS_H
/**
 * @defgroup strings 
 * 
 * @brief 
 * 
 * @{
 */


char *rsmp_util_string_set(char **target, const char *value, const char *defvalue);
#define rsmp_util_string_value_set(target, value) rsmp_util_string_set(&target, value, NULL)
#define rsmp_util_string_defvalue_set(target, value) rsmp_util_string_set(&target, NULL, value)

int rsmp_str_cpy_dup(char const *value, char **str, size_t *len);

char *string_concat(char *dest, char const *src);

/**@}*/
#endif //UTILS_STRINGS_H