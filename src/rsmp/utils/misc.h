/**
* @file misc.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-05
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_UTILS_MISC_H
#define RSMP_UTILS_MISC_H
#include <stdlib.h>
#include <string.h>
/**
 * @defgroup misc 
 * 
 * @brief 
 * 
 * @{
 */


static inline uint rsmp_lib_find(char const *list[], size_t max, char const * str)
{
    if (str != NULL && *str != '\0')
    {
        for(uint i=0; i<max; i++)
        {
            if (strcmp(str, list[i])==0) return i;
        }
    }
    return max;
}


// int decodeparam(char *param, char sep, char ***argv);



/**@}*/
#endif //UTILS_MISC_H