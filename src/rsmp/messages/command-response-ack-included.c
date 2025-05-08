/**
 * @file command-response-ack-included.c
 * @brief Implémentation du message CommandResponse avec Ack intégré.
 * @version 0.1
 * @date 2023-07-03
 * 
 * @details Ce fichier contient l'implémentation des fonctions associées au message
 *          CommandResponseAckIncluded, utilisé pour envoyer des réponses avec un
 *          acquittement intégré dans le protocole RSMP.
 */

 #include "config.h"
 #define RSMP_MESSAGES_CMD_RESPONSE_ACK_INCLUDED_C
 
 #include "command-response-ack-included.h"
 #include "command-request-ack-included.h" // Inclusion du fichier d'en-tête correspondant
 #include "rsmp/coder/json.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>


#include "messages.h"
#include "messageack.h"
#include "rsmp/fields/fields.h"      // Définitions générales des champs RSMP
#include "messagenotack.h"
#include "cmd-response.h"
#include "rsmp/sxl/sxl.h"       // Pour la fonction rsmp_sxl_cmd_exe
//#include "rsmp/engine/engine.h" // Pour la fonction rsmp_engine_send_msg


rsmp_field_desc_t _rsmp_msg_CommandResponseAckIncluded_desc[] =
{
    rsmp_field_desc(rsmp_msg_CommandResponseAckIncluded_t, mType),       /*!< Type de message RSMP */
    rsmp_field_desc(rsmp_msg_CommandResponseAckIncluded_t, type),        /*!< Type spécifique du message */
    rsmp_field_desc(rsmp_msg_CommandResponseAckIncluded_t, mId),         /*!< Identifiant unique du message */
    rsmp_field_desc(rsmp_msg_CommandResponseAckIncluded_t, ntsOId),      /*!< Identifiant de l'objet cible */
    rsmp_field_desc(rsmp_msg_CommandResponseAckIncluded_t, xNId),        /*!< Identifiant supplémentaire */
    rsmp_field_desc(rsmp_msg_CommandResponseAckIncluded_t, cId),         /*!< Identifiant de commande */
    rsmp_field_desc(rsmp_msg_CommandResponseAckIncluded_t, cTS),         /*!< Horodatage de la commande */
    rsmp_field_desc_array(rsmp_msg_CommandResponseAckIncluded_t, rvs, _rsmp_msg_rvs_desc), /*!< Tableau des résultats */
    rsmp_field_desc(rsmp_msg_CommandResponseAckIncluded_t, ack),         /*!< Indicateur d'acquittement (Ack ou Not Ack) */
    rsmp_field_desc(rsmp_msg_CommandResponseAckIncluded_t, ackMessage),  /*!< Message d'acquittement ou d'erreur */
    rsmp_field_desc_end(),                                               /*!< Fin des champs */
};


 /**
 * @brief Crée un nouveau message CommandResponseAckIncluded.
 * 
 * @param rsmp_context Contexte RSMP global ou spécifique.
 * @return Pointeur vers une nouvelle instance de rsmp_msg_CommandResponseAckIncluded_t.
 */
rsmp_msg_CommandResponseAckIncluded_t *rsmp_msg_CommandResponseAckIncluded_new(rsmp_context_t const *rsmp_context)
{
    (void)rsmp_context; // Marque le paramètre comme inutilisé pour éviter l'avertissement

    rsmp_msg_CommandResponseAckIncluded_t *item = calloc(1, sizeof(*item));
    if (item)
    {
        item->type = _rsmp_msg_CommandResponseAckIncluded_;
        rsmp_mId_new(item->mId); // Génère un nouvel identifiant unique
        rsmp_cTS_set(&item->cTS); // Initialise l'horodatage
        rsmp_array_rvs_declare(&item->rvs); // Initialise le tableau des résultats
        item->ack = false; // Par défaut, pas d'acquittement
        memset(item->ackMessage, 0, sizeof(item->ackMessage)); // Initialise le message d'acquittement
    }
    return item;
}


/**
 * @brief Libère la mémoire associée à un message CommandResponseAckIncluded.
 * 
 * @param item Pointeur vers l'instance à libérer.
 */
void rsmp_msg_CommandResponseAckIncluded_free(rsmp_msg_CommandResponseAckIncluded_t *item)
{
    if (item)
    {
        rsmp_array_rvs_flush(&item->rvs); // Libère les résultats
        free(item); // Libère la structure principale
    }
}


/**
 * @brief Fonction interne pour créer un message CommandResponseAckIncluded.
 * 
 * @param rsmp_context Contexte RSMP global ou spécifique.
 * @return Pointeur générique vers une nouvelle instance de rsmp_msg_CommandResponseAckIncluded_t.
 */
void *_rsmp_msg_CommandResponseAckIncluded_new(rsmp_context_t const *rsmp_context)
{
    return (void *)rsmp_msg_CommandResponseAckIncluded_new(rsmp_context);
}

