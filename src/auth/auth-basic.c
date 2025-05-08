/**
* @file auth.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-06
* 
* @copyright Copyright (c) 2024
* 
*/
#define _GNU_SOURCE
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <search.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

#include "lstdblcyc/lstdblcyc.h"
#include "lock/rwlock.h"
#include "ioprintf/ioprintf.h"

#define AUTH_PROTECT_C
#define AUTH_AUTH_BASIC_C
#include "auth.h"
#include "auth-pwd.h"
#include "auth-basic.h"
#include "crypt/bcd.h"

/**
 * @ingroup auth-basic
 * @{
 */

typedef struct auth_lst_lock_t
{
    sDblChain root;
    rwlock_t  lock;
} auth_lst_lock_t;

#define _auth_dbs_root _auth_dbs.root
static auth_lst_lock_t _auth_dbs =
{
    .root={.next=&_auth_dbs_root, .prev=&_auth_dbs_root},
    .lock = RWLOCK_INITIALIZER,
};

static void auth_sIds_purge(sDblChain *root);
static void auth_logins_purge(sDblChain *root);
static void auth_secrets_purge(sDblChain *root);

// #define _rwlock_write(a) do { _log_info("[X] <=  Thread %d tente de prendre le wrlock... (%p)\n", gettid(), a); if(0!=rwlock_write_nowait(a)) _log_info("Thread %d: WRLOCK bloqué, quelqu'un a un RDLOCK (%p)", gettid(), a);}while(0)
// #define _rwlock_read(a) do {   rwlock_read(a); _log_info("[x] <- Thread %d a pris un rdlock (%p)", gettid(), a); } while(0)
// #define _rwlock_release(a) do { rwlock_release(a); _log_info("[.] -> Thread %d a libéré le lock (%p)", gettid(), a); } while(0)

#define _rwlock_write(a) do { if(0!=rwlock_write_nowait(a)) _log_info("Thread %d: WRLOCK bloqué, quelqu'un a un RDLOCK (%p)", gettid(), a);}while(0)
#define _rwlock_read(a) do {   rwlock_read(a);  } while(0)
#define _rwlock_release(a) do { rwlock_release(a);  } while(0)


/**
 * @brief fonction générique de recherche dsn une liste double circulaire
 * 
 * \remark le cast de sortie est à la charge de l'appelant
 * 
 * @param lock verrou de protection
 * @param root racine de la liste
 * @param key  clé de recherche
 * @param cmp  fonction de comparaison entre un nœud de la liste et la référence
 * @return void* NULL si pas trouvé, l'adresse de l'item sinon
 */
static inline void *auth_find(rwlock_t *lock, sDblChain *root, void const *key, void *(*cmp)(sDblChain const *node, void const *key))
{
    assert(cmp);
    assert(root);
    void *item = NULL;
    sDblChain *node;
    if (!key) return NULL;

    if (lock) _rwlock_read(lock);
    if (!ldcIsEmpty(root))
    {
        ldcForeach(node, root)
        {
            item = cmp(node, key);
            if (item) break;
        }
    }
    if (lock) _rwlock_release(lock);

    return item;
}


/*

         88  88              88           88
         88  88              88           ""               ,d
         88  88              88                            88
 ,adPPYb,88  88,dPPYba,      88           88  ,adPPYba,  MM88MMM
a8"    `Y88  88P'    "8a     88           88  I8[    ""    88
8b       88  88       d8     88           88   `"Y8ba,     88
"8a,   ,d88  88b,   ,a8"     88           88  aa    ]8I    88,
 `"8bbdP"Y8  8Y"Ybbd8"'      88888888888  88  `"YbbdP"'    "Y888


*/
/**
 * \brief description d'une entrée dbId
*/
typedef struct auth_db_t
{
    auth_dbId_t dbId;  /*!< \brief référence de l'entrée (clé primaire unique) */
    sDblChain chain;   /*!< chaînage des entrées dbId */
    sDblChain sIds;    /*!< chaînage des entrée sId de la base de données */
    rwlock_t  lock;    /*!< verrou de protection */
} auth_db_t;

/**
 * @brief récupération de l'entrée dbId à partir du champ chain
 * 
 * @param a l'adresse du champ chain dans la structure
 */
#define auth_db_by_chain(a) container_of(a, auth_db_t, chain)

/**
 * @brief récupération de l'entrée dbId à partir du champ  sIds
 * 
 * @param a l'adresse du champ chain dans la structure
 */
#define auth_db_by_sIds(a) container_of(a, auth_db_t, sIds)

/**
 * @brief création d'une entité dbId
 * 
  * @param dbId la référence
 * @return auth_db_t* 
 */
static auth_db_t *auth_db_new(auth_dbId_t dbId)
{
    auth_db_t *item = calloc(1, sizeof(*item));
    if (item)
    {
        ldcInitialize(&item->chain);
        ldcInitialize(&item->sIds);
        item->dbId = strdup(dbId);
        rwlock_initialize(&item->lock);
    }
    return item;
}

/**
 * @brief libération d'une entité dbId et de ses dépendances
 * 
 * @param item l'entrée dbId à supprimer
 */
static void auth_db_free(auth_db_t *item)
{
    if (item)
    {
        ldcSuppress(&item->chain);
        if (!ldcIsEmpty(&item->sIds)) auth_sIds_purge(&item->sIds);
        if (item->dbId) free(item->dbId);
    }
}

/**
 * @brief Purge d'une entrée dbId
 * 
 * La purge est récursive, elle supprime toutes les références
 * qui s'y rattache
 * 
 */
