/**
* @file ldc-mutex.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-05-11
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdlib.h>
#include <assert.h>

#include "log/log.h"
#define SYNC_LDC_MUTEX_C
#include "ldc-mutex.h"

/**
 * @ingroup ldc-mutex
 * @{
 */


sDblChain *ldc_mutex_insert_cmp(ldc_mutex_t *list, sDblChain *item, ldc_mutex_entry_cmp_t cmp)
{
    if (item==NULL) return NULL;
    assert(list != NULL);
    mutex_lock(&list->mutex);
    if (cmp != NULL) ldcInsertCompar(&list->root, item, cmp);
    else             ldcInsert(&list->root, item);
    list->no ++;
    mutex_unlock(&list->mutex);    
    return item;
}

sDblChain *ldc_mutex_find(ldc_mutex_t *list, ldc_mutex_entry_cmp_key_t cmp_key, void const *key)
{
    assert(list != NULL);
    assert(cmp_key != NULL);
    mutex_lock(&list->mutex);
    sDblChain *node;
    sDblChain *item=NULL;
    ldcForeach(node, &list->root)
    {
        if (cmp_key(key, node)==0) 
        {
            item = node;
            break;
        }
    }
    mutex_unlock(&list->mutex);    
    return item;
}

sDblChain *ldc_mutex_search(ldc_mutex_t *list, ldc_mutex_entry_cmp_key_t cmp_key, void const *key, ldc_mutex_update_t update, void *context, ldc_mutex_entry_cmp_t cmp)
{
    assert(list != NULL);
    assert(cmp_key != NULL);
    assert(key != NULL);
    assert(update != NULL);
    mutex_lock(&list->mutex);
    sDblChain *node;
    sDblChain *item = NULL;
    // _log_info("%s %p %lu", __FUNCTION__, list, list->no);

    ldcForeach(node, &list->root)
    {
        if (cmp_key(node, key)==0) 
        {
            item = node;
            //-- Supression momentanée
            node = node->prev;
            ldcSuppress(item);
            list->no--;
            break;
        }
    }
    
    if ((item = update(context, item)) != NULL)
    {
        if (ldcIsEmpty(item)) ldc_mutex_insert_cmp(list, item, cmp);
    }
    // _log_info("%s %p %lu", __FUNCTION__, list, list->no);
    mutex_unlock(&list->mutex);    
    return item;
}






sDblChain *ldc_mutex_extract(ldc_mutex_t *list)
{
    assert(list != NULL);
    sDblChain *item = NULL;
    mutex_lock(&list->mutex);
    if (!ldcIsEmpty(&list->root))
    {
        item = list->root.next;
        ldcSuppress(item);
        list->no --;
    }
    mutex_unlock(&list->mutex);    
    return item;
}



sDblChain *ldc_mutex_extract_by_key(ldc_mutex_t *list, ldc_mutex_entry_cmp_key_t cmp, void *key)
{
    assert(list != NULL);
    sDblChain *item = NULL;
    mutex_lock(&list->mutex);
    if ((item = ldc_mutex_find(list, cmp, key)) != NULL) ldcSuppress(item);
    mutex_unlock(&list->mutex);    
    return item;
}



void ldc_mutex_lock(ldc_mutex_t *list)
{
    assert(list != NULL);
    mutex_lock(&list->mutex);  
}



void ldc_mutex_unlock(ldc_mutex_t *list)
{
    assert(list != NULL);
    mutex_unlock(&list->mutex);  
}



ldc_mutex_t *ldc_mutex_initialize(ldc_mutex_t *list)
{
    assert(list != NULL);
    //-- Initialisation du mutex
    mutex_initialize_rec(&list->mutex);
    ldcInitialize(&list->root);
    list->no = 0;
    return list;
}

ldc_mutex_t *ldc_mutex_new(void)
{
    ldc_mutex_t *list = calloc(1, sizeof(*list));
    if (list) ldc_mutex_initialize(list);
    return list;
}
void ldc_mutex_free(ldc_mutex_t *list, ldc_mutex_entry_free_t cb, void *context)
{
    if (!list) return;
    //-- on bloque la liste ? pas cool si dans un signal...
    while (!ldcIsEmpty(&list->root))
    {
        sDblChain *node = list->root.next;
        ldcSuppress(node);
        if (cb) cb(context, node);
    }
    free(list);
}

static void ldc_mutex_cb_free(void *c, sDblChain *node)
{
    (void)c;
    if (node) free(node);
}

ssize_t ldc_mutex_flush(ldc_mutex_t *list, ldc_mutex_entry_free_t cb, void *context)
{
    assert(list != NULL);
    // assert(cb != NULL);
    if (!cb) cb = ldc_mutex_cb_free;
    ssize_t ret;
    mutex_lock(&list->mutex);
    while (!ldcIsEmpty(&list->root))
    {
        sDblChain *node = list->root.next;
        ldcSuppress(node);
        cb(context, node);
    }
    ret = list->no;
    list->no = 0;
    mutex_unlock(&list->mutex);    
    return ret;
}

ssize_t ldc_mutex_foreach(ldc_mutex_t *list, ldc_mutex_foreach_cb_t cb, void *context)
{
    assert(list != NULL);
    assert(cb != NULL);
    ssize_t ret = 0;
    mutex_lock(&list->mutex);
    sDblChain *node;
    ldcForeach(node, &list->root)
    {
        if (!cb(context, node)) break;
        ret ++;
    }
    mutex_unlock(&list->mutex);    
    return ret;
}

ssize_t ldc_mutex_no(ldc_mutex_t *list)
{
    assert(list != NULL);
    mutex_lock(&list->mutex);
    ssize_t no = list->no;
    mutex_unlock(&list->mutex);
    return no;
}


void ldc_mutex_suppress(ldc_mutex_t *list, sDblChain *item)
{
    assert(list != NULL);
    assert(item != NULL);
    mutex_lock(&list->mutex);

    //-- on suprime et on diminue
    ldcSuppress(item);
    list->no--;

    mutex_unlock(&list->mutex);
}

bool ldc_mutex_is_in(ldc_mutex_t *list, sDblChain *item)
{
    assert(list != NULL);
    assert(item != NULL);
    bool ret = false;
    mutex_lock(&list->mutex);

    sDblChain *n;
    ldcForeach(n, &list->root)
    {
        if (n==item) 
        {
            ret=true;
            break;
        }
    }

    mutex_unlock(&list->mutex);
    return ret;
}


/**@}*/