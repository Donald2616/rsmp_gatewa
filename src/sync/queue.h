/**
* @file queue.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-05-10
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef SYNC_QUEUE_H
#define SYNC_QUEUE_H
#include <stdio.h>

#ifdef SYNC_QUEUE_C
#define QUEUE_PRIVATE
#define QUEUE_PROTECTED
#define QUEUE_EXTERN
#endif

#ifdef QUEUE_INHERITAGE
#define QUEUE_PROTECTED 
#endif

#ifndef QUEUE_PRIVATE
#define QUEUE_PRIVATE const
#endif
#ifndef QUEUE_PROTECTED
#define QUEUE_PROTECTED const
#endif
#ifndef QUEUE_EXTERN
#define QUEUE_EXTERN extern
#endif


#include <stdbool.h>

/**
 * @page queue-pg Défintion d'une queue de message
 * 
 * La queue de message est une structure permettant l'echange de message
 * entre _x_ producteurs et _y_ consomateurs
 * 
 * elle s'appuit sur une liste de type ldc_mutex_t et de la mise en place d'un
 * mécanisme de signalisation par sémaphore de mise à disposition d'éléments.
 * 
 * La queue utilise une liste avec priorité (voir ldcAged dans le module lstdblcyc)
 * Cette particularitéimplique 
 * 
 * Les éléments 
 * 
 */
/**
 * @defgroup queue Message Queue
 * 
 * @brief Définition d'une queue de message
 * 
 * @{
 */
typedef unsigned int queue_msg_prio_t;
#define QUEUE_MSG_PRIO_MAX LDC_AGE_MAX
#define QUEUE_MSG_PRIO_MIN LDC_AGE_MIN

QUEUE_EXTERN const bool __queue_clone_msg__
#ifdef SYNC_QUEUE_C
=true
#endif
;

QUEUE_EXTERN const bool __queue_direct_msg__
#ifdef SYNC_QUEUE_C
=false
#endif
;

/**
 * @brief Définition d'un message e la liste
 * 
 */
typedef struct queue_msg_t
{
    char *                buf;      /*!< \brief pointeur vers la zone de donénes effective (payload) */
    size_t                length;   /*!< \brief longeur en octet de la zone buf */
    queue_msg_prio_t prio;     /*!< \brief priorité initiale du message */
    bool                 flag_do_encrypt; /*!< \brief flag indiquant s'il faut crypter le message */
    void const *          mark;     /*!< \brief marqueur associé au message qui généralement identifie le producteur */
    // void const *        cyber;
    void const *        attrib;

} queue_msg_t;


/**
 * @brief objet de queue
 * 
 */
typedef struct queue
{
    /**
     * @brief Défini les données privées et inaccessibles
     */
    void *dptr; 
             
    /**
     * @brief nombre d'entrée dans la queue
     * 
     * @param this pointeur sur la queue
     * @return le nombre de message en queue si >= 0
     */
    ssize_t (*no)(struct queue *this);

    /**
     * @brief tire un message de la queue (consomateur)
     * 
     * _Principe générale_
     * 
     * Le processus appelant est bloqué jusqu'à ce qu'un message 
     * soit disponible dans la queue.
     * Dès que la présente d'un mesage est signalé le mesage en tête de 
     * la liste soujacete est extrait et son contenu recopié dasn la structure 
     * soumise par l'appelant.
     * 
     * Si un timeout est positionné et qu'il échut avant la réception d'un
     * message, alors le code retour est négatif et la variable errno est positionnée 
     * à ETIMEDOUT
     * 
     * @param this la queue
     * @param[out] msg le contenu du message
     * @param[in] timout timeout en milliseconde si <0 attente infini si 0 pas d'attente
     * @return ==0 OK, \<0 en cas d'échec.
     */
    ssize_t (*pull)(struct queue *this, queue_msg_t *msg, long timeout);

    /**
     * @brief pousse un message en queue (producteur)
     * 
     * Le message associée est recopié dans une allouée
     * dynamiquement, mis dasn la liste et un signalement aux
     * producteurs est effectué.
     * 
     * _Utilisation de clone_
     * 
     * Si clone est positionné à `__queue_direct_msg__`, alors le message ne sera
     * pas cloner. Cela peut être nécessaire lorsque l'appelant effectue lui-même
     * l'allocation dynamique du buffer et ne compte pas libérer la mémoire avant
     * l'extraction du message es liste.
     * 
     * Si l'appelant utilise une structure fixe définie dans sa pile, il est 
     * nécessaire que clone soit positionné à `__queue_clone_msg__` et qu'à 
     * l'extraction une procédure de libération soit mise en œuvre.
     * 
     * Le principe est que la mémoire pointée par le champ buf est _toujours_
     * allouée dynamiquement...
     * 
     * @param this la queue
     * @param[in] msg le contenu du message
     * @param[in] clone flag indiquant s'il faut cloner (true) le bufer ou non
     * @return 0 en cas de succès, <0 sinon
     */
    ssize_t (*push)(struct queue *this, queue_msg_t const *msg, bool clone);
    /**
     * @brief Vide une queue...
     * 
     * \attention au effet de bord...
     * 
     * @param this la queue
     */
    ssize_t (*flush)(struct queue *this);
    /**
     * @brief Libère une queue
     * 
     * @param this la queue
     */
    void (*free)(struct queue *this);
} queue_t;

/**
 * @brief Constructeur de queue
 * 
 * @return queue_t* une queue toute prête à l'emploi.
 */
queue_t *queue_new(void);

/**
 * @brief Desctruction de l'environemendes queue
 * 
 * Lancer le flush et la libération des entrées recyclées
 * 
 * \note à lancer à la fin du programme pour le grand nettoyage final...
 */
void queue_destroy(void);


/**
 * @brief Couple de queue
 * 
 * Permet la création d'une structure ayant 
 * deux queues une pour chaque sens de communication
 */
typedef struct {
    queue_t *queue;
    QUEUE_PRIVATE unsigned int flags;
} queue_inout_t[2];

/**
 * @brief CRéation d'un couple de queue
 * 
 * @param queues tables des queues à initialiser
 * @param q0     si non NULL la queue pointée sera à l'indice 0, sinon une queue sera allouée
 * @param q1     si non NULL la queue pointée sera à l'indice 1, sinon une queue sera allouée
 * @return -1 sur erreur, 0 si OK 
 */
ssize_t queue_inout_init(queue_inout_t queues, queue_t *q0, queue_t *q1);

/**
 * @brief CRéation d'un couple de queue
 * 
 * @param q0 si non NULL la queue pointée sera à l'indice 0, sinon une queue sera allouée
 * @param q1 si non NULL la queue pointée sera à l'indice 1, sinon une queue sera allouée
 * @return queue_inout_t* 
 */
queue_inout_t *queue_inout_new(queue_t *q0, queue_t *q1);

/**
 * @brief Vide un couple de queue
 * 
 * @param queues table des queues à vider
 */
void queue_inout_flush(queue_inout_t queues);

void queue_inout_free_queue(queue_inout_t queues);

/**
 * @brief libère un couple de queue
 * 
 * Lance un flush avant la libération de la table.
 * 
 * @param this pointeur sur le couple de queue
 */
void queue_inout_free(queue_inout_t *this);
void queue_inout_clone(queue_inout_t des, queue_inout_t src);

/**@}*/
#endif //SYNC_QUEUE_H