static void auth_db_purge(void)
{
    _rwlock_write(&_auth_dbs.lock);
    while(!ldcIsEmpty(&_auth_dbs_root)) 
    {
        auth_db_free(auth_db_by_chain(_auth_dbs_root.next));
    }
    _rwlock_release(&_auth_dbs.lock);
}

/**
 * @brief Comparaison d'un nœud et d'une référence dbId
 * 
 * @param node le nœud (pointeur sur sDblChain)
 * @param key  la référence attendue
 * @return void* NULL si différent, l'item sinon
 */
static void *auth_db_cmp(sDblChain const *node, void const *key)
{
    if (!node || !key) return NULL;
    auth_db_t *item = auth_db_by_chain(node);
    return (strcmp(item->dbId, (auth_dbId_t)key)==0)?item:NULL;
}


/**
 * @brief recherche une entrée dbId
 * 
 * Raccourci sur la fonction commune de recherche auth_find()
 * 
 * @param lock verrou de protection
 * @param key  identifiantsId attendu
 * @return auth_db_t* 
 */#define auth_db_find(lock, key) (auth_db_t *)auth_find(lock, &_auth_dbs_root, key, auth_db_cmp)

static auth_db_t *auth_db_search(auth_dbId_t dbId)
{
    auth_db_t *item = auth_db_find(&_auth_dbs.lock, dbId);
    if (!item) 
    {
        item = auth_db_new(dbId);
        if (item)
        {
            _rwlock_write(&_auth_dbs.lock);
            ldcInsert(&_auth_dbs_root, &item->chain);
            _rwlock_release(&_auth_dbs.lock);
        }
    }
    return item;
}



/*

           88           88     88           88
           88           88     88           ""               ,d
           88           88     88                            88
,adPPYba,  88   ,adPPYb,88     88           88  ,adPPYba,  MM88MMM
I8[    ""  88  a8"    `Y88     88           88  I8[    ""    88
 `"Y8ba,   88  8b       88     88           88   `"Y8ba,     88
aa    ]8I  88  "8a,   ,d88     88           88  aa    ]8I    88,
`"YbbdP"'  88   `"8bbdP"Y8     88888888888  88  `"YbbdP"'    "Y888


*/

/**
 * @brief entrée sId
 * 
 */
typedef struct auth_sIds_t
{
    auth_sId_t sId;       /*!< \brief référence sId */
    sDblChain chain;      /*!< \brief chaînage des entrées sId pour une base de données */
    auth_db_t *to_dbId;   /*!< \brief pointeur sur l'entrée dbId corespondante */
    sDblChain logins;     /*!< \brief liste des entrée (login) associé */
    rwlock_t  lock;       /*!< \brief verrou de protection */
} auth_sIds_t;

/**
 * @brief récupération de l'entrée sId à partir du chaînage
 * 
 * @param c l'adresse du champ chain
 */
#define sIds_by_chain(c) container_of(c, auth_sIds_t, chain) 

/**
 * @brief récupération de l'entrée sId à partir de la liste des login
 * 
 * @param c l'adresse du champ logins
 */
#define sIds_by_login(c) container_of(c, auth_sIds_t, logins) 

/**
 * @brief création d'une entrée sId
 * 
 * @param sId la clé sId
 * @return auth_sIds_t* NULL si échec
 */
static auth_sIds_t *auth_sId_new(auth_sId_t sId)
{
    auth_sIds_t *item = calloc(1, sizeof(*item));
    if (item)
    {
        ldcInitialize(&item->chain);
        // ldcInitialize(&item->by_dbId);
        ldcInitialize(&item->logins);
        rwlock_initialize(&item->lock);
        item->sId = strdup(sId);
    }
    return item;
}

/**
 * @brief libère une entrée sId
 * 
 * la libération de l'entrée entraîne la libération de toute les entrées 
 * qui s'y rattachent
 * 
 * @param item l'entrée sId à supprimer
 */
static void auth_sId_free(auth_sIds_t *item)
{
    if (item)
    {
        _rwlock_write(&item->lock);
        ldcSuppress(&item->chain);
        // ldcInitialize(&item->by_dbId);
        if (!ldcIsEmpty(&item->logins)) auth_logins_purge(&item->logins);
        if (item->sId) free(item->sId);
        _rwlock_release(&item->lock);
        free(item);
    }
}

/**
 * @brief purge une liste de sId
 * 
 * La purge est récursive.
 * 
 * @param root racine de la liste
 */
static void auth_sIds_purge(sDblChain *root)
{
    while(!ldcIsEmpty(root)) 
    {
        auth_sId_free(sIds_by_chain(root->next));
    }
}

/**
 * @brief COmparaison de sId
 * 
 * @param node nœud représentant l'entrée auth_sId_t
 * @param key  la clé de référence
 * @return void* l'entrée sId si trouvée, NULL sinon
 */
static void *auth_sId_cmp(sDblChain const *node, void const *key)
{
    if (!node || !key) return NULL;
    auth_sIds_t *item = sIds_by_chain(node);
    if (strcmp(item->sId, (auth_sId_t)key)==0)
    {
        return item;
    }
    return NULL;
}

/**
 * @brief recherche une entrée sId
 * 
 * Raccourci sur la fonction commune de recherche auth_find()
 * 
 * @param lock verrou de protection
 * @param root racine de recherche
 * @param key  identifiantsId attendu
 * @return auth_sIds_t* 
 */
#define auth_sId_find(lock, root, key) (auth_sIds_t *)auth_find(lock, root, key, auth_sId_cmp)

/**
 * @brief trouve ou insère une entrée sId
 * 
 * @param lock verrou de protection
 * @param root racine de recherche
 * @param key  identifiantsId attendu
 * @return auth_sIds_t* 
 */
