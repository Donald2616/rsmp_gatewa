/**
* @file auth-basic.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-06
* 
* @copyright Copyright (c) 2024
* 
*/
#include "config.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "horodate/horodate.h"

#include "horodate/horodate.h"
#include "auth/auth.h"
#include "auth/auth-secret.h"

#ifndef AUTH_AUTH_BASIC_H
#define AUTH_AUTH_BASIC_H
/**
 * @defgroup auth-basic
 * 
 * @brief Définition des données d'authentification 
 * 
 * ## Description
 * 
 * Il y a deux bases de données similaires : une, base _to_, pour les communications vers un
 * pair ; l'autre, base _from_ pour les communications en provenance d'un tiers.
 * 
 * Dans les deux cas les données d'authentifications sont identiques : 
 * - un login,
 * - un identifiant de site
 * - un mot de passe (ou secret)
 * 
 * Ces deux bases de données à termes doivent être conçus pour protéger les secrets.
 * C'est-à-dire que leur sérialisation doit être chiffrée.
 * 
 * Dans chaque base et pour un identifiant de site, il peut exister un certain nombre de login.
 * 
 * Il n'y a pas de dépendance entre les deux bases, leur contenu et tous les champs sont
 * entièrement indépendants.
 * 
 * Les entrées de ces base permettent de dériver les clés de chiffrement associées à l'entrée 
 * et à l'algo symétrique utilisé, mais il faut adjoindre un élément qui n'est pas stocké dans 
 * les bases pour pouvoir dériver les clés par cipher.
 *  
 * 
 * ### base _to_
 * Les données de cette base servent à chiffrer un message sortant.
 * 
 * Chaque message doit disposer d'une référence dans cette base pour être traité.
 * 
 * Pour les messages d'ordre fonctionnelle comme les _xxxxrequest_ c'est à la couche 
 * fonctionnelle de choisir l'entrée dans la base qui sera utiliser.
 * 
 * Pour les autres messages dont les messages internes au moteur, il faut définir une 
 * entrée par défaut pour la destination.
 * 
 * ### base _from_
 * La base _from_ est nécessaire au déchiffrement des messages reçus.
 * Cette base sert pour authentifier les messages reçus, mais aussi comme point d'entrée 
 * pour la vérification d'autorisation.
 * 
 * Il sera judicieux que les données d'authorisation soit aussi protéger dans leur sérialisation
 * 
 * Pour l'instant on table sur uen base d'autorisation déconnectée de la base from.
 * @{
 */
#ifndef AUTH_PROTECTED
#define AUTH_PROTECTED const
#endif
/**
 * @brief Libère une entrée d'authentification
 * 
 * @param ptr pointeur sur l'entrée
 */
void auth_free(auth_t *ptr);

/**
 * @brief signature d'une fonction de callback de parcours
 * 
 * @param context context d'exécution de la fonction
 * @param dbId identifiant de la base 
 * @param auth entrée d'authentification
 */
typedef int(auth_foreach_callback_t)(void *context, const auth_dbId_t dbId, auth_t const *auth);

/**
 * @brief sérialisation de la base sur un flux
 * 
 * crée des entrées de ce type
 * 
 *     2;to;log-001;sId-2;$0$FA2BCF25D4B8722D72199ABAA9DFC37F33A61A3AA78D0D;0;2016-03-03T13:41:03Z
 *     3;to;log-025;sId-2;$0$3FFEB685B55E2BCD4A2595908DE214337F81BBB21B680E;0;2016-03-03T13:41:03Z
 * 
 * avec les champs suivants :
 * - n° de la ligne
 * - base de donnée
 * - login
 * - sId
 * - encodage du mot de passe `$x$` représente l'encodage, ensuite le mot de passe en bcd
 * - flag d'activation
 * - date de la dernière modification du mot de passe
 * 
 * @param fd descripteur du fichier de sortie
 * @return int le nombre de lignes
 */
int auth_db_serialize(int fd);

/**
 * @brief Sérialisation sur fichier
 * 
 * transfert les données en mémoire sur un fichier de sauvegarde
 * 
 * @param fname nom du fichier
 * @return ssize_t nombre d'items sauvegardés
 */
ssize_t auth_serialize_to_file(char const *fname);

/**
 * @brief désérialisation depuis un fichier
 * 
 * Récupère la base de données depuis un fichier.
 * 
 * @param fname nom du fichier à lire
 * @param check_db fonction de vérification de la base de données
 * @return ssize_t 
 */
ssize_t auth_unserialize_from_file(char const *fname, int(*check_db)(const auth_dbId_t dbId));

/**
 * @brief Initialisation de la base d'authentification
 * 
 * @param secret si non NULL la clé du chiffrement
 */
void auth_initialize(char const *secret);
/**
 * @brief initialisation sans secret...
 * 
 */
#define  auth_initialize_no_secret() auth_initialize(NULL)

/**
 * @brief Modification du mot de passe
 * 
 * @param auth 
 * @param buf 
 * @param len 
 * @return true 
 * @return false 
 */
