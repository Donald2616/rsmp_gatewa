/**
* @file queue.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-05-10
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <errno.h>
#include <string.h>
#include <assert.h>

#include "lock/mutex.h"
#include "lock/sem.h"

#define SYNC_QUEUE_C
#include "log/log.h"
#include "ldc-mutex.h"
#include "queue.h"

/**
 * @ingroup queue
 * @{
 */



typedef ldc_mutex_t queue_msg_list_t;

/**
 * @brief Structure privé de la queue
 * 
 */
typedef struct queue_priv_t
{
    semaphore_t sem;            /*!< \brief le sémaphore de synchronisation */
    queue_msg_list_t list;      /*!< \brief la liste des messages en attente dans la queue */
} queue_priv_t;


/**
 * @brief Structure de définition d'uen entrée de queue 
 */
typedef struct queue_msg_entry_t
{
    sDblChainPrio   chain;    /*!< \brief chaînage des messages avec priorité et donc age */
    queue_msg_t     msg;      /*!< \brief contenu formatté du message */
} queue_msg_entry_t;

/**
 * @brief liste des entrées non utilisées
 * 
 */
static queue_msg_list_t _unused_msg = {
    .mutex = LOCK_MUTEX_INITIALIZER_REC,
    .root = {.next=&_unused_msg.root, .prev=&_unused_msg.root},
    .no = 0,
};

queue_msg_list_t *queue_msg_list_initialize(queue_msg_list_t *list)
{
    return ldc_mutex_initialize(list);
}

/**
 * @brief Insertion d'un message dans la queue
 * 
 * \note passe-plat vers ldc_mutex
 * 
 * @param list list des message en attente
 * @param item entrée à insérer
 * @return queue_msg_entry_t* le pointeur sur l'entrée insérée
 */
static inline queue_msg_entry_t *queue_msg_insert(queue_msg_list_t *list, queue_msg_entry_t *item)
{
    if (item==NULL) return NULL;
    ldc_mutex_insert(list, (sDblChain *)&item->chain);
    return item;
}

/**
 * @brief Extraction d'un message dans la queue
 * 
 * \note passe-plat vers ldc_mutex
 * 
 * 
 * @param list list des message en attente
 * @param item entrée à insérer
 * @return queue_msg_entry_t* le pointeur sur l'entrée extraite
 */
static inline queue_msg_entry_t *queue_msg_extract(queue_msg_list_t *list)
{
    queue_msg_entry_t *item = NULL;
    sDblChainPrio *node = (sDblChainPrio *)ldc_mutex_extract(list);
    if (node) item = container_of(node, queue_msg_entry_t, chain);
    return item;
}

/**
 * @brief Création d'un message de queue
 * 
 * La structure queue_msg_entry_t est extraite de la liste des
 * structure déjà allouée et plus utilisée.
 * Si de structure inutilisée, alors une allocation dynamique est 
 * réalisée.
 * 
 * @param buf     buffer des données représentant le payload du message,
 *                ces données doivent pouvoir êtr libérées par un _simple_ 
 *                appel à free()
 * @param length  longueur du buffer
 * @param prio    priorité du message, servira comme âge initial
 * @param mark    mark du producer ou autre rôle (transparent pour la communication)
 * @param clone   flag indiquant si le buf doit être cloné ou pas
 * @return queue_msg_entry_t* 
 */
static queue_msg_entry_t *queue_msg_new(char *buf, size_t length, bool flag_do_encrypt, queue_msg_prio_t prio, void const *mark, void const *attrib, bool clone)
{
    queue_msg_entry_t *item = queue_msg_extract(&_unused_msg);

    if (item == NULL) item = calloc(1, sizeof(*item));
    if (item != NULL)
    {
        ldcInitialize(&item->chain);
        item->msg.length = length;
        item->msg.mark = mark;
        item->msg.prio = prio;
        item->msg.flag_do_encrypt = flag_do_encrypt;
        item->msg.attrib = attrib;
        if (length == 0 || buf==NULL)
        {
            item->msg.buf = NULL;
        }
        else
        if (!clone)
        {
           item->msg.buf = buf;
        }
        else
        if ((item->msg.buf = calloc(1, length))!= NULL)
        {
            memcpy(item->msg.buf, buf, length);
        }
        else {
            free(item);
            item = NULL;
        }
    }
    return item;
}