static auth_sIds_t *auth_sId_search(rwlock_t *lock, sDblChain *root, auth_sId_t key)
{
    if (!root) return NULL;
    auth_sIds_t *item = auth_sId_find(lock, root, key);
    if (!item) 
    {
        item = auth_sId_new(key);
        if (item)
        {
            _rwlock_write(lock);
            ldcInsert(root, &item->chain);
            _rwlock_release(lock);
            item->to_dbId = auth_db_by_sIds(root);
        }
    }
    return item;
}


/*

                                                            88              88           88
                                                     ,d     88              88           ""               ,d
                                                     88     88              88                            88
                          ,adPPYYba,  88       88  MM88MMM  88,dPPYba,      88           88  ,adPPYba,  MM88MMM
                          ""     `Y8  88       88    88     88P'    "8a     88           88  I8[    ""    88
                          ,adPPPPP88  88       88    88     88       88     88           88   `"Y8ba,     88
                          88,    ,88  "8a,   ,a88    88,    88       88     88           88  aa    ]8I    88,
                          `"8bbdP"Y8   `"YbbdP'Y8    "Y888  88       88     88888888888  88  `"YbbdP"'    "Y888

888888888888  888888888888
*/

/**
 * @brief structure privée d'une entrée
 * 
 */
typedef struct __auth_t
{
    auth_t auth;           /*!< \brief vision publique de l'entrée */
    auth_sIds_t *to_sId;   /*!< \brief pointeur vers la liste des sId d'appartenance*/
    sDblChain chain;       /*!< \ brief chaînage principal (dans le même sId) */
    sDblChain secrets;     /*!< \brief liste des clefs associées */
    rwlock_t lock;
} __auth_t;

/**
 * @brief récupération de l'entrée à partir de 
 *        l'adresse de chaînage principal
 * 
 * @param c adresse du champ chain
 */
#define __auth_by_chain(c) container_of(c, __auth_t, chain) 
/**
 * @brief récupération de l'entrée à partir de l'adreses publique
 * 
 * @param c adresse de la structure auth oublique
 */
#define __auth_by_auth(c) container_of(c, __auth_t, auth) 

/**
 * @brief création d'une entrée
 * 
 * @param body corps des donnes publiques
 * @return __auth_t* 
 */
static __auth_t *__auth_new(auth_t *body)
{
    __auth_t *item = calloc(1, sizeof(*item));
    if (item)
    {
        ldcInitialize(&item->chain);
        ldcInitialize(&item->secrets);
        item->auth = *body;
        rwlock_initialize(&item->lock);
    }
    return item;
}

/**
 * @brief libération d'une entrée
 * 
 * @param lock verrou de protection
 * @param item entrée à supprimer
 */
static inline void __auth_free(rwlock_t *lock, __auth_t *item)
{
    if(lock) _rwlock_write(lock);
    if (item)
    {
        _rwlock_write(&item->lock);
        if (!ldcIsEmpty(&item->chain)) ldcSuppress(&item->chain);
        if (item->auth.key.login) free(item->auth.key.login);
        if (item->auth.key.sId) free(item->auth.key.sId);
        auth_pwd_release(&item->auth.pwd);
        auth_secrets_purge(&item->secrets);
        free(item);
    }
    if(lock) _rwlock_release(lock);
}

/**
 * @brief purge d'une liste d'entrée
 * 
 * @param root racine à purger
 */
static void auth_logins_purge(sDblChain *root)
{
    assert(root);
    while(!ldcIsEmpty(root))
    {
        auth_free(&((__auth_by_chain(root->next))->auth));
    }
}

/**
 * @brief comparaison de login
 * 
 * @param node nœud qui rerésente l'entrée
 * @param key  clé de référence
 * @return void* l'entrée si OK NULL sinon
 */
static void *auth_login_cmp(sDblChain const *node, void const *key)
{
    if (!node || !key) return NULL;
    __auth_t *item = __auth_by_chain(node);
    return (strcmp(item->auth.key.login, (auth_login_t)key)==0)?item:NULL;
}


/**
 * @brief recherche une entrée
 * 
 * Raccourci sur la fonction commune de recherche auth_find()
 * 
 * @param lock verrou de protection
 * @param root racine de recherche
 * @param key  identifiantsId attendu
 * @return __auth_t* 
 */
#define auth_login_find(lock, root, key) (__auth_t *)auth_find(lock, root, key, auth_login_cmp)


/**
 * @brief insertion d'une nouvelle entrée
 * 
 * @param lock verrou de protection
 * @param root racine de a liste
 * @param item entrée à insérer
 * @return __auth_t* 
 */
static __auth_t *auth_login_insert(rwlock_t *lock, sDblChain *root, __auth_t *item)
{
    if (root && item)
    {
        if (lock) _rwlock_write(lock);
        ldcInsert(root, &item->chain);
        item->to_sId = sIds_by_login(root);
        if (lock) _rwlock_release(lock);
    }
    return item;
}



/*

  ,ad8888ba,   88888888ba   88        88  88888888ba,
 d8"'    `"8b  88      "8b  88        88  88      `"8b
d8'            88      ,8P  88        88  88        `8b
88             88aaaaaa8P'  88        88  88         88
88             88""""88'    88        88  88         88
Y8,            88    `8b    88        88  88         8P
 Y8a.    .a8P  88     `8b   Y8a.    .a8P  88      .a8P
  `"Y8888Y"'   88      `8b   `"Y8888Y"'   88888888Y"'


*/



