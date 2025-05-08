/**
* @file sxl.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-12
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <search.h>

#include "rsmp/rsmp.h"
#include "rsmp/fields/age.h"



#define RSMP_SXL_SXL_C
#include "sxl.h"
#ifdef LOG_SECTION
#undef LOG_SECTION
#endif
#define LOG_SECTION "SXL"

/**
 * @ingroup sxl
 * @{
 */


/**
 * @brief enregistrement d'un callback
 * 
 */
typedef struct rsmp_sxl_rec
{
    cCI_t               cCI;      /*!< \brief identifiant de la commande */
    rsmp_sxl_callback_t callback; /*!< \brief callback associé */
} rsmp_sxl_rec_t ;

static void *_rsmp_sxl_cmd_root = NULL;

/**
 * @brief Affectation d'un enregistrement
 * 
 * @param rec       pointeur sur l'enregistrement
 * @param cCI       code de la commande
 * @param callback  description du callback associé
 */
static inline void rsmp_sxl_cmd_set(rsmp_sxl_rec_t *rec, const cCI_t cCI, rsmp_sxl_callback_t const *callback)
{
    if (rec)
    {
        rsmp_cCI_set(&rec->cCI, cCI);
        rec->callback = *callback;
    }
}

/**
 * @brief Création d'un enregistrement
 * 
 * @param cCI              code de la commande
 * @param callback         description du callback associé
 * @return rsmp_sxl_rec_t* 
 */
static rsmp_sxl_rec_t *rsmp_sxl_cmd_new(const cCI_t cCI, rsmp_sxl_callback_t const *callback)
{
    rsmp_sxl_rec_t *rec = calloc(1, sizeof(*rec));
    if (rec) rsmp_sxl_cmd_set(rec, cCI, callback);
    return rec;
}

/**
 * @brief Libération d'un enregsitrement
 * 
 * \remark il n'y a pas de libération du context d'exécution du callbak...
 * @param item 
 */
static void rsmp_sxl_cmd_free(void *item)
{
    rsmp_sxl_rec_t *rec = item;
    rsmp_cCI_free(&rec->cCI);
    free(rec);
}

/**
 * @brief callback exécuté en sortie de programme
 * 
 * - libération de l'arbre en sortie de programme
 * 
 */
void rsmp_sxl_atexit(void)
{
    tdestroy(_rsmp_sxl_cmd_root, rsmp_sxl_cmd_free);
    
}

/**
 * @brief Comparaison d'enregistrement
 * 
 * Reprend les principes génériques d'une fonction de comparaison
 * 
 * @param a     pointeur dégguisé en void sur un enregistrement
 * @param b     pointeur dégguisé en void sur un enregistrement
 * @return int  ==0 si a==b, >0 si a > b, \<0 si a \< b
 */
static int rsmp_sxl_rec_cmp(const void* a, const void* b) {
    return strcmp(((rsmp_sxl_rec_t*)a)->cCI, ((rsmp_sxl_rec_t*)b)->cCI);
}

/**
 * @brief recherche un enregistrement par son code 
 * 
 * @param cCI      code recherché
 * @return rsmp_sxl_rec_t* !=NULL si trouvé, NULL sinon
 */
static inline rsmp_sxl_rec_t *_rsmp_sxl_find_by_cCI(const cCI_t cCI)
{
    rsmp_sxl_rec_t *rec = NULL;
    rsmp_sxl_rec_t key={.cCI=cCI, .callback.fct=NULL};
    rec = (rsmp_sxl_rec_t *)tfind(&key, &_rsmp_sxl_cmd_root, rsmp_sxl_rec_cmp);
    return rec;
}

/**
 * @brief recherche un enregistrement par son code 
 * 
 * Version avec log en cas d'erreur
 * 
 * @param cCI      code recherché
 * @return rsmp_sxl_rec_t* !=NULL si trouvé, NULL sinon
 */