/**
 * @brief Fonction interne pour libérer un message CommandResponseAckIncluded.
 * 
 * @param item Pointeur générique vers l'instance à libérer.
 */
void _rsmp_msg_CommandResponseAckIncluded_free(void *item)
{
    rsmp_msg_CommandResponseAckIncluded_free((rsmp_msg_CommandResponseAckIncluded_t *)item);
}

/**
 * @brief Crée une réponse CommandResponseAckIncluded à partir d'une requête CommandRequestAckIncluded.
 * 
 * @param request Pointeur constant vers la requête CommandRequestAckIncluded.
 * @return Pointeur vers une nouvelle instance de rsmp_msg_CommandResponseAckIncluded_t, ou NULL en cas d'erreur.
 */
rsmp_msg_CommandResponseAckIncluded_t *rsmp_msg_CommandResponseAckIncluded_from_request(rsmp_msg_CommandRequestAckIncluded_t const *request)
{
    // Allouer et initialiser une nouvelle réponse
    rsmp_msg_CommandResponseAckIncluded_t *response = rsmp_msg_CommandResponseAckIncluded_new(NULL);
    if (!response) // Vérifier si l'allocation a échoué
        return NULL;

    // Copier les champs pertinents de la requête vers la réponse
    rsmp_ntsOId_set(&response->ntsOId, request->ntsOId); // Copier l'identifiant de l'objet cible
    rsmp_xNId_set(&response->xNId, request->xNId);       // Copier l'identifiant supplémentaire
    rsmp_cId_set(&response->cId, request->cId);          // Copier l'identifiant de commande
    rsmp_cTS_set(&response->cTS);                        // Initialiser l'horodatage de la commande
    rsmp_array_rvs_initialize(&response->rvs, request->arg.len); // Initialiser le tableau des résultats avec la taille des arguments

    // Copier les arguments de la requête dans les résultats de la réponse
    for (size_t i = 0; i < response->rvs.len; i++)
    {
        rsmp_cCI_set(&response->rvs.items[i].cCI, request->arg.items[i].cCI); // Copier l'identifiant de commande
        rsmp_n_set(&response->rvs.items[i].n, request->arg.items[i].n);       // Copier le numéro de commande
        rsmp_array_char_flush(&response->rvs.items[i].v);                     // Réinitialiser la valeur associée
    }

    // Initialiser les champs spécifiques à AckIncluded
    response->ack = true; // Par défaut, on suppose un succès
    snprintf(response->ackMessage, sizeof(response->ackMessage), "Initialized from request"); // Message par défaut

    return response; // Retourner la réponse créée
}

/**
 * @brief Exécute une commande reçue dans un message CommandRequestAckIncluded et génère une réponse CommandResponseAckIncluded.
 * 
 * @param engine Pointeur vers le moteur RSMP.
 * @param attrib Attributs supplémentaires pour l'exécution.
 * @param context Contexte spécifique pour l'exécution.
 * @param msg Pointeur vers le message CommandRequestAckIncluded à exécuter.
 * @return Code d'erreur ou succès.
 */
int _rsmp_msg_CommandResponseAckIncluded_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg)
{
    // Extraire la requête CommandRequestAckIncluded
    rsmp_msg_CommandRequestAckIncluded_t *request = msg.CommandRequestAckIncluded;

    // Créer une réponse à partir de la requête
    rsmp_msg_CommandResponseAckIncluded_t *response = rsmp_msg_CommandResponseAckIncluded_from_request(request);
    if (!response) // Vérifier si la création de la réponse a échoué
        return _rsmp_errcod_gen_generic_; // Retourner une erreur générique

    // Traiter les résultats de la requête
    for (size_t i = 0; i < request->arg.len; i++)
    {
        // Exécuter la commande pour chaque argument
        int ret = rsmp_sxl_cmd_exe(context, &request->arg.items[i], &response->rvs.items[i]);
        if (ret != _rsmp_no_error_) // Vérifier si une erreur s'est produite
        {
            response->ack = false; // Indiquer un échec
            snprintf(response->ackMessage, sizeof(response->ackMessage), "Error processing argument %zu", i); // Ajouter un message d'erreur
            break; // Arrêter le traitement en cas d'erreur
        }
    }

    // Envoyer la réponse via le moteur RSMP
    int send_ret = rsmp_engine_send_msg(engine, response, attrib);
    if (send_ret != _rsmp_no_error_) // Vérifier si l'envoi a échoué
    {
        rsmp_msg_CommandResponseAckIncluded_free(response); // Libérer la mémoire en cas d'échec
        return send_ret; // Retourner l'erreur d'envoi
    }

    // Libérer la mémoire de la réponse après envoi
    rsmp_msg_CommandResponseAckIncluded_free(response);

    return _rsmp_no_error_; // Retourner le succès
}