static auth_t *auth_insert(const auth_dbId_t dbId, __auth_t *item)
{
    //-- Trouve la db
    __auth_t *ret = NULL;
    auth_db_t *db = auth_db_search(dbId);
    if (db)
    {
    //-- Trouve le sId
       auth_sIds_t *sIds =  auth_sId_search(&db->lock, &db->sIds, item->auth.key.sId);
       if (sIds) 
       {
            if (!(ret = auth_login_find(&sIds->lock, &sIds->logins, item->auth.key.login)))
            {
                ret = auth_login_insert(&sIds->lock, &sIds->logins, item);
            }
            else
            {
                _log_err("%s:%s:%s: already exists, insert aborted!", ret->auth.key.dbId, ret->auth.key.login, ret->auth.key.sId);
                ret = NULL;
            }
       }
    }
    return ret?&ret->auth:NULL;
}


auth_t *auth_insert_or_update(auth_dbId_t dbId, __auth_t *item)
{
    //-- Trouve la db
    __auth_t *ret = NULL;
    auth_db_t *db = auth_db_search(dbId);
    if (db)
    {
    //-- Trouve le sId
       auth_sIds_t *sIds =  auth_sId_search(&db->lock, &db->sIds, item->auth.key.sId);
       if (sIds) 
       {
            ret = auth_login_find(&sIds->lock, &sIds->logins, item->auth.key.login);
            _rwlock_write(&sIds->lock);
            if (ret) __auth_free(NULL, ret);
            ret = auth_login_insert(NULL, &sIds->logins, item);
            _rwlock_release(&sIds->lock);
       }
    }
    return ret?&ret->auth:NULL;
}


auth_t * auth_find_by_key(const auth_key_t *key)
{
    if (!key) return NULL;
    //-- Trouve la db
    __auth_t *item = NULL;
    _rwlock_read(&_auth_dbs.lock);
    auth_db_t *db = auth_db_find(NULL, key->dbId);
    if (db)
    {
        //-- Trouve le sId
        auth_sIds_t *sIds =  auth_sId_find(&db->lock, &db->sIds, key->sId);
        if (sIds) 
        {
            item = auth_login_find(&sIds->lock, &sIds->logins, key->login);
        }
    }
    _rwlock_release(&_auth_dbs.lock);
    return item?&item->auth:NULL;
}


auth_t *auth_insert_by_key(auth_key_t const *key)
{
    if (
           !key->dbId  || *key->dbId=='\0' 
        || !key->login || *key->login=='\0'
        || !key->sId   || *key->sId=='\0'
        ) return NULL;
    //-- Trouve la db
    auth_db_t *db = auth_db_search(key->dbId);
    if (db)
    {
    //-- Trouve le sId
       auth_sIds_t *sIds =  auth_sId_search(&db->lock, &db->sIds, key->sId);
       if (sIds) 
       {
           if (!auth_login_find(&sIds->lock, &sIds->logins, key->login)) 
           {
                auth_t *item = auth_new(key->dbId, key->login, key->sId, 0, NULL, NULL, false, NULL, NULL, false);
                return item;
           }
           else
           {
                _log_err("%s:%s:%s already exists, abort insert", key->dbId, key->login, key->sId);
           }
       }
    }
    return NULL;
}


auth_t *auth_search_by_key(auth_key_t const *key)
{
    __auth_t *ret = NULL; 
    if (
           !key->dbId  || *key->dbId=='\0' 
        || !key->login || *key->login=='\0'
        || !key->sId   || *key->sId=='\0'
        ) return NULL;
    //-- Trouve la db
    auth_db_t *db = auth_db_search(key->dbId);
    if (db)
    {
    //-- Trouve le sId
       auth_sIds_t *sIds =  auth_sId_search(&db->lock, &db->sIds, key->sId);
       if (sIds) 
       {
           ret = auth_login_find(&sIds->lock, &sIds->logins, key->login);
           if (!ret) return auth_new(key->dbId, key->login, key->sId, 0, NULL, NULL, false, NULL, NULL, false);
       }
    }
    return ret?&ret->auth:NULL;
}


auth_t *auth_new(const auth_dbId_t dbId, const auth_login_t login, const auth_sId_t sId, size_t pwd_len, char const *pwd, HoroDate const *ts, bool active, size_t new_pwd_len, char const *new_pwd, bool new_pwd_pending)
{
    assert(login);
    assert(*login);
    assert(sId);
    assert(*sId);
    auth_t body = {.pwd.buf = NULL, .pwd.len=0, .new_pwd.buf=NULL, .new_pwd.len=0};

    body.key.dbId = strdup(dbId);
    body.key.login = strdup(login);
    body.key.sId = strdup(sId);
    if (ts) body.timestamp = *ts;
    else    setHorodate(&body.timestamp);

    if (pwd_len > 0 && pwd)
    {
        body.pwd.len = pwd_len;
        body.pwd.buf = malloc(body.pwd.len);
        if (body.pwd.buf) memcpy(body.pwd.buf, pwd, pwd_len);
        else
        {
            _log_errno("failed to allocate pwd!");
            body.pwd.len = 0;
        }
    }

    // idem pour new_pwd
    if (new_pwd_len > 0 && new_pwd)
    {
        body.new_pwd.len = new_pwd_len;
        body.new_pwd.buf = malloc(body.new_pwd.len);
        if (body.new_pwd.buf) memcpy(body.new_pwd.buf, new_pwd, new_pwd_len);
        else
        {
            _log_errno("failed to allocate new_pwd!");
            body.new_pwd.len = 0;
        }
    }

    body.active = (body.pwd.len && body.pwd.buf) ? active : false;
    body.new_pwd_pending = (body.new_pwd.len && body.new_pwd.buf) ? new_pwd_pending : false;

    __auth_t *item = __auth_new(&body);
    if (item)
    {
        if (auth_insert(dbId, item))
        {
            char dt[50];
            _log_info("db: %s, login: %s, sId: %s, pwd: xxxxxx, active: %s, timestamp: %s, new_pwd: xxxxxx, new_pwd_pending: %s", 
                item->to_sId->to_dbId->dbId, item->auth.key.login, item->auth.key.sId, 
                item->auth.active?"true":"false", auth_strHorodate(dt, sizeof(dt), &item->auth.timestamp), item->auth.new_pwd_pending?"true":"false");
        }
    }
    return item?&item->auth:NULL;
}