/**
 * @brief recycle la structure du message
 * 
 * \attention le payload buf *n'est pas libéré* a ce moment là !
 * 
 * La structure de message est réinitialisée et remise 
 * dans la liste des structures inutilisées.
 * 
 * @param item l'entrée de message
 */
static void queue_msg_recycle(queue_msg_entry_t *item)
{
    if (!item) return;
    ldcSuppress(item);
    item->msg.buf = NULL;
    item->msg.length = 0;
    item->msg.flag_do_encrypt = true;
    item->msg.prio = 0;
    item->msg.mark = NULL;
    queue_msg_insert(&_unused_msg, item);
}

struct flush_context
{
    bool tobefree; /*!< \brief flag indiquant que le'entrée doit être recyclée */
};

/**
 * @brief Callback de flush
 * 
 * Sur demande les entrées sont recyclées ou libérées.
 * 
 * @param context 
 * @param node 
 */
static void queue_msg_list_flush_cb(void *context, sDblChain *node)
{
    queue_msg_entry_t *item = (queue_msg_entry_t *)node;//-- container_of(node, queue_msg_entry_t, chain);
    if (item->msg.buf) free(item->msg.buf);
    if (((struct flush_context *)context)->tobefree) free(item); 
    else queue_msg_recycle(item);
}

/**
 * @brief Vide les message d'une queue (liste)
 * 
 * Si la liste à vider est la liste des messages unitilisés, 
 * alors il est demandé de les libérer (free())
 * sinon ils sont recyclés.
 * 
 * @param list  liste à vider
 * @return ssize_t le nombre d'entrées supprimées
 */
ssize_t queue_msg_list_flush(queue_msg_list_t *list)
{
    struct flush_context context={.tobefree = (list == &_unused_msg)};
    return ldc_mutex_flush(list, queue_msg_list_flush_cb, &context);
}


inline static size_t queue_msg_no(queue_msg_list_t *list)
{
    return ldc_mutex_no(list);
}




static ssize_t __no(struct queue *this)
{
    assert(this != NULL);
    assert(this->dptr != NULL);
    queue_priv_t *priv = this->dptr;
    return ldc_mutex_no(&priv->list);
}

static ssize_t __pull(struct queue *this, queue_msg_t *msg, long timeout)
{
    assert(msg != NULL);
    assert(this != NULL);
    assert(this->dptr != NULL);
    ssize_t ret = -1;
    queue_priv_t *priv = this->dptr;

    errno=0;
    if (semaphore_wait(&priv->sem, timeout) == 0)
    {
        queue_msg_entry_t *item = queue_msg_extract(&priv->list);
        if (item)
        {
            *msg = item->msg;
            queue_msg_recycle(item);
            ret = 0;
        }
    }

    return ret;
}

static ssize_t __push(struct queue *this, queue_msg_t const *msg, bool clone)
{
    assert(this != NULL);
    assert(this->dptr != NULL);
    ssize_t ret=-1;
    queue_priv_t *priv = this->dptr;
    if (queue_msg_insert(&priv->list, queue_msg_new(msg->buf, msg->length, msg->flag_do_encrypt, msg->prio, msg->mark, msg->attrib, clone)))
    {
        semaphore_signal(&priv->sem);
        ret = 0;
    }
    return ret;
}

static ssize_t __flush(struct queue *this)
{
    assert(this != NULL);
    assert(this->dptr != NULL);
    queue_priv_t *priv = this->dptr;
    ssize_t ret = queue_msg_list_flush(&priv->list);
    semaphore_flush(&priv->sem);   
    return ret; 
}

