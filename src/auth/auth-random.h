/**
* @file auth-random.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-07
* 
* @copyright Copyright (c) 2024
* 
*/
#ifndef AUTH_AUTH_RANDOM_H
#define AUTH_AUTH_RANDOM_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * @defgroup auth-random 
 * 
 * @brief 
 * @ingroup auth
 * @{
 */


ssize_t auth_random(char *dest, size_t len);


/**@}*/
#endif //AUTH_AUTH-RANDOM_H