void auth_free(auth_t *ptr)
{
    __auth_t *item =container_of(ptr, __auth_t, auth);
    if (item)
    {
        __auth_free(item->to_sId?&item->to_sId->lock:NULL, item);
    }
}

void auth_delete_by_key(auth_key_t const *key)
{
    auth_t *item = auth_find_by_key(key);
    if (item)
    {
        _log_info("%s:%s:%s deleted", item->key.dbId, item->key.login, item->key.sId);
        auth_free(item);        
    }
}


static inline auth_dbId_t auth_db_id_get(auth_t *ptr)
{
    __auth_t *auth=__auth_by_auth(ptr);
    return (auth && auth->to_sId && auth->to_sId->to_dbId)?auth->to_sId->to_dbId->dbId:NULL;
}

bool auth_db_pwd_change(auth_t *auth, char *buf, size_t len)
{
    if (!auth) return false;
    bool ret = false;
    auth_pwd_t pwd={.len=0, .buf=NULL};
    if (auth_pwd_set(&pwd, buf, len))
    {
        char *pwd_old=NULL; 
        char *pwd_new=NULL;
        auth_pwd_hash_bdc(&auth->pwd, &pwd_old); 
        auth_pwd_hash_bdc(&pwd, &pwd_new); 
        auth_pwd_cpy(&auth->pwd, &pwd);
        setHorodate(&auth->timestamp);
        _log_info("%s:%s:%s, change pwd from %s, to %s", auth->key.dbId, auth->key.login, auth->key.sId, pwd_old, pwd_new);
        auth_pwd_release(&pwd);
        if (pwd_old) free(pwd_old);
        if (pwd_new) free(pwd_new);
        ret = true;
    }
    else
    {

    }
    return ret;
}


/*

88888888888  ,ad8888ba,    88888888ba   88888888888         db         ,ad8888ba,   88        88
88          d8"'    `"8b   88      "8b  88                 d88b       d8"'    `"8b  88        88
88         d8'        `8b  88      ,8P  88                d8'`8b     d8'            88        88
88aaaaa    88          88  88aaaaaa8P'  88aaaaa          d8'  `8b    88             88aaaaaaaa88
88"""""    88          88  88""""88'    88"""""         d8YaaaaY8b   88             88""""""""88
88         Y8,        ,8P  88    `8b    88             d8""""""""8b  Y8,            88        88
88          Y8a.    .a8P   88     `8b   88            d8'        `8b  Y8a.    .a8P  88        88
88           `"Y8888Y"'    88      `8b  88888888888  d8'          `8b  `"Y8888Y"'   88        88


*/



static int auth_foreach_login(rwlock_t *lock, sDblChain *root, int(fct)(void *context, const auth_dbId_t dbId, auth_t const *auth), void *context)
{
    int ret = 0;
    sDblChain *node;
    if (lock) _rwlock_read(lock);
    ldcForeach(node, root)
    {
        __auth_t *item =  __auth_by_chain(node);
        if (!fct(context, item->to_sId->to_dbId->dbId, &item->auth)) break;
        ret++;
    }        
    if (lock) _rwlock_release(lock);
    return ret;
}

static int auth_foreach_sId(rwlock_t *lock, sDblChain *root, const auth_sId_t psId, int(fct)(void *context, const auth_dbId_t dbId, auth_t const *auth), void *context)
{
    int ret = 0;
    if (lock) _rwlock_read(lock);
    if (psId)
    {
        auth_sIds_t *sId = auth_sId_find(lock, root, psId);
        if (sId) ret = auth_foreach_login(&sId->lock, &sId->logins, fct, context);
    }
    else
    {
        sDblChain *node;
        ldcForeach(node, root)
        {
            auth_sIds_t *sId =  sIds_by_chain(node);
            ret += auth_foreach_login(&sId->lock, &sId->logins,fct, context);
        }        
    }
    if (lock) _rwlock_release(lock);
    return ret;
}


int auth_foreach(const auth_dbId_t dbId, const auth_sId_t sId, int(fct)(void *context, const auth_dbId_t dbId, auth_t const *auth), void *context)
{
    int ret = 0;
    _rwlock_read(&_auth_dbs.lock);
    if (dbId)
    {
        auth_db_t *db = auth_db_find(NULL, dbId);
        if (db) ret= auth_foreach_sId(&db->lock, &db->sIds, sId, fct, context);
    }
    else
    {
        sDblChain *node;
        ldcForeach(node, &_auth_dbs_root)
        {
            auth_db_t *db =  auth_db_by_chain(node);
            ret += auth_foreach_sId(&db->lock, &db->sIds, sId, fct, context);
        }
    }
    _rwlock_release(&_auth_dbs.lock);
    return ret;
}


