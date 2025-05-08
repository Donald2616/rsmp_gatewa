/**
* @file auth-random.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-07
* 
* @copyright Copyright (c) 2024
* 
*/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "log/log.h"
#define AUTH_AUTH_RANDOM_C
#include "auth-random.h"


/**
 * @ingroup auth-random
 * @{
 */

ssize_t auth_random(char *dest, size_t len)
{
    assert(dest);
    ssize_t ret = -1;
    int fd;
    if ((fd = open("/dev/random", O_RDONLY)) < 0)
    {
        _log_errno("/dev/random");
    }
    else
    {
        ret = read(fd, dest, len);
        close(fd);
    }
    return ret;
}



/**@}*/