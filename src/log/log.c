/**
* @file log.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-05-15
* 
* @copyright Copyright (c) 2023
* 
*/
#define LOG_LOG_C
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "horodate/horodate.h"
#include "lock/mutex.h"

#include "misc/thread.h"
#include "log.h"

/**
 * @ingroup log
 * @{
 */

static char _log_level_str[sizeof(log_level_t)*8+1] = "";

#define log_level_check(l) if (((l)&log_level)==0) return 0

const log_level_t _log_mask = (log_level_t)(-1);

lock_mutex_t _log_mutex = LOCK_MUTEX_INITIALIZER_REC;
#define log_lock()  mutex_lock(&_log_mutex)
#define log_unlock()  mutex_unlock(&_log_mutex)


static char const *_log_set_str(void)
{
    static char const *mark="UIEWD";
    uint i=0;
    while(mark[i]!='\0')
    {
        _log_level_str[i]=(log_level&(1<<i))?mark[i]:'.';
        i++;
    }
    _log_level_str[i]='\0';
    return _log_level_str;
}

char const *_log_level_to_str(void)
{
    if (*_log_level_str=='\0') _log_set_str();
    return _log_level_str;
}


static inline int log_header(int dlog, char const *prefix, char const *section)
{
    HoroDate now;
    char dt[50];
    int ret=0;
    ret += dprintf(dlog,"%s %s (%04d)%-8.8s %5.5s ", strHorodate("%Y-%m-%d %H:%M:%S.#c", 0, dt, sizeof(dt), setHorodate(&now)), prefix, __my_tid, __my_name, section); 
    return ret;
}

int _log_dump(int dlog, log_level_t level, char const *sens, int err, char const *section, char const *buf, ssize_t len, char const *fformat, char const *function, char const *fname, int line,  char const *format, ...)
{
    int ret=0;
    log_level_check(level);
    log_lock();
    ret += log_header(dlog, sens, section);
    ret += dprintf(dlog, fformat, function, fname, line);
    ret += dprintf(dlog,"size:%ld ", len);
    if (err) ret += dprintf(dlog,"error: (%d) %s ", err, strerror(err));

    if (format)
    {
        va_list ap;

        va_start(ap, format);
        ret += vdprintf(dlog, format, ap);
        va_end(ap);
    }
    if (buf && len>0)
    {
        ret += dprintf(dlog," data: <");
        for (ssize_t i=0; i<len; i++)
        {
            ret += dprintf(dlog,isprint(buf[i])?"%c":"\\x%02hhx", buf[i]);
            // if (isprintable(*buf))  dprintf(dlog,"%c", *buf)
            // else                    dprintf(dlog,"\x%02hhu", *buf);
        }
        ret += dprintf(dlog,">\n");
    }
    log_unlock();
    return ret;
}

int _log_varg(int dlog, log_level_t level, char const *prefix, int err, char const *section, char const *format, va_list ap)
{
    int ret=0;
    log_level_check(level);
    log_lock();
    ret += log_header(dlog, prefix, section);
    ret += vdprintf(dlog,format, ap);
    if (err) ret += dprintf(dlog," : (%d) %s", err, strerror(err));
    ret += dprintf(dlog,"\n");
    log_unlock();
    return ret;
}

int _log(int fd, log_level_t level, char const *sens, int err, char const *section, char const *format, ...) 
{
    int ret = -1;
    log_level_check(level);
    va_list ap;
    va_start(ap, format);
    ret = _log_varg(fd, level, sens, err, section, format, ap);
    va_end(ap);
    return ret;
}


void _log_fd_set(int fd)
{
    log_lock();
    _log_fd = fd; 
    log_unlock();
}

void _log_fd_stdout(void)
{
    log_lock();
   _log_fd = fileno(stdout);  
    log_unlock();
}

void _log_level_set(log_level_t level)
{
    log_lock();
    log_level |= (level & _log_mask);
    _log_set_str();
    log_unlock();
}

void _log_level_clear(log_level_t level)
{
    log_lock();
    log_level &= (~level & _log_mask);
    _log_set_str();
    log_unlock();
}
/**@}*/