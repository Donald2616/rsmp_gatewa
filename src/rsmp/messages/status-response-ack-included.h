/**
 * @file status-response-ack-included.h
 * @brief Déclaration des structures et fonctions pour StatusResponseAckIncluded.
 * @version 0.1
 * @date 2025-04-28
 * 
 * @copyright Copyright
 */

 #ifndef RSMP_MESSAGES_STATUS_RESPONSE_ACK_INCLUDED_H
 #define RSMP_MESSAGES_STATUS_RESPONSE_ACK_INCLUDED_H
 
 #include "rsmp/fields/fields.h"
 #include "rsmp/fields/type.h"
 #include "rsmp/fields/mId.h"
 #include "rsmp/fields/cId.h"
 #include "rsmp/fields/ntsOId.h"
 #include "rsmp/fields/xNId.h"
 #include "rsmp/fields/mType.h"
 #include "rsmp/fields/sTs.h"
 #include "rsmp/fields/sS-array.h"
 #include "rsmp/fields/ackType.h"
 #include "rsmp/fields/ackMessage.h"
 
 #ifdef RSMP_MESSAGES_STATUS_RESPONSE_ACK_INCLUDED_C
 #define STATUS_RESPONSE_ACK_INCLUDED_EXTERN
 #define STATUS_RESPONSE_ACK_INCLUDED_PRIVATE
 #endif
 
 #ifndef STATUS_RESPONSE_ACK_INCLUDED_EXTERN
 #define STATUS_RESPONSE_ACK_INCLUDED_EXTERN extern
 #endif
 
 #ifndef STATUS_RESPONSE_ACK_INCLUDED_PRIVATE
 #define STATUS_RESPONSE_ACK_INCLUDED_PRIVATE const
 #endif
 
 /**
  * @defgroup status-response-ack-included
  * @brief Message StatusResponseAckIncluded
  * @{
  */
 
 /**
  * @brief Structure représentant un message StatusResponseAckIncluded.
  */
 typedef struct rsmp_msg_StatusResponseAckIncluded
 {
     type_t            type;      /**< Type du message */
     mId_t             mId;       /**< Identifiant unique du message */
     ntsOId_t          ntsOId;    /**< Identifiant de l'objet cible */
     xNId_t            xNId;      /**< Identifiant supplémentaire */
     cId_t             cId;       /**< Identifiant de commande */
     sTs_t             sTs;       /**< Horodatage du statut */
     rsmp_array_sSs_t  sS;        /**< Tableau des statuts */
     mType_t           mType;     /**< Type de message */
     ack_t             ack;       /**< Indique si la réponse est acquittée */
     char              ackMessage[256]; /**< Message d'acquittement ou d'erreur */
 } rsmp_msg_StatusResponseAckIncluded_t;
 
 /**
  * @brief Alloue et initialise un message StatusResponseAckIncluded.
  * @param rsmp_context Contexte RSMP global ou spécifique.
  * @return Pointeur vers une nouvelle instance ou NULL en cas d'erreur.
  */
 rsmp_msg_StatusResponseAckIncluded_t *rsmp_msg_StatusResponseAckIncluded_new(rsmp_context_t const *rsmp_context);
 
 /**
  * @brief Libère la mémoire associée à un message StatusResponseAckIncluded.
  * @param item Pointeur vers l'instance à libérer.
  */
 void rsmp_msg_StatusResponseAckIncluded_free(rsmp_msg_StatusResponseAckIncluded_t *item);
 
 /**
  * @brief Fonction interne pour allouer un message StatusResponseAckIncluded.
  * @param rsmp_context Contexte RSMP global ou spécifique.
  * @return Pointeur vers une nouvelle instance ou NULL en cas d'erreur.
  */
 void *_rsmp_msg_StatusResponseAckIncluded_new(rsmp_context_t const *rsmp_context);
 
 /**
  * @brief Fonction interne pour libérer un message StatusResponseAckIncluded.
  * @param item Pointeur vers l'instance à libérer.
  */
 void _rsmp_msg_StatusResponseAckIncluded_free(void *item);
 
 /**
  * @brief Exécute une commande reçue dans un message StatusResponseAckIncluded.
  * @param engine Pointeur vers le moteur RSMP.
  * @param attrib Attributs supplémentaires pour l'exécution.
  * @param context Contexte spécifique pour l'exécution.
  * @param msg Pointeur vers le message StatusResponseAckIncluded à exécuter.
  * @return Code d'erreur ou succès.
  */
 int _rsmp_msg_StatusResponseAckIncluded_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);
 
 STATUS_RESPONSE_ACK_INCLUDED_EXTERN STATUS_RESPONSE_ACK_INCLUDED_PRIVATE rsmp_field_desc_t _rsmp_msg_StatusResponseAckIncluded_desc[];
 
 /**@}*/
 #endif // RSMP_MESSAGES_STATUS_RESPONSE_ACK_INCLUDED_H