bool auth_db_pwd_change(AUTH_PROTECTED auth_t *auth, char *buf, size_t len);
#define auth_db_pwd_change_auto(a) auth_db_pwd_change(a, NULL, 0)
#define auth_db_pwd_change_str(a, s) auth_db_pwd_change(a, s, strlen(s))

/**
 * @brief 
 * 
 * @param auth 
 * @param activate 
 * @return true 
 * @return false 
 */
bool auth_db_active_set(AUTH_PROTECTED auth_t *auth, bool activate);

// #ifdef AUTH_BASIC_LEGACY

/**
 * @brief recherche une clé dans la base de donnée
 * 
 * @param key clé de recherche
 * @return auth_t* l'enregistrement trouvé ou NULL
 */
AUTH_PROTECTED auth_t * auth_find_by_key(const auth_key_t *key);

/**
 * @brief création d'une entrée dans la base
 * 
 * @param dbId   identifiant de l'entrée
 * @param login  login
 * @param sId    sId
 * @param len    longueur du mt de passe
 * @param pwd    mot de passe (chaîne d'octet, pas chaîne de caractères)
 * @param ts     timestamp (si NULL création aujourd'hui)
 * @param active  activation de l'entrée
 * @param new_pwd_len longueur du nouveau mot de passe
 * @param new_pwd nouveau mot de passe
 * @param new_pwd_pending flag de (potentiel) nouveau mot de passe
 * @return auth_t* 
 */
auth_t *auth_new(const auth_dbId_t dbId, const auth_login_t login, const auth_sId_t sId, size_t pwd_len, char const *pwd, HoroDate const *ts, bool active, size_t new_pwd_len, char const *new_pwd, bool new_pwd_pending);

/**
 * @brief position une cl&
 * 
 * @param key    la structure à initialiser
 * @param dbId   identifiant de l'entrée
 * @param login  login
 * @param sId    sId
 * @return auth_key_t* 
 */
static inline auth_key_t *auth_key_set(auth_key_t *key, const auth_dbId_t dbId, const auth_login_t login, const auth_sId_t sId) 
{
    _log_info("key = %p", key);
    
    if (key)
    {
        key->dbId = dbId;
        key->login = login;
        key->sId = sId;
    }
    return key;
}

/**
 * @brief vérifie un identifiant de base
 * 
 * @param val  l'instance
 * @param dbId la référence
 * @return true 
 * @return false 
 */
static inline bool auth_dbId_is(const auth_dbId_t val, const auth_dbId_t dbId) 
{ 
    return val?strcmp(val, dbId)==0:false;
}
/**
 * @brief vérifie di une entrée est bien associée à une base
 * 
 * @param item  l'entrée
 * @param dbId  la référence
 * @return true 
 * @return false 
 */
static inline bool auth_db_is(const auth_t *item, const auth_dbId_t dbId) 
{ 
    return item?auth_dbId_is(item->key.dbId, dbId):false;
}

/**
 * @brief supprime une entrée via sa clé
 * 
 * @param key clé primaire
 */
void auth_delete_by_key(auth_key_t const *key);

/**
 * @brief parcours la liste des entrées
 * 
 * 
 * @param dbId    identifiant de l'entrée
 * @param login   login
 * @param fct     callback sur chaque entrée
 * @param context context d'exécution du callback
 * @return int 
 */
int auth_foreach(const auth_dbId_t dbId, const auth_sId_t sId, auth_foreach_callback_t fct, void *context);

/**
 * @brief insertion par clé
 * 
 * Si la clé n'existe pas l'insert
 * 
 * @param key       clé complète
 * @return auth_t* 
 */
auth_t *auth_insert_by_key(auth_key_t const *key);

/**
 * @brief retrouve ou crée par clé
 * 
 * si la clé n'est pas trouvée, alors création de la clée
 * 
 * @param key       clé complète
 * @return auth_t* 
 */
auth_t *auth_search_by_key(auth_key_t const *key);

// #endif


auth_t *auth_get_first(auth_key_t const *key);
auth_t *auth_get_next_from_auth(auth_t const *auth);

ssize_t auth_unserialize_from_file(char const *fname, int(*check_deb)(const auth_dbId_t dbId));
ssize_t auth_serialize_to_file(char const *fname);
int auth_db_serialize(int fd);


AUTH_PROTECTED auth_secret_t *auth_secret_get(AUTH_PROTECTED auth_t *auth, char const *ref);
AUTH_PROTECTED auth_secret_t *auth_secret_find_by_key(auth_key_t const *key, char const *ref);
bool auth_secret_delete_by_key(auth_key_t const *key, char const *ref);
AUTH_PROTECTED auth_secret_t * auth_secret_add_or_update(auth_t const *auth, char const *ref, auth_secret_t const *secret);
AUTH_PROTECTED auth_secret_t * auth_secret_add_or_update_by_key(auth_key_t const *key, char const *ref, auth_secret_t const *secret);

void testDDB(void);
/**@}*/
#endif //AUTH_AUTH_H