static rsmp_sxl_rec_t *rsmp_sxl_find_by_cCI(const cCI_t cCI)
{
    rsmp_sxl_rec_t *rec = _rsmp_sxl_find_by_cCI(cCI);

    if (!rec) _rsmp_log_err(_rsmp_errcod_sxl_unknown_cmd_, "%s", cCI);
    return rec;
}

/*

88888888888  ,ad8888ba,  888888888888     88888888ba   88        88  88888888ba   88           88    ,ad8888ba,    88        88  88888888888  ad88888ba
88          d8"'    `"8b      88          88      "8b  88        88  88      "8b  88           88   d8"'    `"8b   88        88  88          d8"     "8b
88         d8'                88          88      ,8P  88        88  88      ,8P  88           88  d8'        `8b  88        88  88          Y8,
88aaaaa    88                 88          88aaaaaa8P'  88        88  88aaaaaa8P'  88           88  88          88  88        88  88aaaaa     `Y8aaaaa,
88"""""    88                 88          88""""""'    88        88  88""""""8b,  88           88  88          88  88        88  88"""""       `"""""8b,
88         Y8,                88          88           88        88  88      `8b  88           88  Y8,    "88,,8P  88        88  88                  `8b
88          Y8a.    .a8P      88          88           Y8a.    .a8P  88      a8P  88           88   Y8a.    Y88P   Y8a.    .a8P  88          Y8a     a8P
88           `"Y8888Y"'       88          88            `"Y8888Y"'   88888888P"   88888888888  88    `"Y8888Y"Y8a   `"Y8888Y"'   88888888888  "Y88888P"


*/


void rsmp_sxl_initialize(void)
{
    atexit(rsmp_sxl_atexit);
     // Appel à la fonction centralisée pour enregistrer tous les handlers DISP/
     register_disp_handlers();
     // Message de confirmation de l'enregistrement
     printf("[RSMP] Handlers DISP/ enregistrés avec succès.\n");
}


int rsmp_sxl_cmd_add(const cCI_t cCI, rsmp_sxl_callback_t const * callback, rsmp_sxl_callback_t *old)
{
    int ret = _rsmp_no_error_;
    
    rsmp_sxl_rec_t *item = _rsmp_sxl_find_by_cCI(cCI);
    if (item)
    {
        if (old != NULL) *old = item->callback;
        item->callback = *callback;
    }
    else
    {
       if ((item =  rsmp_sxl_cmd_new(cCI, callback)) != NULL)
       {
            if (tsearch(item, &_rsmp_sxl_cmd_root, rsmp_sxl_rec_cmp)==NULL)
            {
                ret = _rsmp_errcod_sxl_not_available_cmd_;
                _rsmp_log_errno(ret, "cmd: %s", cCI);
            }
            else
            { 
                if (old != NULL) {old->fct = NULL; old->context=NULL;}
            }
       }
    }
    return ret;
}


int rsmp_sxl_cmd_exe(void *context, args *_arg, rvs *_rvs)
{
    int ret = _rsmp_no_error_;
    rsmp_sxl_rec_t *rec = _rsmp_sxl_find_by_cCI(_arg->cCI);
    if (rec)
    {
        if (rec->callback.fct) 
        {
            ret = rec->callback.fct(rec->callback.context, context, _arg, _rvs);
        }
        else 
        {
            _rvs->age = _rsmp_age_unknown_;
            ret=_rsmp_errcod_sxl_not_available_cmd_;
            _rsmp_log_err(ret, "cmd: %s", rec->cCI);
        }
    }
    return ret;
}

int rsmp_sxl_cmd_is_active(const cCI_t cCI)
{
    int ret = _rsmp_no_error_;
    assert(cCI != NULL);
    rsmp_sxl_rec_t *rec = _rsmp_sxl_find_by_cCI(cCI);
    if (rec)
    {
        if (!rec->callback.fct) ret = _rsmp_errcod_sxl_not_available_cmd_;
    }
    else
    {
        ret  = _rsmp_errcod_sxl_unknown_cmd_;
    }
    return ret;
}

/**@}*/