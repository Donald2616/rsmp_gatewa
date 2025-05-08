/**
 * @file command-response-ack-included.h
 * @brief Définition du message CommandResponse avec Ack intégré.
 * @version 0.1
 * @date 2023-07-03
 * 
 * @details Ce fichier contient la définition de la structure et des fonctions associées
 *          au message CommandResponseAckIncluded, utilisé pour envoyer des réponses
 *          avec un acquittement intégré dans le protocole RSMP.
 */

 #ifndef RSMP_MESSAGES_CMD_RESPONSE_ACK_INCLUDED_H
 #define RSMP_MESSAGES_CMD_RESPONSE_ACK_INCLUDED_H
 
 #include "rsmp/fields/fields.h"      // Définitions générales des champs RSMP
 #include "rsmp/fields/type.h"        // Champ "type" pour identifier le type de message
 #include "rsmp/fields/mId.h"         // Champ "mId" pour l'identifiant unique du message
 #include "rsmp/fields/cId.h"         // Champ "cId" pour l'identifiant de commande
 #include "rsmp/fields/ntsOId.h"      // Champ "ntsOId" pour l'identifiant de l'objet cible
 #include "rsmp/fields/xNId.h"        // Champ "xNId" pour un identifiant supplémentaire
 #include "rsmp/fields/mType.h"       // Champ "mType" pour le type de message RSMP
 #include "rsmp/fields/rvs.h"         // Champ "rvs" pour les résultats de la commande
 #include "rsmp/fields/cTS.h"         // Champ "cTS" pour l'horodatage
 #include "rsmp/fields/ackType.h"     // Champ "ack" pour l'acquittement
 #include "rsmp/fields/ackMessage.h"  // Champ "ackMessage" pour les messages d'acquittement
 #include "rsmp/messages/messages.h"  // Dépendances générales pour les messages
 
 /**
 * @defgroup cmd-response ack included 
 * 
 * @brief message commande
 * 
 * @{
 */
#ifdef RSMP_MESSAGES_CMD_RESPONSE_ACK_INCLUDED_C
#define CMD_RESPONSE_ACK_INCLUDED_EXTERN
#define CMD_RESPONSE_ACK_INCLUDED_PRIVATE
#endif

#ifndef CMD_RESPONSE_ACK_INCLUDED_EXTERN
#define CMD_RESPONSE_ACK_INCLUDED_EXTERN extern
#endif

#ifndef CMD_RESPONSE_ACK_INCLUDED_PRIVATE
#define CMD_RESPONSE_ACK_INCLUDED_PRIVATE const
#endif

/**
 * @struct rsmp_msg_CommandResponseAckIncluded
 * @brief Structure représentant un message CommandResponse avec Ack intégré.
 * 
 * @details Cette structure contient les champs nécessaires pour définir un message
 *          CommandResponse avec un acquittement intégré dans le protocole RSMP.
 */
typedef struct rsmp_msg_CommandResponseAckIncluded
{
    type_t mType;             /*!< Type de message RSMP */
    type_t type;              /*!< Type spécifique du message */
    mId_t mId;                /*!< Identifiant unique du message */
    ntsOId_t ntsOId;          /*!< Identifiant de l'objet cible */
    xNId_t xNId;              /*!< Identifiant supplémentaire */
    cId_t cId;                /*!< Identifiant de commande */
    cTS_t cTS;                /*!< Horodatage de la commande */
    rsmp_array_rvs_t rvs;     /*!< Tableau des résultats associés à la commande */
    ack_t ack;                /*!< Indicateur d'acquittement (Ack ou Not Ack) */
    char ackMessage[256];     /*!< Message d'acquittement ou d'erreur */
} rsmp_msg_CommandResponseAckIncluded_t;
 
 /**
  * @brief Crée un nouveau message CommandResponseAckIncluded.
  * 
  * @param rsmp_context Contexte RSMP global ou spécifique.
  * @return Pointeur vers une nouvelle instance de rsmp_msg_CommandResponseAckIncluded_t.
  */
 rsmp_msg_CommandResponseAckIncluded_t *rsmp_msg_CommandResponseAckIncluded_new(rsmp_context_t const *rsmp_context);
 
 /**
  * @brief Libère la mémoire associée à un message CommandResponseAckIncluded.
  * 
  * @param item Pointeur vers l'instance à libérer.
  */
 void rsmp_msg_CommandResponseAckIncluded_free(rsmp_msg_CommandResponseAckIncluded_t *item);
 
 /**
  * @brief Fonction interne pour créer un message CommandResponseAckIncluded.
  * 
  * @param rsmp_context Contexte RSMP global ou spécifique.
  * @return Pointeur générique vers une nouvelle instance de rsmp_msg_CommandResponseAckIncluded_t.
  */
 void *_rsmp_msg_CommandResponseAckIncluded_new(rsmp_context_t const *rsmp_context);
 
 /**
  * @brief Fonction interne pour libérer un message CommandResponseAckIncluded.
  * 
  * @param item Pointeur générique vers l'instance à libérer.
  */
 void _rsmp_msg_CommandResponseAckIncluded_free(void *item);
 
 /**
  * @brief Exécute une commande reçue dans un message CommandResponseAckIncluded.
  * 
  * @param engine Pointeur vers le moteur RSMP.
  * @param attrib Attributs supplémentaires pour l'exécution.
  * @param context Contexte spécifique pour l'exécution.
  * @param msg Pointeur vers le message CommandResponseAckIncluded à exécuter.
  * @return Code d'erreur ou succès.
  */
 int _rsmp_msg_CommandResponseAckIncluded_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);
 
 
 CMD_RESPONSE_ACK_INCLUDED_EXTERN CMD_RESPONSE_ACK_INCLUDED_PRIVATE rsmp_field_desc_t _rsmp_msg_CommandResponseAckIncluded_desc[];

/**
 * @brief Crée un message CommandResponseAckIncluded à partir d'une requête CommandRequestAckIncluded.
 * 
 * @param request Pointeur vers la requête CommandRequestAckIncluded.
 * @return Pointeur vers une nouvelle instance de CommandResponseAckIncluded.
 */
rsmp_msg_CommandResponseAckIncluded_t *rsmp_msg_CommandResponseAckIncluded_from_request(rsmp_msg_CommandRequestAckIncluded_t const *request);
 #endif // RSMP_MESSAGES_CMD_RESPONSE_ACK_INCLUDED_H