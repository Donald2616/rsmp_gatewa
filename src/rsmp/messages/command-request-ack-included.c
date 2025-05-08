/**
 * @file command-request-ack-included.c
 * @brief Implémentation du message CommandRequest avec Ack intégré.
 * @version 0.1
 * @date 2023-07-03
 * 
 * @details Ce fichier contient l'implémentation des fonctions associées au message
 *          CommandRequestAckIncluded, utilisé pour envoyer des commandes avec un
 *          acquittement intégré dans le protocole RSMP.
 */
#include "config.h"
#define RSMP_MESSAGES_CMD_REQUEST_ACK_INCLUDED_C

#include "rsmp/coder/json.h"
#include "command-request-ack-included.h" // Inclusion du fichier d'en-tête correspondant
#include <stdio.h>
#include <stdlib.h>                     // Pour les fonctions calloc et free
#include <string.h>                     // Pour memset
#include <assert.h>
#include "messages.h"
#include "messageack.h"
#include "messagenotack.h"
#include "cmd-response.h"
#include "rsmp/sxl/sxl.h"

#include "cmd-request.h"
#include "rsmp/coder/json.h"

/**
 * @ingroup cmd-request
 * @{
 */

 /*
 rsmp_field_desc_t _rsmp_msg_arg_desc[] =
 {
     rsmp_field_desc(args, cCI),
     rsmp_field_desc(args, n),
     rsmp_field_desc(args, cO),
     rsmp_field_desc(args, v),
     rsmp_field_desc_end(),
 };
 */
 

rsmp_field_desc_t _rsmp_msg_CommandRequestAckIncluded_desc[] = {
    rsmp_field_desc(rsmp_msg_CommandRequestAckIncluded_t, mType),
    rsmp_field_desc(rsmp_msg_CommandRequestAckIncluded_t, type),
    rsmp_field_desc(rsmp_msg_CommandRequestAckIncluded_t, mId),
    rsmp_field_desc(rsmp_msg_CommandRequestAckIncluded_t, ntsOId),
    rsmp_field_desc(rsmp_msg_CommandRequestAckIncluded_t, xNId),
    rsmp_field_desc(rsmp_msg_CommandRequestAckIncluded_t, cId),
    rsmp_field_desc_array(rsmp_msg_CommandRequestAckIncluded_t, arg, _rsmp_msg_arg_desc),
    rsmp_field_desc(rsmp_msg_CommandRequestAckIncluded_t, ack),
    rsmp_field_desc(rsmp_msg_CommandRequestAckIncluded_t, ackMessage),
    rsmp_field_desc_end(),
};

/**
 * @brief Crée un nouveau message CommandRequestAckIncluded.
 * 
 * @param rsmp_context Contexte RSMP global ou spécifique.
 * @return Pointeur vers une nouvelle instance de rsmp_msg_CommandRequestAckIncluded_t.
 */
rsmp_msg_CommandRequestAckIncluded_t *rsmp_msg_CommandRequestAckIncluded_new(rsmp_context_t const *rsmp_context)
{
    (void)rsmp_context; // Marque le paramètre comme inutilisé pour éviter l'avertissement

    rsmp_msg_CommandRequestAckIncluded_t *item = calloc(1, sizeof(*item));
    if (item)
    {
        item->type = _rsmp_msg_CommandRequestAckIncluded_;
        rsmp_mId_new(item->mId); // Génère un nouvel identifiant unique
        rsmp_array_args_declare(&item->arg); // Initialise le tableau des arguments
        item->ack = false; // Par défaut, pas d'acquittement
        memset(item->ackMessage, 0, sizeof(item->ackMessage)); // Initialise le message d'acquittement
    }
    return item;
}

/**
 * @brief Libère la mémoire associée à un message CommandRequestAckIncluded.
 * 
 * @param item Pointeur vers l'instance à libérer.
 */