auth_t *auth_get_first(auth_key_t const *key)
{
    assert(key);
    __auth_t *item = NULL;

    if (!ldcIsEmpty(&_auth_dbs_root))
    {
        auth_db_t *db = NULL;
       
        if (key->dbId) db = auth_db_find(&_auth_dbs.lock, key->dbId); 
        else           db = auth_db_by_chain(_auth_dbs_root.prev);

        if (db)
        {
            auth_sIds_t *sId = NULL;
            if (!ldcIsEmpty(&db->sIds))
            {
                if (key->sId)  sId = auth_sId_find(&db->lock, &db->sIds, key->sId);
                else           sId = sIds_by_chain(db->sIds.prev);
                if (sId && !ldcIsEmpty(&sId->logins)) item = __auth_by_chain(sId->logins.prev);
            }
        }
    }
    return item?&item->auth:NULL;;
}

// donne l'auth suivant et de manière circulaire
auth_t *auth_get_next_from_auth(auth_t const *_auth)
{
    assert(_auth);
    __auth_t *item = __auth_by_auth(_auth);

    // est-ce que l'objet est décroché ?
    assert(item->chain.next);
    assert(item->to_sId->sId);

    if(item->chain.next != &item->to_sId->logins) { // le suivant pointe sur un element et pas sur une tete de liste
        _log_info("from __auth_t %p to __auth_t %p", item, (__auth_t *)(__auth_by_chain(item->chain.next)));
         return &((__auth_t *)(__auth_by_chain(item->chain.next)))->auth;
    } else { // tête de liste
        // on va chercher le suivant de la tete de liste et vérifier que ce n'est pas le même noeud
        if(__auth_by_chain(item->to_sId->logins.next) != item) {
            _log_info("from last __auth_t %p to __auth_t %p (saute la tete de liste)", item, item->chain.next->next);
            return &((__auth_t *)(__auth_by_chain(item->to_sId->logins.next)))->auth;
        }
    }

    return NULL;
}



/*

 ad88888ba   88888888888  88888888ba   88         db         88           88   ad88888ba         db    888888888888  88    ,ad8888ba,    888b      88
d8"     "8b  88           88      "8b  88        d88b        88           88  d8"     "8b       d88b        88       88   d8"'    `"8b   8888b     88
Y8,          88           88      ,8P  88       d8'`8b       88           88  Y8,              d8'`8b       88       88  d8'        `8b  88 `8b    88
`Y8aaaaa,    88aaaaa      88aaaaaa8P'  88      d8'  `8b      88           88  `Y8aaaaa,       d8'  `8b      88       88  88          88  88  `8b   88
  `"""""8b,  88"""""      88""""88'    88     d8YaaaaY8b     88           88    `"""""8b,    d8YaaaaY8b     88       88  88          88  88   `8b  88
        `8b  88           88    `8b    88    d8""""""""8b    88           88          `8b   d8""""""""8b    88       88  Y8,        ,8P  88    `8b 88
Y8a     a8P  88           88     `8b   88   d8'        `8b   88           88  Y8a     a8P  d8'        `8b   88       88   Y8a.    .a8P   88     `8888
 "Y88888P"   88888888888  88      `8b  88  d8'          `8b  88888888888  88   "Y88888P"  d8'          `8b  88       88    `"Y8888Y"'    88      `888


*/

struct auth_serialize_t {
    int fd;
    uint no;
};


void log_auth_info(int fd, int no, const char *dbId, const char *login, const char *sId, const char *pwd, int active, const char *timestamp, const char *newPwd, int new_pwd_pending) {
    // Get the current date and time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_str[100];
    strftime(date_str, sizeof(date_str)-1, "%Y-%m-%d %H:%M:%S", t);

    // Construct the command string
    char command[1024];
    snprintf(command, sizeof(command),
             "echo '[%s] %d;%s;%s;%s;$0$%s;%d;%s;%s;%d' >> auth.db.log",
             date_str, no, dbId, login, sId, pwd ? pwd : "*", active, timestamp, newPwd ? newPwd : "*", new_pwd_pending);

    // Execute the command
    system(command);
}

static int auth_write(void *context, const auth_dbId_t dbId, auth_t const *auth)
{
    struct auth_serialize_t *c = context;
    char *pwd = NULL;
    char *newPwd = NULL;
    char dt[50];
    if (auth->pwd.len && auth->pwd.buf) pwd = bcd_encode(auth->pwd.buf, auth->pwd.len);
    if (auth->new_pwd_pending && auth->new_pwd.len && auth->new_pwd.buf) newPwd = bcd_encode(auth->new_pwd.buf, auth->new_pwd.len);
    dprintf(c->fd, "%d;%s;%s;%s;%s%s;%d;%s;%s%s;%d\n", ++c->no,
        dbId, auth->key.login, auth->key.sId, pwd?"$0$":"", 
        pwd?pwd:"*", auth->active, auth_strHorodate(dt, sizeof(dt), &auth->timestamp), newPwd?"$0$":"", newPwd?newPwd:"*", auth->new_pwd_pending
    );
    log_auth_info(c->fd, c->no, dbId, auth->key.login, auth->key.sId, pwd, auth->active, auth_strHorodate(dt, sizeof(dt), &auth->timestamp), newPwd, auth->new_pwd_pending);

    _free(pwd);
    _free(newPwd);
    return 1;
}

int auth_db_serialize(int fd)
{
    struct auth_serialize_t c = {.fd = fd, .no=0};
    return auth_foreach(NULL, NULL, auth_write, &c);
}


ssize_t auth_serialize_to_file(char const *fname)
{
    ssize_t ret = -1;
    //-- On bloque le total en ecriture
    _rwlock_write(&_auth_dbs.lock);
    if (fname && *fname != '\0')
    {
        int fd = open(fname, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP);
        if (fd < 0)
        {
            _log_errno("%s", fname);
        }
        else
        {
            auth_db_serialize(fd);
            close(fd);
        }
    }
    _rwlock_release(&_auth_dbs.lock);
    return ret;
}


