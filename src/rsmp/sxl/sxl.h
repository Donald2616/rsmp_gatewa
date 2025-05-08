/**
* @file sxl.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-12
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef SXL_SXL_H
#define SXL_SXL_H

#include "rsmp/fields/arg.h"
#include "rsmp/fields/rvs.h"
#include "rsmp/fields/cCI.h"
#include "rsmp/sxl/display/disp_display.h"

/**
 * @defgroup sxl 
 * 
 * @brief regoupe les traitements liés aux commandes SXL
 * 
 * @ingroup rsmp
 * @{
 */
/**
 * \brief sxl function callback prototype
 * 
 * \param[inout] context       context d'exécution du callback
 * \param[inout] rsmp_context  context d'exécution de la connexion RSMP
 * \param[in]    request       requête de la commande
 * \param[out]   response      tableau des réponses associées
 * 
 * \return _rsmp_no_error_ si OK, le code d'erreur associé sinon
*/
typedef int (*rsmp_sxl_cmd_cb_t)(void *context, void *rsmp_context, args const *request, rvs *response);

typedef struct rsmp_sxl_callback
{
    rsmp_sxl_cmd_cb_t fct;
    void *            context;
} rsmp_sxl_callback_t;

/**
 * @brief Initialisation du module
 * 
 * \remark doit être appelé en premier...
 */
void rsmp_sxl_initialize(void);

/**
 * @brief Ajoute un callback de traitement de commande
 * 
 * Si le pointeur out est positionné (!= NULL), alors l'ancien
 * callback est retourné.
 * 
 * @param[in] cCI       code de la commande
 * @param[in] callback  callback (fct + context)
 * @param[out] old      ancien callback (fct + context)
 * @return int le code d'erreur de l'exécution 
 */
int rsmp_sxl_cmd_add(const cCI_t cCI, rsmp_sxl_callback_t const * callback, rsmp_sxl_callback_t *old);

static inline int rsmp_sxl_cmd_jsut_add(const cCI_t cCI, rsmp_sxl_cmd_cb_t fct, void *context)
{
    rsmp_sxl_callback_t callback={.context=context,.fct=fct};
    return rsmp_sxl_cmd_add(cCI, &callback, NULL);
}

/**
 * @brief exécute un callback
 * 
 * Cette focntion est déclenchée sur réception d'un message CommandRequest.
 * 
 * Elle parcourt deux fois les arguments d'entrée :
 * - une fois pour valider toutes les commandes SXL
 * - une fois si la première est un succès pour l'exécution de chaque commande
 * 
 * @param context context d'exécution
 * @param _arg    liste des arguments d'entrées
 * @param _rvs    liste des résultats de sortie
 * @return int    le code erreur 
 */
int rsmp_sxl_cmd_exe(void *context, args *_arg, rvs *_rvs);

/**
 * @brief vérifie qu'une commande est active
 * 
 * Une commande active est une commande SXL enregistrée et dont le
 * callback est non NULL.
 * 
 * @param cCI   identifiant de la commande recherchée
 * @retval _rsmp_no_error_ commande active
 * @retval _rsmp_errcod_sxl_unknown_cmd_ commande inconnu (non enregistrée)
 * @retval _rsmp_errcod_sxl_not_available_cmd_ commande enregistrée, mais non disponible (callback NULL)
 */
int rsmp_sxl_cmd_is_active(const cCI_t cCI);

/**@}*/
#endif //SXL_SXL_H
