/**
* @file thread.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-05-23
* 
* @copyright Copyright (c) 2023
* 
*/
#define _GNU_SOURCE
#define MISC_THREAD_C
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/syscall.h>
#include <assert.h>

#include "log/log.h"

#include "thread.h"

/**
 * @ingroup thread
 * @{
 */
 
inline static void _pthread_initialize(void)
{
    static bool initialized=false;
    if (!initialized)
    {
        __my_tid =  syscall(SYS_gettid);
        initialized=true;
    }
}

void pthread_initialize(void)
{
    _pthread_initialize();
}

pthread_context_t * pthread_new(pthread_routine_t routine, char const *name)
{
    _pthread_initialize();
    static unsigned int no=0;
    assert(routine);
    pthread_context_t *t = calloc(1,sizeof(*t));
    if(t)
    {
        t->is_started = false;
        t->routine = routine;
        if (name) t->name = strdup(name);
        else      asprintf(&t->name,"thr-%04u", no++);
    }
    return t;
}



struct pthread_starter
{
    pthread_context_t *ptc;
    void *arg;    
};

static void *pthread_starter(void *arg)
{
    struct pthread_starter *c=arg;    
    pthread_context_t *ptc = c->ptc;
    ptc->is_started = true;
    __my_tid = ptc->tid = syscall(SYS_gettid);
    __my_name = ptc->name; 
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    void *ret = ptc->routine(c->arg); 
    ptc->is_started = false;
    free(arg);
    return ret;
}


int pthread_start(pthread_context_t *ptc, void *arg)
{
    _pthread_initialize();
    int ret = -1;
    if (ptc)
    {
        assert(ptc->routine);
        struct pthread_starter *c = calloc(1, sizeof(*c));
        if (c)
        {
            c->ptc=ptc;
            c->arg=arg;
            if ((ret=pthread_create(&ptc->pthread, NULL, pthread_starter, c)))
            {
                _log_errno("start thread %s failed", ptc->name);
                free(c);
            }
        }
        else
        {
            _log_errno("start thread %s failed", ptc->name);
        }

    }
    return ret;
}

void *_pthread_stop(pthread_context_t *ptc, bool by_cancel)
{
    void *ret = NULL;
    if (ptc && ptc->is_started)
    {
        if (by_cancel)
        {
            pthread_cancel(ptc->pthread);
        }
        else
        {
            pthread_join(ptc->pthread, &ret);
        }
        if (errno) _log_errno("stop %s failed (%s)", ptc->name, by_cancel?"cancel":"join");
    }
    return ret;
}


void pthread_free(pthread_context_t *ptc)
{
    if(ptc)
    {
        if (ptc->is_started) _pthread_stop(ptc, false);
        if (ptc->name) free(ptc->name);
        free(ptc);
    }
}




/**@}*/