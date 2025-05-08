/**
 * @file status-request-ack-included.h
 * @brief Déclaration des structures et fonctions pour StatusRequestAckIncluded.
 * @version 0.1
 * @date 2025-04-28
 * 
 * @copyright Copyright
 */

 #ifndef RSMP_MESSAGES_STATUS_REQUEST_ACK_INCLUDED_H
 #define RSMP_MESSAGES_STATUS_REQUEST_ACK_INCLUDED_H
 
 #include "rsmp/fields/fields.h"
 #include "rsmp/fields/type.h"
 #include "rsmp/fields/mId.h"
 #include "rsmp/fields/cId.h"
 #include "rsmp/fields/ntsOId.h"
 #include "rsmp/fields/xNId.h"
#include "rsmp/fields/mType.h"
#include "rsmp/fields/sS-array.h"
#include "rsmp/fields/ackType.h"
#include "cmd-request.h"
#include "rsmp/fields/ackMessage.h"
 
 /**
  * @defgroup status-request-ack-included
  * 
  * @brief Message StatusRequestAckIncluded
  * @{
  */
 
 #ifdef RSMP_MESSAGES_STATUS_REQUEST_ACK_INCLUDED_C
 #define STATUS_REQUEST_ACK_INCLUDED_EXTERN
 #define STATUS_REQUEST_ACK_INCLUDED_PRIVATE
 #endif
 
 #ifndef STATUS_REQUEST_ACK_INCLUDED_EXTERN
 #define STATUS_REQUEST_ACK_INCLUDED_EXTERN extern
 #endif
 
 #ifndef STATUS_REQUEST_ACK_INCLUDED_PRIVATE
 #define STATUS_REQUEST_ACK_INCLUDED_PRIVATE const
 #endif
 
 /**
  * @brief Structure représentant un message StatusRequestAckIncluded.
  */
 typedef struct rsmp_msg_StatusRequestAckIncluded
 {
     type_t            type;      /**< Type du message */
     mId_t             mId;       /**< Identifiant unique du message */
     ntsOId_t          ntsOId;    /**< Identifiant de l'objet cible */
     xNId_t            xNId;      /**< Identifiant supplémentaire */
     cId_t             cId;       /**< Identifiant de commande */
     rsmp_array_sSs_t  sS;        /**< Tableau des signaux demandés */
     mType_t           mType;     /**< Type de message */
     ack_t              ack;       /**< Indique si la requête est acquittée */
     char              ackMessage[256]; /**< Message d'acquittement ou d'erreur */
 } rsmp_msg_StatusRequestAckIncluded_t;
 
 /**
  * @brief Alloue et initialise un message StatusRequestAckIncluded.
  * 
  * @param rsmp_context Contexte RSMP global ou spécifique.
  * @return Pointeur vers une nouvelle instance ou NULL en cas d'erreur.
  */
 rsmp_msg_StatusRequestAckIncluded_t *rsmp_msg_StatusRequestAckIncluded_new(rsmp_context_t const *rsmp_context);
 
 /**
  * @brief Libère la mémoire associée à un message StatusRequestAckIncluded.
  * 
  * @param item Pointeur vers l'instance à libérer.
  */
 void rsmp_msg_StatusRequestAckIncluded_free(rsmp_msg_StatusRequestAckIncluded_t *item);

/**
 * @brief Fonction interne pour allouer un message StatusRequestAckIncluded.
 * 
 * @param rsmp_context Contexte RSMP global ou spécifique.
 * @return Pointeur vers une nouvelle instance ou NULL en cas d'erreur.
 */
void *_rsmp_msg_StatusRequestAckIncluded_new(rsmp_context_t const *rsmp_context);

/**
 * @brief Fonction interne pour libérer un message StatusRequestAckIncluded.
 * 
 * @param item Pointeur vers l'instance à libérer.
 */
void _rsmp_msg_StatusRequestAckIncluded_free(void *item);

 
 /**
  * @brief Exécute une commande reçue dans un message StatusRequestAckIncluded.
  * 
  * @param engine Pointeur vers le moteur RSMP.
  * @param attrib Attributs supplémentaires pour l'exécution.
  * @param context Contexte spécifique pour l'exécution.
  * @param msg Pointeur vers le message StatusRequestAckIncluded à exécuter.
  * @return Code d'erreur ou succès.
  */
 int _rsmp_msg_StatusRequestAckIncluded_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);

 STATUS_REQUEST_ACK_INCLUDED_EXTERN STATUS_REQUEST_ACK_INCLUDED_PRIVATE rsmp_field_desc_t _rsmp_msg_StatusRequestAckIncluded_desc[];
 
 /**@}*/
 #endif // RSMP_MESSAGES_STATUS_REQUEST_ACK_INCLUDED_H