void rsmp_msg_CommandRequestAckIncluded_free(rsmp_msg_CommandRequestAckIncluded_t *item)
{
    if (item)
    {
        rsmp_array_args_flush(&item->arg); // Libère les arguments
        free(item); // Libère la structure principale
    }
}

/**
 * @brief Fonction interne pour créer un message CommandRequestAckIncluded.
 * 
 * @param rsmp_context Contexte RSMP global ou spécifique.
 * @return Pointeur générique vers une nouvelle instance de rsmp_msg_CommandRequestAckIncluded_t.
 */
void *_rsmp_msg_CommandRequestAckIncluded_new(rsmp_context_t const *rsmp_context)
{
    return (void *)rsmp_msg_CommandRequestAckIncluded_new(rsmp_context);
}


/**
 * @brief Fonction interne pour libérer un message CommandRequestAckIncluded.
 * 
 * @param item Pointeur générique vers l'instance à libérer.
 */
void _rsmp_msg_CommandRequestAckIncluded_free(void *item)
{
    rsmp_msg_CommandRequestAckIncluded_free((rsmp_msg_CommandRequestAckIncluded_t *)item);
}



  /**

 * @brief Exécute une commande reçue dans un message CommandRequestAckIncluded.
 * 
 * @param engine Pointeur vers le moteur RSMP.
 * @param attrib Attributs supplémentaires pour l'exécution.
 * @param context Contexte spécifique pour l'exécution.
 * @param msg Pointeur vers le message CommandRequestAckIncluded à exécuter.
 * @return Code d'erreur ou succès.
 */
int _rsmp_msg_CommandRequestAckIncluded_exe(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg)
{
    int ret = _rsmp_no_error_;
    rsmp_msg_CommandRequestAckIncluded_t *request = msg.CommandRequestAckIncluded;
    assert(request != NULL);
    assert(rsmp_message_is_CommandRequestAckIncluded(msg));

    uuid_str_t mId_str;
    rsmp_oMId_to_string(request->mId, mId_str);

    // Vérification des arguments
    if (request->arg.len)
    {
        for (size_t i = 0; i < request->arg.len; i++)
        {
            if ((ret = rsmp_sxl_cmd_is_active(request->arg.items[i].cCI)) != _rsmp_no_error_)
            {
                _rsmp_log_err(ret, "mid:%s, cCI: %s", mId_str, request->arg.items[i].cCI);

                // Not Ack intégré dans le message
                request->ack = false;
                snprintf(request->ackMessage, sizeof(request->ackMessage), "cCI:%s not allowed/defined", request->arg.items[i].cCI);
                return rsmp_engine_send_msg(engine, msg, attrib);
            }
        }

        // Acquittement positif intégré dans le message
        request->ack = true;
        snprintf(request->ackMessage, sizeof(request->ackMessage), "Command executed successfully");

        // Création du message de réponse
        rsmp_msg_CommandResponse_t *response = rsmp_msg_CommandResponse_from_request((rsmp_msg_CommandRequest_t *)request);
        if (response)
        {
            for (size_t i = 0; i < request->arg.len; i++)
            {
                ret = rsmp_sxl_cmd_exe(context, &request->arg.items[i], &response->rvs.items[i]);
            }
            ret = rsmp_engine_send_msg(engine, response, attrib);
        }
        else
        {
            request->ack = false;
            snprintf(request->ackMessage, sizeof(request->ackMessage), "Failed to create response");
        }
    }
    else
    {
        // Not Ack si aucun argument n'est présent
        ret = _rsmp_errcod_sxl_no_arg_cmd_;
        _rsmp_log_err(ret, "mId:%s", mId_str);

        request->ack = false;
        snprintf(request->ackMessage, sizeof(request->ackMessage), "No arg items provided");
    }

    // Envoi du message avec Ack ou Not Ack intégré
    return rsmp_engine_send_msg(engine, msg, attrib);
}