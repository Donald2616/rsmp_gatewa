/**
* @file stat-response.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"
#define RSMP_MESSAGES_STAT_RESPONSE_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"

#include "stat-response.h"
#include <json-c/json.h>


// ####################################################################""

// Définir la fonction de traitement S1
int fct_GEN_S1(void *context, sSs *elt, json_object **jobj, char **err) {

    *jobj = json_object_new_object();
    if (*jobj)
    {

        if (strcmp(elt->n, "Manufacturer") == 0)
        {
            json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_s_), json_object_new_string("SVMS"));
        }
        else if (strcmp(elt->n, "ProductName") == 0)
        {
            json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_s_), json_object_new_string("PIP"));
        }
        else if (strcmp(elt->n, "SoftwareVersion") == 0)
        {
            json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_s_), json_object_new_string("1.2.3"));
        }
        else if (strcmp(elt->n, "HardwareVersion") == 0)
        {
            json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_s_), json_object_new_string("4.6"));
        }
        else
        {
            _log_err("n: %s not recognized", elt->n);
            json_object_put(*jobj);
            // Supprimer l'élément de la liste
            // Implémentation de la suppression de l'élément de la liste
            // Cela dépend de la structure de la liste et de la gestion de la mémoire
            return -1;
        }
        json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_q_), json_object_new_string("recent"));
        json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_sCI_), json_object_new_string(elt->sCI));
        json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_n_), json_object_new_string(elt->n));
        return 0;
    }

    return -1;
}


// Définir la fonction de traitement S2
int fct_GEN_S2(void *context, sSs *elt, json_object **jobj, char **err) {

    *jobj = json_object_new_object();
    if (*jobj)
    {

        if (strcmp(elt->n, "Name") == 0)
        {
            json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_s_), json_object_new_string("default.conf"));
        }
        else if (strcmp(elt->n, "Version") == 0)
        {
            json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_s_), json_object_new_string("version-dev"));
        }
        else if (strcmp(elt->n, "Timestamp") == 0)
        {
            json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_s_), json_object_new_string("2025-03-24T17:04:05.540Z"));
        }
        else if (strcmp(elt->n, "Location") == 0)
        {
            json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_s_), json_object_new_string("0,0"));
        }
        else
        {
            _log_err("n: %s not recognized", elt->n);
            json_object_put(*jobj);
            // Supprimer l'élément de la liste
            // Implémentation de la suppression de l'élément de la liste
            // Cela dépend de la structure de la liste et de la gestion de la mémoire
            return -1;
        }
        json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_q_), json_object_new_string("recent"));
        json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_sCI_), json_object_new_string(elt->sCI));
        json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_n_), json_object_new_string(elt->n));
        return 0;
    }

    return -1;
}


// Définir la fonction de traitement S100
// int fct_GEN_S100(void *context, sSs *elt, json_object **jobj, char **err) {

//     *jobj = json_object_new_array();
//     if (*jobj)
//     {

//         if (strcmp(elt->n, "Event") == 0)
//         {
//             // ajout d'un tableau vide
//             json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_event_), json_object_new_array());
//         }
//         else
//         {
//             _log_err("n: %s not recognized", elt->n);
//             json_object_put(*jobj);
//             // Supprimer l'élément de la liste
//             // Implémentation de la suppression de l'élément de la liste
//             // Cela dépend de la structure de la liste et de la gestion de la mémoire
//             return -1;
//         }
//         json_object_object_add(*jobj, _rsmp_field_lib_get(_rsmp_field_q_), json_object_new_string("recent"));
//         return 0;
//     }

//     return -1;
// }

// Initialiser le tableau de CallbackEntry
CallbackEntry callbacks[] = {
    {"GEN/S1", fct_GEN_S1},
    {"GEN/S2", fct_GEN_S2},
    // {"GEN/S2", fct_GEN_S100}, \todo déclarer l'élément "Event" dans les fields
    // Ajouter d'autres callbacks ici si nécessaire
};
// \todo évaluer l'implémentation de rsmp_sxl_cmd_exe() par rapport à celle là (doublon)
// ####################################################################""
/**
 * @ingroup stat-response
 * @{
 */

static rsmp_field_desc_t _rsmp_msg_sS_desc[] =
{
    rsmp_field_desc(sSs, sCI),
    rsmp_field_desc(sSs, n),
    rsmp_field_desc(sSs, s),
    rsmp_field_desc(sSs, q),
    rsmp_field_desc_end(),
};


rsmp_field_desc_t _rsmp_msg_StatusResponse_desc[] =
{
    rsmp_field_desc(rsmp_msg_StatusResponse_t, mType),
    rsmp_field_desc(rsmp_msg_StatusResponse_t, type),
    rsmp_field_desc(rsmp_msg_StatusResponse_t, mId),
    rsmp_field_desc(rsmp_msg_StatusResponse_t, ntsOId),
    rsmp_field_desc(rsmp_msg_StatusResponse_t, xNId),
    rsmp_field_desc(rsmp_msg_StatusResponse_t, cId),
    rsmp_field_desc(rsmp_msg_StatusResponse_t, sTs),
    rsmp_field_desc_array(rsmp_msg_StatusResponse_t, sS, _rsmp_msg_sS_desc),
   rsmp_field_desc_end(),
};

rsmp_msg_StatusResponse_t *rsmp_msg_StatusResponse_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_StatusResponse_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_StatusResponse_;
        rsmp_mId_new(item->mId);
        rsmp_sTs_set(&item->sTs);
        rsmp_array_sSs_declare(&item->sS);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(item, rsmp_context);
    }
    return item;
}

void *_rsmp_msg_StatusResponse_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_StatusResponse_new(rsmp_context);
}




void rsmp_msg_StatusResponse_free(rsmp_msg_StatusResponse_t *item)
{
    if (item)
    {
        rsmp_ntsOId_free(&item->ntsOId);
        rsmp_xNId_free(&item->xNId);
        rsmp_cId_free(&item->cId);
        rsmp_array_sSs_flush(&item->sS);
        free(item);
    }
}

void _rsmp_msg_StatusResponse_free(void *item)
{
    rsmp_msg_StatusResponse_free((rsmp_msg_StatusResponse_t *)item);
}



/**@}*/