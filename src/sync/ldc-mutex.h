/**
* @file ldc-mutex.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-05-11
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef SYNC_LDC_MUTEX_H
#define SYNC_LDC_MUTEX_H
#include <stdbool.h>

#include "lstdblcyc/lstdblcyc.h"
#include "lock/mutex.h"
/**
 * @page ldc_mutex_pg Liste Doublement chaînée Circulaire avec exclusion mutuelle
 * 
 * Cet objet est l'héritage d'une liste définie dans le module lstdblcyc et d'un 
 * mécanisme d'exclusion mutuelle via l'objet lock_mutex_t du module lock
 * 
 * Les principaux accès à la liste sont écrits pour que la process appelant
 * dispose d'un verrouillage exclusif sur la liste.
 * 
 * \note Le verrou mutex utilisé est de type réentrant, cela _simplifie_ un peu
 *       la gestion externe du vérrouillage.
 * 
 * \attention Il reste nécessaire qu'autant d'appel de dévérouillage de la liste soient 
 *            faits que d'appel de verrouillage. Sinon la liste reste vérouillé et 
 *            inaccessible aux autres process
 * 
 */
/**
 * @defgroup ldc-mutex ldc avec mutex
 * 
 * @brief Définition d'uen liste double circulaire avec exclusion mutuelle
 * 
 * cf. \ref ldc_mutex_pg
 * @{
 */

/**#include "lock/mutex.h"

 * @brief Définition d'une liste avec verrou exclusif
 * 
 */
typedef struct ldc_mutex_t
{
    lock_mutex_t mutex;       /*!< \brief verrou d'exclusion */
    sDblChain root;           /*!< racine de la liste */
    size_t no;                /*!< nombre d'élément */
} ldc_mutex_t;

/**
 * @brief Prototype des callback sur modification d'un item
 * 
 * @param context contexte d'exécution
 * @param chain   élément de la liste
 */
typedef sDblChain *(*ldc_mutex_update_t)(void *context, sDblChain const *chain);

/**
 * @brief Prototype des callback sur boucle interne
 * 
 * @param context contexte d'exécution
 * @param chain   élément de la liste
 */
typedef int(*ldc_mutex_foreach_cb_t)(void *context, sDblChain const *chain);

/**
 * @brief Prototype d'une fonction de comparaison de deux entrées
 * 
 * @param n1 pointeur sur le premier nœud de la comparison
 * @param n2 pointeur sur le second nœud de la comparison
 * @return ==0 si n1==n2, >0 si n1>n2 et \<0 si n1\<n2
 */
typedef int (*ldc_mutex_entry_cmp_t)(sDblChain const *n1, sDblChain const *n2);

/**
 * @brief Prototype d'une fonction de comparaison d'un entrée avec un clé
 * 
 * @param entry pointeur sur l'entrée à comparer
 * @param key pointeur sur la clé de référence
 * @return ==0 si entry==key, >0 si entry>key et \<0 si entry\<key
 */
typedef int (*ldc_mutex_entry_cmp_key_t)(sDblChain const *entry, void const *key);

/**
 * @brief Prototype de libération d'une entrée de la liste
 * 
 * @param context contest d'exécution
 * @param entry pointeur sur l'entrée à libérer
 */
typedef void (*ldc_mutex_entry_free_t)(void *context, sDblChain *entry);

/**
 * @brief Création d'une liste
 * 
 * Création et initalisation 
 * @return ldc_mutex_t* 
 */
ldc_mutex_t *ldc_mutex_new(void);

/**
 * @brief Libération d'une liste
 * 
 * @param list liste précédement allouée
 */
void ldc_mutex_free(ldc_mutex_t *list, ldc_mutex_entry_free_t cb, void *context);

/**
 * @brief insert d'une entrée dans la liste
 * 
 * @param this la liste 
 * @param item l'entrée à insérer
 * @return sDblChain* le pointeur sur l'entrée insérée
 */
#define ldc_mutex_insert(list, item) ldc_mutex_insert_cmp((list), (item), NULL)


/**
 * @brief insert d'une entrée dans la liste
 * 
 * @param this la liste 
 * @param item l'entrée à insérer
 * @param cmp  la fonction de comparaison à utiliser si besoin. si NULL alors la
 *             liste est une FIFO
 * @return sDblChain* le pointeur sur l'entrée insérée
 */
sDblChain *ldc_mutex_insert_cmp(ldc_mutex_t *list, sDblChain *item, ldc_mutex_entry_cmp_t cmp);

/**
 * @brief retrouve une entrée dans la liste par une clé
 * 
 * @param this la liste
 * @param cmp  la fonction de comparaison avec la clé 
 * @param key  la clé
 * @return sDblChain* !=NULL si trouvé, ==NULL si non trouvé
 */
sDblChain *ldc_mutex_find(ldc_mutex_t *list, ldc_mutex_entry_cmp_key_t cmp, void const *key);

