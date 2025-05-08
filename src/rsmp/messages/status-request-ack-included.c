/**
 * @file status-request-ack-included.c
 * @brief Implémentation des fonctions pour StatusRequestAckIncluded.
 * @version 0.1
 * @date 2025-04-28
 * 
 * @details Ce fichier contient l'implémentation des fonctions associées au message
 *          StatusRequestAckIncluded, utilisé pour envoyer des requêtes de statut
 *          avec un acquittement intégré dans le protocole RSMP.
 */

 #include "config.h"
 #define RSMP_MESSAGES_STATUS_REQUEST_ACK_INCLUDED_C
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>
 
 #include "messages.h"
 #include "status-request-ack-included.h"
 #include "stat-response.h"
 #include "rsmp/coder/json.h"

 /**
 * @ingroup status-request-ack-included
 * @{
 */

// Description des champs du tableau sS
static rsmp_field_desc_t _rsmp_msg_sS_desc[] =
{
    rsmp_field_desc(sSs, sCI),
    rsmp_field_desc(sSs, n),
    rsmp_field_desc_end(),
};

// Description des champs du message StatusRequestAckIncluded
rsmp_field_desc_t _rsmp_msg_StatusRequestAckIncluded_desc[] =
{
    rsmp_field_desc(rsmp_msg_StatusRequestAckIncluded_t, mType),
    rsmp_field_desc(rsmp_msg_StatusRequestAckIncluded_t, type),
    rsmp_field_desc(rsmp_msg_StatusRequestAckIncluded_t, mId),
    rsmp_field_desc(rsmp_msg_StatusRequestAckIncluded_t, ntsOId),
    rsmp_field_desc(rsmp_msg_StatusRequestAckIncluded_t, xNId),
    rsmp_field_desc(rsmp_msg_StatusRequestAckIncluded_t, cId),
    rsmp_field_desc_array(rsmp_msg_StatusRequestAckIncluded_t, sS, _rsmp_msg_sS_desc),
    rsmp_field_desc(rsmp_msg_StatusRequestAckIncluded_t, ack),
    rsmp_field_desc(rsmp_msg_StatusRequestAckIncluded_t, ackMessage),
    rsmp_field_desc_end(),
};
/**@}*/
 
 /**
  * @ingroup status-request-ack-included
  * @{
  */
 
 /**
  * @brief Alloue et initialise un message StatusRequestAckIncluded.
  * 
  * @param rsmp_context Contexte RSMP global ou spécifique.
  * @return Pointeur vers une nouvelle instance ou NULL en cas d'erreur.
  */
 rsmp_msg_StatusRequestAckIncluded_t *rsmp_msg_StatusRequestAckIncluded_new(rsmp_context_t const *rsmp_context)
 {
     // Appelle la fonction interne pour l'allocation
     return (rsmp_msg_StatusRequestAckIncluded_t *)_rsmp_msg_StatusRequestAckIncluded_new(rsmp_context);
 }
 
 /**
  * @brief Fonction interne pour allouer un message StatusRequestAckIncluded.
  * 
  * @param rsmp_context Contexte RSMP global ou spécifique.
  * @return Pointeur vers une nouvelle instance ou NULL en cas d'erreur.
  */
 void *_rsmp_msg_StatusRequestAckIncluded_new(rsmp_context_t const *rsmp_context)
 {
     // Alloue la mémoire pour la structure
     rsmp_msg_StatusRequestAckIncluded_t *item = calloc(1, sizeof(*item));
     if (item)
     {
         item->type = _rsmp_msg_StatusRequestAckIncluded_; // Définit le type du message
         rsmp_mId_new(item->mId);                         // Génère un nouvel identifiant unique
         rsmp_array_sSs_declare(&item->sS);               // Initialise le tableau des signaux demandés
         item->ack = true;                                // Par défaut, la requête est acquittée (Ack)
         snprintf(item->ackMessage, sizeof(item->ackMessage), "Initialized"); // Message par défaut
         if (!rsmp_context) rsmp_context = rsmp_context_global_get(); // Utilise le contexte global si non fourni
         if (rsmp_context) rsmp_context_set(item, rsmp_context);      // Associe le contexte à la structure
     }
     return item;
 }
 
 /**
  * @brief Libère la mémoire associée à un message StatusRequestAckIncluded.
  * 
  * @param item Pointeur vers l'instance à libérer.
  */
 void rsmp_msg_StatusRequestAckIncluded_free(rsmp_msg_StatusRequestAckIncluded_t *item)
 {
     // Appelle la fonction interne pour la libération
     _rsmp_msg_StatusRequestAckIncluded_free(item);
 }
 
 /**
  * @brief Fonction interne pour libérer un message StatusRequestAckIncluded.
  * 
  * @param item Pointeur vers l'instance à libérer.
  */
 void _rsmp_msg_StatusRequestAckIncluded_free(void *item)
 {
     if (item)
     {
         rsmp_msg_StatusRequestAckIncluded_t *msg = (rsmp_msg_StatusRequestAckIncluded_t *)item;
         rsmp_ntsOId_free(&msg->ntsOId);      // Libère l'identifiant de l'objet cible
         rsmp_xNId_free(&msg->xNId);          // Libère l'identifiant supplémentaire
         rsmp_cId_free(&msg->cId);            // Libère l'identifiant de commande
         rsmp_array_sSs_flush(&msg->sS);      // Libère le tableau des signaux
         free(msg);                           // Libère la structure elle-même
     }
 }
 
 /**
  * @brief Exécute une commande reçue dans un message StatusRequestAckIncluded.
  * 
  * Cette fonction traite les signaux demandés dans le message, génère une réponse,
  * et met à jour les champs d'acquittement (Ack ou NotAck) en fonction du succès
  * ou de l'échec de l'exécution.
  * 
  * @param engine Pointeur vers le moteur RSMP.
  * @param attrib Attributs supplémentaires pour l'exécution.
  * @param context Contexte spécifique pour l'exécution.
  * @param msg Pointeur vers le message StatusRequestAckIncluded à exécuter.
  * @return Code d'erreur ou succès.
  */
 int _rsmp_msg_StatusRequestAckIncluded_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg)
 {
     // Récupère la requête à partir du message
     rsmp_msg_StatusRequestAckIncluded_t *request = msg.StatusRequestAckIncluded;
     assert(request != NULL);
 
     _log_info("StatusRequestAckIncluded received");
     // Affiche les signaux demandés pour le debug
     for (size_t i = 0; i < request->sS.len; i++)
     {
         _log_info("sCI: %s, n: %s", request->sS.items[i].sCI, request->sS.items[i].n);
     }
 
     // Crée un message de réponse de type StatusResponse
     rsmp_messages_ptr msgResponse = rsmp_message_new(NULL, _rsmp_msg_StatusResponse_);
     rsmp_msg_StatusResponse_t *response = msgResponse.StatusResponse;
 
     // Prépare un tableau JSON pour les résultats des signaux
     struct json_object *json_sS_array = json_object_new_array();
     bool error_found = false; // Indique si une erreur a été rencontrée
 
     // Pour chaque signal demandé, ajoute une réponse dans le tableau JSON
     for (size_t i = 0; i < request->sS.len; i++)
     {
         struct json_object *jobj = json_object_new_object();
         json_object_object_add(jobj, "sCI", json_object_new_string(request->sS.items[i].sCI));
         json_object_object_add(jobj, "n", json_object_new_string(request->sS.items[i].n));
         json_object_object_add(jobj, "q", json_object_new_string("recent"));
 
         // Exemple de gestion d'erreur : si le nom du signal est inconnu, on met NotAck
         if (strcmp(request->sS.items[i].n, "inputstatus") == 0)
         {
             json_object_object_add(jobj, "s", json_object_new_string("100101"));
         }
         else if (strcmp(request->sS.items[i].n, "extendedinputstatus") == 0)
         {
             json_object_object_add(jobj, "s", json_object_new_string("100100101"));
         }
         else
         {
             // Signal inconnu, on marque une erreur
             json_object_object_add(jobj, "s", json_object_new_string("error"));
             error_found = true;
         }
         json_object_array_add(json_sS_array, jobj);
     }
 
     // Génère le payload JSON à partir de la réponse et du tableau des signaux
     char *payload = rsmp_json_msg2payload_with_inserting_jobj(msgResponse, "sS", json_sS_array);
 
     // Envoie la réponse si possible
     if (payload && engine)
     {
         engine->send(engine, payload, strlen(payload), attrib);
         // Si tout s'est bien passé, Ack
         request->ack = true;
         snprintf(request->ackMessage, sizeof(request->ackMessage), "StatusRequestAckIncluded processed successfully");
     }
     else
     {
         // En cas d'échec d'envoi, NotAck
         request->ack = false;
         snprintf(request->ackMessage, sizeof(request->ackMessage), "Failed to send StatusRequestAckIncluded response");
     }
 
     // Si une erreur a été détectée dans les signaux, NotAck
     if (error_found)
     {
         request->ack = false;
         snprintf(request->ackMessage, sizeof(request->ackMessage), "One or more signals are invalid");
     }
 
     // Libère la mémoire de la requête
     rsmp_msg_StatusRequestAckIncluded_free(request);
 
     // Retourne le code de succès ou d'erreur
     return request->ack ? _rsmp_no_error_ : _rsmp_errcod_gen_generic_;
 }
 
 /**@}*/