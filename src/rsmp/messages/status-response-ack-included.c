/**
 * @file status-response-ack-included.c
 * @brief Implémentation des fonctions pour StatusResponseAckIncluded.
 * @version 0.1
 * @date 2025-04-28
 */

 #include "config.h"
 #define RSMP_MESSAGES_STATUS_RESPONSE_ACK_INCLUDED_C
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>
 
 #include "messages.h"
 #include "status-response-ack-included.h"
 #include "rsmp/coder/json.h"
 
 /**
  * @ingroup status-response-ack-included
  * @{
  */
 
 // Description des champs du tableau sS
 static rsmp_field_desc_t _rsmp_msg_sS_desc[] =
 {
     rsmp_field_desc(sSs, sCI),
     rsmp_field_desc(sSs, n),
     rsmp_field_desc(sSs, s),
     rsmp_field_desc(sSs, q),
     rsmp_field_desc_end(),
 };
 
 // Description des champs du message StatusResponseAckIncluded
 rsmp_field_desc_t _rsmp_msg_StatusResponseAckIncluded_desc[] =
 {
     rsmp_field_desc(rsmp_msg_StatusResponseAckIncluded_t, mType),
     rsmp_field_desc(rsmp_msg_StatusResponseAckIncluded_t, type),
     rsmp_field_desc(rsmp_msg_StatusResponseAckIncluded_t, mId),
     rsmp_field_desc(rsmp_msg_StatusResponseAckIncluded_t, ntsOId),
     rsmp_field_desc(rsmp_msg_StatusResponseAckIncluded_t, xNId),
     rsmp_field_desc(rsmp_msg_StatusResponseAckIncluded_t, cId),
     rsmp_field_desc(rsmp_msg_StatusResponseAckIncluded_t, sTs),
     rsmp_field_desc_array(rsmp_msg_StatusResponseAckIncluded_t, sS, _rsmp_msg_sS_desc),
     rsmp_field_desc(rsmp_msg_StatusResponseAckIncluded_t, ack),
     rsmp_field_desc(rsmp_msg_StatusResponseAckIncluded_t, ackMessage),
     rsmp_field_desc_end(),
 };
 /**@}*/
 
 /**
  * @brief Alloue et initialise un message StatusResponseAckIncluded.
  */
 rsmp_msg_StatusResponseAckIncluded_t *rsmp_msg_StatusResponseAckIncluded_new(rsmp_context_t const *rsmp_context)
 {
     return (rsmp_msg_StatusResponseAckIncluded_t *)_rsmp_msg_StatusResponseAckIncluded_new(rsmp_context);
 }
 
 /**
  * @brief Fonction interne pour allouer un message StatusResponseAckIncluded.
  */
 void *_rsmp_msg_StatusResponseAckIncluded_new(rsmp_context_t const *rsmp_context)
 {
     rsmp_msg_StatusResponseAckIncluded_t *item = calloc(1, sizeof(*item));
     if (item)
     {
         item->type = _rsmp_msg_StatusResponseAckIncluded_;
         rsmp_mId_new(item->mId);
         rsmp_sTs_set(&item->sTs);
         rsmp_array_sSs_declare(&item->sS);
         item->ack = true;
         snprintf(item->ackMessage, sizeof(item->ackMessage), "Initialized");
         if (!rsmp_context) rsmp_context = rsmp_context_global_get();
         if (rsmp_context) rsmp_context_set(item, rsmp_context);
     }
     return item;
 }
 
 /**
  * @brief Libère la mémoire associée à un message StatusResponseAckIncluded.
  */
 void rsmp_msg_StatusResponseAckIncluded_free(rsmp_msg_StatusResponseAckIncluded_t *item)
 {
     _rsmp_msg_StatusResponseAckIncluded_free(item);
 }
 
 /**
  * @brief Fonction interne pour libérer un message StatusResponseAckIncluded.
  */
 void _rsmp_msg_StatusResponseAckIncluded_free(void *item)
 {
     if (item)
     {
         rsmp_msg_StatusResponseAckIncluded_t *msg = (rsmp_msg_StatusResponseAckIncluded_t *)item;
         rsmp_ntsOId_free(&msg->ntsOId);
         rsmp_xNId_free(&msg->xNId);
         rsmp_cId_free(&msg->cId);
         rsmp_array_sSs_flush(&msg->sS);
         free(msg);
     }
 }
 
 /**
  * @brief Exécute une commande reçue dans un message StatusResponseAckIncluded.
  */
 int _rsmp_msg_StatusResponseAckIncluded_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg)
 {
     rsmp_msg_StatusResponseAckIncluded_t *response = msg.StatusResponseAckIncluded;
     assert(response != NULL);
 
     // Ici, on pourrait vérifier la validité des champs, etc.
     // Exemple : si un champ obligatoire est manquant, on met NotAck
     bool error_found = false;
     if (response->sS.len == 0) {
         response->ack = false;
         snprintf(response->ackMessage, sizeof(response->ackMessage), "No status signals provided");
         error_found = true;
     }
 
     // Génère le payload JSON à partir de la réponse
     struct json_object *json_sS_array = json_object_new_array();
     for (size_t i = 0; i < response->sS.len; i++)
     {
         sSs *elt = &response->sS.items[i];
         struct json_object *jobj = json_object_new_object();
         json_object_object_add(jobj, "sCI", json_object_new_string(elt->sCI));
         json_object_object_add(jobj, "n", json_object_new_string(elt->n));
         json_object_object_add(jobj, "s", json_object_new_string(elt->s));
         // Correction ici pour le champ q :
         char q_str[16];
         snprintf(q_str, sizeof(q_str), "%d", elt->q);
         json_object_object_add(jobj, "q", json_object_new_string(q_str));
         json_object_array_add(json_sS_array, jobj);
     }
     char *payload = rsmp_json_msg2payload_with_inserting_jobj(msg, "sS", json_sS_array);
 
     if (payload && engine && !error_found)
     {
         engine->send(engine, payload, strlen(payload), attrib);
         response->ack = true;
         snprintf(response->ackMessage, sizeof(response->ackMessage), "StatusResponseAckIncluded sent successfully");
     }
     else
     {
         response->ack = false;
         snprintf(response->ackMessage, sizeof(response->ackMessage), "Failed to send StatusResponseAckIncluded");
     }
 
     return response->ack ? _rsmp_no_error_ : _rsmp_errcod_gen_generic_;
 }
 
 /**@}*/