enum {
    _auth_column_no_   = 0,
    _auth_column_dbId_,
    _auth_column_login_,
    _auth_column_sId_,
    _auth_column_pwd_,
    _auth_column_active_,
    _auth_column_timestamp_,
    _auth_column_new_pwd_,
    _auth_column_new_one_pending_,

    _auth_column_max_,
} ;



ssize_t auth_unserialize_from_file(char const *fname, int(*check_deb)(const auth_dbId_t dbId))
{
    ssize_t ret = -1;
    //-- On bloque le total en ecriture
    _rwlock_write(&_auth_dbs.lock);
    if (fname && *fname != '\0')
    {
        int fd = open(fname, O_RDONLY);
        if (fd < 0)
        {
            _log_errno("%s", fname);
        }
        else
        {
            //-- mapping mémoire
            char *data = NULL;
            struct stat fileStat;
            if (!fstat(fd, &fileStat))
            {
                data = mmap(NULL, fileStat.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
                if (data == MAP_FAILED)
                {
                    _log_errno("failed to map %s, aborted!", fname);
                }
            }
            else
            {
                _log_errno("failed to map %s, aborted!", fname);
            }
            close(fd);
            if (data != MAP_FAILED)
            {
                //-- identification des lignes
                char *tok;
                char *s;
                static char const *delim="\n";
                s = strtok_r(data, delim, &tok);
                uint line=1;
                auth_key_t key={.dbId=NULL, .login=NULL, .sId=NULL};
                auth_t *auth=NULL;
                ret = 0;
                while(s)
                {
                    char **argv;
                    int argc;
                    argc = decodeparam(s, ';', &argv);
                    if (argc && argv)
                    {
                        // if (argc == _auth_column_max_)
                        if (argc >= _auth_column_max_-2)
                        {
                            if (!check_deb || (check_deb && check_deb(argv[_auth_column_dbId_])))
                            {
                                //-- il faudrait faire une vérification de truc..., mais bon...
                                // bool active = isdigit(*argv[_auth_column_active_]) && atoi(argv[_auth_column_active_]);
                                bool active = 1; // \todo BUG parse de active ne semble pas fonctinoner
                                auth_pwd_t pwd = _auth_pwd_null_;
                                auth_pwd_t new_pwd = _auth_pwd_null_;

                                HoroDate timestamp={-1,-1};
                                if(strlen(argv[_auth_column_timestamp_]) >= 19) str2GmtHorodate(argv[_auth_column_timestamp_], &timestamp);
                                else { 
                                    timestamp.tv_sec = 0;
                                    timestamp.tv_nsec = 0;
                                    _log_info("timestamp filled with 1970 for login %s", argv[_auth_column_login_]);
                                }
                                if (timestamp.tv_sec <0||timestamp.tv_nsec<0)
                                {
                                    _log_err("file %s, line %u, decode timestamp failed, ignore line", fname, line);
                                }
                                else
                                if (!auth_pwd_decode(&pwd, argv[_auth_column_pwd_]))
                                {
                                    _log_err("file %s, line %u, decode password failed, ignore line", fname, line);
                                }
                                else
                                if (!(auth = auth_search_by_key(auth_key_set(&key, argv[_auth_column_dbId_],argv[_auth_column_login_],argv[_auth_column_sId_]))))
                                {
                                    _log_err("file %s, line %u, creation failed %s:%s:%s", fname, line, argv[_auth_column_dbId_],argv[_auth_column_login_],argv[_auth_column_sId_]);
                                }
                                else
                                {
                                    if (argc == _auth_column_max_)
                                    {
                                        // \todo BUG parse new pw et flag
                                        bool new_pwd_pending = isdigit(*argv[_auth_column_new_one_pending_]) && atoi(argv[_auth_column_new_one_pending_]);

                                        _log_info("file %s, line %u, new_pwd_pending %d", fname, line, new_pwd_pending);

                                        if(new_pwd_pending) {
                                                        
                                            if (!auth_pwd_decode(&new_pwd, argv[_auth_column_new_pwd_]))
                                            {
                                                _log_err("file %s, line %u, decode password failed, ignore line", fname, line);
                                            } else {
                                                // recopie du potentiel nouveau mot de passe pending
                                                if (auth_pwd_cmp(&auth->new_pwd, &new_pwd))
                                                {
                                                    auth_pwd_cpy(&auth->new_pwd, &new_pwd);
                                                }

                                                auth->new_pwd_pending = new_pwd_pending;
                                            
                                            }
                                        }
                                    }
                                    
                                    //-- On recopie le mot de passe "courant"
                                    if (auth_pwd_cmp(&auth->pwd, &pwd))
                                    {
                                        auth_pwd_cpy(&auth->pwd, &pwd);
                                    }
                                


                                    if (auth->active != active) auth->active = active;
                                    if (cmpHorodate(&auth->timestamp, &timestamp)) auth->timestamp = timestamp;
                                    ret++;
                                } // \todo chargement du nouveau mdp et flag pending

                            }
                            else
                            {
                                _log_err("file %s, line %u, dbname %s unknown, ignore line", fname, line, argv[_auth_column_dbId_]);
                            }
                        }
                        else
                        {
                            _log_warn("file %s, line %u, mal formatted, ignored", fname, line);
                        }
                        free(argv);
                        argv=NULL;
                    }
                    s = strtok_r(NULL, delim, &tok);
                    line++;
                }
                //-- Fermeture du mapping
                if (munmap(data, fileStat.st_size)) 
                {
                    _log_errno("failed to unmap %s, aborted!", fname);
                }
            }
        }
    }
    system("rm -f auth.db.log");
    _rwlock_release(&_auth_dbs.lock);
    return ret;
}

/*

 ad88888ba   88888888888  ,ad8888ba,   88888888ba   88888888888  888888888888  ad88888ba
d8"     "8b  88          d8"'    `"8b  88      "8b  88                88      d8"     "8b
Y8,          88         d8'            88      ,8P  88                88      Y8,
`Y8aaaaa,    88aaaaa    88             88aaaaaa8P'  88aaaaa           88      `Y8aaaaa,
  `"""""8b,  88"""""    88             88""""88'    88"""""           88        `"""""8b,
        `8b  88         Y8,            88    `8b    88                88              `8b
Y8a     a8P  88          Y8a.    .a8P  88     `8b   88                88      Y8a     a8P
 "Y88888P"   88888888888  `"Y8888Y"'   88      `8b  88888888888       88       "Y88888P"


*/

typedef struct __auth_secret_t
{
    auth_secret_t secret;
    char const *ref;
    __auth_t *to_auth;
    sDblChain     chain;
} __auth_secret_t;

#define auth_secret_by_chain(a) container_of(a, __auth_secret_t, chain)
#define auth_secret_by_secret(a) container_of(a, __auth_secret_t, secret)

static __auth_secret_t *auth_secret_new(char const *ref, auth_secret_t const *secret)
{
    __auth_secret_t *item = calloc(1, sizeof(*item));
    if (item)
    {
        ldcInitialize(&item->chain);
        item->ref = strdup(ref);
        auth_secret_cpy(&item->secret, secret);
    }
    return item;
}

static void auth_secret_free(__auth_secret_t *item)
{
    if (item)
    {
        if (!ldcIsEmpty(&item->chain)) ldcSuppress(&item->chain);
        if (item->ref) free((char *)item->ref);
        auth_secret_release(&item->secret);
        free(item);
    }
}

static void auth_secrets_purge(sDblChain *root)
{
    while(!ldcIsEmpty(root))
    {
        auth_secret_free(auth_secret_by_chain(root->next));
    }
}

static __auth_secret_t *auth_secret_find(rwlock_t *lock, sDblChain *root, char const *ref)
{
    __auth_secret_t *item = NULL;
    if (lock) _rwlock_read(lock);

    sDblChain *node;
    ldcForeach(node, root)
    {
        item = auth_secret_by_chain(node);
        if (strcmp(ref, item->ref)==0) break;
        item = NULL;
    }
    if (lock) _rwlock_release(lock);
    return item;
}


static inline __auth_secret_t *__auth_secret_get(__auth_t *auth, char const *ref)
{   
    __auth_secret_t *item =NULL;
    if (auth && ref)
    {
        item=auth_secret_find(&auth->lock, &auth->secrets, ref);
    }
    return item;
}


AUTH_PROTECTED auth_secret_t *auth_secret_get(AUTH_PROTECTED auth_t *auth, char const *ref)
{   
    if (!auth || !ref) return NULL;
    __auth_secret_t *item = __auth_secret_get(__auth_by_auth(auth), ref);
    return item?&item->secret:NULL;
}


AUTH_PROTECTED auth_secret_t *auth_secret_find_by_key(auth_key_t const *key, char const *ref)
{
    return (!key || !ref)?NULL:auth_secret_get(auth_find_by_key(key), ref);
}

bool auth_secret_delete_by_key(auth_key_t const *key, char const *ref)
{
    bool ret = false;
    if (key && ref)
    {
        auth_secret_t  *secret = auth_secret_find_by_key(key, ref);
        if (secret)
        {
            __auth_secret_t *__secret = auth_secret_by_secret(secret);
            _rwlock_write(&__secret->to_auth->lock);
            auth_secret_free(__secret);
            // _rwlock_release(&__secret->to_auth->lock); ce release ne peut avoir lieu apres un free
            ret = true;
        }
    }
    return ret;
}

static inline auth_secret_t * __auth_secret_add_or_update(__auth_t const *__auth, char const *ref, auth_secret_t const *secret)
{
    __auth_secret_t *__secret = NULL;
    if (__auth && ref)
    {
        _rwlock_write(&__auth->lock);
        __secret = __auth_secret_get(__auth, ref);
        if (!__secret)
        {
            //-- Création et insertion...
            __secret = auth_secret_new(ref, secret);
            if (__secret)
            {
                __secret->to_auth = __auth;
                ldcInsert(&__auth->secrets, &__secret->chain);
            }
        }
        else
        if (secret)
        {
            auth_secret_cpy(&__secret->secret, secret);
        }
        _rwlock_release(&__auth->lock);
    }
    return __secret?&__secret->secret:NULL;
}

AUTH_PROTECTED auth_secret_t * auth_secret_add_or_update(auth_t const *auth, char const *ref, auth_secret_t const *secret)
{
    return auth?__auth_secret_add_or_update(__auth_by_auth(auth), ref, secret):NULL;
}

AUTH_PROTECTED auth_secret_t * auth_secret_add_or_update_by_key(auth_key_t const *key, char const *ref, auth_secret_t const *secret)
{
    return (key)?auth_secret_add_or_update(auth_find_by_key(key), ref, secret):NULL;
}



/*

88  888b      88  88  888888888888
88  8888b     88  88       88
88  88 `8b    88  88       88
88  88  `8b   88  88       88
88  88   `8b  88  88       88
88  88    `8b 88  88       88
88  88     `8888  88       88
88  88      `888  88       88


*/

static void auth_atexit(void)
{
    //-- purge des données d'authentification
    auth_db_purge();
}


void auth_initialize(char const *secret)
{
    atexit(auth_atexit);

}

/**@}*/