/**
 * @brief Création des données privées d'une queue
 * 
 * @return queue_priv_t* structure privée allouée et intialisée
 */
static queue_priv_t *queue_priv_new(void)
{
     queue_priv_t *priv = NULL;
    if ((priv = calloc(1, sizeof(*priv))) != NULL)
    {
        semaphore_initialize(&priv->sem, 0);
        queue_msg_list_initialize(&priv->list);
    }
    return priv;
}

/**
 * @brief Libération d'une structure privée de queue
 * 
 * Effectue le flush de la liste puis libère la mémoire allouée
 * dynamiquement.
 * 
 * @param this la structure privée
 */
static void queue_priv_free(queue_priv_t *this)
{
    if (this)
    {
        queue_msg_list_flush(&this->list);
        free(this);
    }
}


static void __free(struct queue *this)
{
    assert(this != NULL);
    this->flush(this);
    queue_priv_free((queue_priv_t *)this->dptr);
    free(this);
}


queue_t *queue_initialize(queue_t *queue)
{
    assert(queue != NULL);
    queue_priv_t *priv = queue_priv_new();
    if (priv != NULL)
    {
        queue->dptr  = priv;
        queue->push  = __push;
        queue->pull  = __pull;
        queue->flush = __flush;
        queue->free  = __free;
        queue->no    = __no;
    }
    else
    {
        return NULL;
    }
    return queue;
}

queue_t *queue_new(void)
{
    queue_t *this = NULL;
    if ((this = calloc(1, sizeof(*this))) != NULL) 
    {
        if (queue_initialize(this) == NULL)
        {
            free(this);
            this = NULL;
        }
    }
    return this;
}

ssize_t queue_inout_init(queue_inout_t queues, queue_t *q0, queue_t *q1)
{
    ssize_t ret = 0;
    if (queues)
    {
        queues[0].flags=(q0==NULL);
        queues[1].flags=(q1==NULL);
        queues[0].queue=q0;
        queues[1].queue=q1;
        if (queues[0].queue==NULL)
        {
            if ((queues[0].queue=queue_new()) == NULL) ret = -1;
        }
        
        if (queues[1].queue==NULL)
        {
            if ((queues[1].queue=queue_new()) == NULL) ret = -1;
        }
        if (ret) 
            for (size_t i=0; i<2; i++)
            {
                if (queues[i].queue && queues[i].flags) queues[i].queue->free(queues[i].queue);
            }
    }
    else
    {
        ret = -1;
    }
    return ret;
}

queue_inout_t *queue_inout_new(queue_t *q0, queue_t *q1)
{
    queue_inout_t *this = calloc(1,sizeof(*this));
    if (this)
    {
        if (queue_inout_init(*this, q0, q1))
        {
            free(this);
            this = NULL;
        }
    }
    return this;
}

void queue_inout_flush(queue_inout_t queues)
{
    if (queues)
    {
        for (size_t i=0; i<2; i++)
        {
            if (queues[i].queue) queues[i].queue->flush(queues[i].queue);
        }
    }
}

void queue_inout_free_queue(queue_inout_t queues)
{
    if (queues)
    {   
        queue_inout_flush(queues);
        for (size_t i=0; i<2; i++)
        {
            if (queues[i].queue && queues[i].flags) queues[i].queue->free(queues[i].queue);
        }
    }
}

void queue_inout_free(queue_inout_t *this)
{
    if (this)
    {
        queue_inout_flush(*this);
        free(this);
    }
}

void queue_inout_clone(queue_inout_t des, queue_inout_t src)
{
    if (des && src)
    {
        queue_inout_init(des, src[0].queue, src[1].queue);
    }
}


void queue_destroy(void)
{
    _log_info("Cleaning %lu unused queue messages", ldc_mutex_no(&_unused_msg));
    queue_msg_list_flush(&_unused_msg);
}
/**@}*/