/**
 * @brief modifie ou insert une entrée
 * 
 * @param this la liste 
 * @param item l'entrée à insérer ou à modifier
 * @param cmp_key  la fonction de comparaison avec la clé 
 * @param key  la clé
 * @param update callback de modification ou création, si param NULL alors demande de création...
 * @param context context d'exécution du callback
 * @param cmp  la fonction de comparaison à utiliser si besoin. si NULL alors la
 * @return sDblChain* le pointeur sur l'entrée insérée
 */
sDblChain *ldc_mutex_search(ldc_mutex_t *list, ldc_mutex_entry_cmp_key_t cmp_key, void const *key, ldc_mutex_update_t update, void *context, ldc_mutex_entry_cmp_t cmp);

/**
 * @brief Extraction d'une entrée
 * 
 * Utilise la liste comme une FIFO, extrait l'entée pointée
 * par next.
 * 
 * @param list la liste
 * @return sDblChain* ==NULL si la liste est vide, !=NULL sinon
 */
sDblChain *ldc_mutex_extract(ldc_mutex_t *list);

/**
 * @brief supprime une entrée
 * 
 * Supprime en exclusion mutelle, une entrée de la liste
 *  
 * \attention c'est à l'appelant de s'assurer de la cohérence entre la liste
 *            et l'entrée à extraire...
 *      
 * @param list la liste
 * @param item nœud de la liste
 */
void ldc_mutex_suppress(ldc_mutex_t *list, sDblChain *item);

/**
 * @brief vérifie si une entrée est dans une liste
 * 
 *      
 * @param list la liste
 * @param item nœud de la liste
 * @return true ou false suivant si est ou n'est pas dans la liste
 */
bool ldc_mutex_is_in(ldc_mutex_t *list, sDblChain *item);

/**
 * @brief Extraction d'une entrée identifiée par sa clé
 * 
 * @param list la liste
 * @param cmp  la fonction de comparaison avec la clé 
 * @param key  la clé
 * @return sDblChain* ==NULL si la liste est vide, !=NULL sinon
 */
sDblChain *ldc_mutex_extract_by_key(ldc_mutex_t *list, ldc_mutex_entry_cmp_key_t cmp, void *key);

/**
 * @brief Verrouille une liste
 * 
 * \note Appel bloquant
 * 
 * Demande l'entrée en session critique
 * 
 * Le processus appellant est bloqué tant que mutex n'est pas 
 * disponible
 * 
 * @param list list à vérouiller
 */
void ldc_mutex_lock(ldc_mutex_t *list);

/**
 * @brief Déverrouille une liste
 * 
 * 
 * @param list list à vérouiller
 */
void ldc_mutex_unlock(ldc_mutex_t *list);

/**
 * @brief Initialisation d'une liste
 * 
 * @param this la liste 
 * @return pointeur sur la liste
 */
ldc_mutex_t *ldc_mutex_initialize(ldc_mutex_t *list);

/**
 * @brief Création d'une nouvelle liste
 * 
 * @return ldc_mutex_t* si !=NULL alors retourne une liste allouée 
 *                      en mémoire et initialisée
 */
ldc_mutex_t *ldc_mutex_new(void);

/**
 * @brief Parcours la liste des entrées de la liste
 * 
 * @param this    la liste
 * @param cb      callback à exécuter sur chaque entrée de la liste, le 
 *                parcours s'arrête si le callaback retourne une valeur 
 *                non nulle.
 * @param context context d'exécution du callback
 * @return int 
 */
ssize_t ldc_mutex_foreach(ldc_mutex_t *list, ldc_mutex_foreach_cb_t cb, void *context);

/**
 * @brief Vide une liste de ses entrées
 * 
 * @param list     Liste consernée 
 * @param fct_free Callback de libération de l'entrée, si non positionné
 *                 le flush utilise un simple free en considérant que a) 
 *                 l'entrée a été allouée dynamiquement et que b) le 
 *                 pointeur de chaînage est l'origine de la zone réservée...
 * @param context contexte d'exécution de la commande fct_free
 * @return ssize_t <0 si pb, sinon le nombre d'éléments supprimés (le no avant le flush)
 */
ssize_t ldc_mutex_flush(ldc_mutex_t *list, ldc_mutex_entry_free_t fct_free, void *context);

/**
 * @brief Donne le nombre d'entrées en liste
 * 
 * @param this     la liste
 * @return ssize_t le nombre d'élément en liste si >=0
 */
ssize_t ldc_mutex_no(ldc_mutex_t *list);



#define ldc_mutex_declare(var) ldc_mutex_t var =\
{\
    .root = {.next=&var.root, .prev=&var.root},\
    .mutex = LOCK_MUTEX_INITIALIZER_REC,\
    .no=0,\
}


/**@}*/
#endif //SYNC_LDC-MUTEX_H