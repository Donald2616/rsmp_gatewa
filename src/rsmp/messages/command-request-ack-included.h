/**
 * @file command-request-ack-included.h
 * @brief Définition du message CommandRequest avec Ack intégré.
 * @version 0.1
 * @date 2023-07-03
 * 
 * @details Ce fichier contient la définition de la structure et des fonctions associées
 *          au message CommandRequestAckIncluded, utilisé pour envoyer des commandes
 *          avec un acquittement intégré dans le protocole RSMP.
 */

 #ifndef RSMP_MESSAGES_CMD_REQUEST_ACK_INCLUDED_H
 #define RSMP_MESSAGES_CMD_REQUEST_ACK_INCLUDED_H

 #include "rsmp/fields/fields.h"      // Définitions générales des champs RSMP
 #include "rsmp/fields/type.h"        // Champ "type" pour identifier le type de message
 #include "rsmp/fields/mId.h"         // Champ "mId" pour l'identifiant unique du message
 #include "rsmp/fields/cId.h"         // Champ "cId" pour l'identifiant de commande
 #include "rsmp/fields/ntsOId.h"      // Champ "ntsOId" pour l'identifiant de l'objet cible
 #include "rsmp/fields/xNId.h"        // Champ "xNId" pour un identifiant supplémentaire
 #include "rsmp/fields/mType.h"       // Champ "mType" pour le type de message RSMP
 #include "rsmp/fields/arg.h"         // Champ "arg" pour les arguments de la commande
 #include "rsmp/messages/messages.h"  // Dépendances générales pour les messages 
 #include "rsmp/fields/ackType.h"
 #include "cmd-request.h"
#include "rsmp/fields/ackMessage.h"




 /**
 * @defgroup cmd-request ack included 
 * 
 * @brief message commande
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_CMD_REQUEST_ACK_INCLUDED_C
#define CMD_REQUEST_ACK_INCLUDED_EXTERN
#define CMD_REQUEST_ACK_INCLUDED_PRIVATE
#endif

#ifndef CMD_REQUEST_ACK_INCLUDED_EXTERN
#define CMD_REQUEST_ACK_INCLUDED_EXTERN const
#endif

#ifndef CMD_REQUEST_ACK_INCLUDED_PRIVATE
#define CMD_REQUEST_ACK_INCLUDED_PRIVATE const
#endif


  /**
  * @struct rsmp_msg_CommandRequestAckIncluded
  * @brief Structure représentant un message CommandRequest avec Ack intégré.
  * 
  * @details Cette structure contient les champs nécessaires pour définir un message
  *          CommandRequest avec un acquittement intégré dans le protocole RSMP.
  */
 typedef struct rsmp_msg_CommandRequestAckIncluded
 {
     type_t            type;       /*!< Type du message (identifie le type de message RSMP). */
     mId_t             mId;        /*!< Identifiant unique du message. */
     ntsOId_t          ntsOId;     /*!< Identifiant de l'objet cible. */
     xNId_t            xNId;       /*!< Identifiant supplémentaire (optionnel). */
     cId_t             cId;        /*!< Identifiant de commande. */
     rsmp_array_args_t arg;        /*!< Tableau des arguments de la commande. */
     mType_t           mType;      /*!< Type de message RSMP (ex. commande, réponse, etc.). */
     ack_t              ack;        /*!< Indicateur d'acquittement positif (true) ou négatif (false). */
     char              ackMessage[256]; /*!< Message d'acquittement ou d'erreur. */
 } rsmp_msg_CommandRequestAckIncluded_t;


/**
 * @brief Crée un nouveau message CommandRequestAckIncluded.
 * 
 * @param rsmp_context Contexte RSMP global ou spécifique.
 * @return Pointeur vers une nouvelle instance de rsmp_msg_CommandRequestAckIncluded_t.
 */
rsmp_msg_CommandRequestAckIncluded_t *rsmp_msg_CommandRequestAckIncluded_new(rsmp_context_t const *rsmp_context);

/**
 * @brief Libère la mémoire associée à un message CommandRequestAckIncluded.
 * 
 * @param item Pointeur vers l'instance à libérer.
 */
void rsmp_msg_CommandRequestAckIncluded_free(rsmp_msg_CommandRequestAckIncluded_t *item);

 /**
 * @brief Fonction interne pour créer un message CommandRequestAckIncluded.
 * 
 * @param rsmp_context Contexte RSMP global ou spécifique.
 * @return Pointeur générique vers une nouvelle instance de rsmp_msg_CommandRequestAckIncluded_t.
 */
void *_rsmp_msg_CommandRequestAckIncluded_new(rsmp_context_t const *rsmp_context);

/**
 * @brief Fonction interne pour libérer un message CommandRequestAckIncluded.
 * 
 * @param item Pointeur générique vers l'instance à libérer.
 */
void _rsmp_msg_CommandRequestAckIncluded_free(void *item);

 /**
 * @brief Exécute une commande reçue dans un message CommandRequestAckIncluded.
 * 
 * @param engine Pointeur vers le moteur RSMP.
 * @param attrib Attributs supplémentaires pour l'exécution.
 * @param context Contexte spécifique pour l'exécution.
 * @param msg Pointeur vers le message CommandRequestAckIncluded à exécuter.
 * @return Code d'erreur ou succès.
 */
extern int _rsmp_msg_CommandRequestAckIncluded_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);

 CMD_REQUEST_ACK_INCLUDED_EXTERN CMD_REQUEST_ACK_INCLUDED_PRIVATE extern rsmp_field_desc_t _rsmp_msg_CommandRequestAckIncluded_desc[];
 #endif // RSMP_MESSAGES_COMMAND_REQUEST_ACK_INCLUDED_H