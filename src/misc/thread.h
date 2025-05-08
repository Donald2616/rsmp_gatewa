/**
* @file thread.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-05-23
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef MISC_THREAD_H
#define MISC_THREAD_H
#ifdef MISC_THREAD_C
#define THREAD_PRIVATE
#define THREAD_EXTERN
#endif

#ifndef THREAD_PRIVATE
#define THREAD_PRIVATE const
#endif

#ifndef THREAD_EXTERN
#define THREAD_EXTERN extern
#endif

#include <stdbool.h>

/**
 * @defgroup thread interface simplifiée sur les pthreads
 * 
 * @brief 
 * 
 * @{
 */
typedef void *(*pthread_routine_t)(void *);

typedef struct pthread_context
{
    THREAD_PRIVATE pthread_t         pthread;
    THREAD_PRIVATE pthread_routine_t routine;
    THREAD_PRIVATE bool              is_started;
    THREAD_PRIVATE int               tid;
    THREAD_PRIVATE char *            name;
} pthread_context_t;

pthread_context_t * pthread_new(pthread_routine_t routine, char const *name);
void pthread_free(pthread_context_t *ptc);

int pthread_start(pthread_context_t *ptc, void *arg);


void *_pthread_stop(pthread_context_t *ptc, bool by_cancel);
#define pthread_stop_nowait(t) _pthread_stop(t, true)
#define pthread_stop(t)        _pthread_stop(t, false)

THREAD_EXTERN __thread THREAD_PRIVATE char const * __my_name
#ifdef MISC_THREAD_C
= "main"
#endif
;
THREAD_EXTERN __thread THREAD_PRIVATE int __my_tid
#ifdef MISC_THREAD_C
= 0
#endif
;

void pthread_initialize(void);

/**@}*/
#endif //MISC_THREAD_H