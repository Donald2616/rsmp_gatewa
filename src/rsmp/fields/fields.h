/**
* @file fields.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_FIELDS_H
#define RSMP_FIELDS_FIELDS_H

#include <stddef.h>

#include "rsmp/env/rsmp-context.h"
#include "rsmp/coder/vars.h"

/**
 * @defgroup fields rsmp fields
 * 
 * @brief 
 * 
 * @ingroup rsmp
 * @{
 */
#ifdef RSMP_FIELDS_FIELDS_C
#define FIELDS_EXTERN
#define FIELDS_PRIVATE
#endif

#ifndef FIELDS_EXTERN
#define FIELDS_EXTERN extern
#endif

#ifndef FIELDS_PRIVATE
#define FIELDS_PRIVATE const
#endif

typedef enum rsmp_fields
{
    _rsmp_field_aCId_,
    _rsmp_field_ack_,
    _rsmp_field_age_,
    _rsmp_field_arg_,
    _rsmp_field_aS_,
    _rsmp_field_aSp_,
    _rsmp_field_aSTS_,
    _rsmp_field_aTs_,
    _rsmp_field_cat_,
    _rsmp_field_cCI_,
    _rsmp_field_cId_,
    _rsmp_field_cO_,
    _rsmp_field_cTS_,
    _rsmp_field_fP_,
    _rsmp_field_fS_,
    _rsmp_field_mId_,
    _rsmp_field_mType_,
    _rsmp_field_n_,
    _rsmp_field_ntsOId_,
    _rsmp_field_oMId_,
    _rsmp_field_pri_,
    _rsmp_field_q_,
    _rsmp_field_rea_,
    _rsmp_field_RSMP_,
    _rsmp_field_rvs_,
    _rsmp_field_s_,
    _rsmp_field_sCI_,
    _rsmp_field_se_,
    _rsmp_field_sId_,
    _rsmp_field_siteId_,
    _rsmp_field_sOc_,
    _rsmp_field_sS_,
    _rsmp_field_sTs_,
    _rsmp_field_SXL_,
    _rsmp_field_eSXLG_,
    _rsmp_field_sg_,
    _rsmp_field_type_,
    _rsmp_field_uRt_,
    _rsmp_field_v_,
    _rsmp_field_vers_,
    _rsmp_field_wTs_,
    _rsmp_field_xACId_,
    _rsmp_field_xNACId_,
    _rsmp_field_xNId_,
    _rsmp_field_uId_,
    _rsmp_field_data_,
    _rsmp_field_login_,
    _rsmp_field_cipher_,
    _rsmp_field_ackMessage_,

    _rsmp_fields_no_,
} rsmp_fields_t;

char const *_rsmp_field_lib_get(rsmp_fields_t field);
rsmp_fields_t _rsmp_field_from_lib(char const *lib);

typedef enum
{
    _rsmp_field_desc_end_mark_,
    _rsmp_field_desc_field_mark_,
    _rsmp_field_desc_array_mark_,
    _rsmp_field_desc_table_mark_,
    _rsmp_field_desc_object_mark_,
} rsmp_field_desc_type_t;





/**
 * @brief prototype d'une fonction de convertion en string
 * 
 * Converti un champ en string
 * 
 * Est nécessaire uniquement aux champs qui ne sont pas des strings de base...
 * 
 * \remark ne s'applique pas aux tableaux, unquement aux champs unitaires
 * 
 * @param[in]  field  pointeur transparent sur le champ à convertir
 * @param[out] str    pointer vers la destination, si NULL allocation dynamique
 * @param[out] len    taille max de target si non NULL
 * 
 * @return ==0 si OK, !=0 si echec
 */
typedef int (*rsmp_field_encode_t)(void const *field, rsmp_var_t **var);

/**
 * @brief prototype d'une fonction de converstion en champ depuis une chaîne
 * 
 * Est nécessaire uniquement aux champs qui ne sont pas des strings de base...
 * 
 * \remark ne s'applique pas aux tableaux, unquement aux champs unitaires
 * 
 * @param[in]  field  pointeur transparent sur le champ à convertir
 * @param[out] target pointer vers la destination, si NULL allocation dynamique
 * @param[in]  len    taille max de target si non NULL
 * 
 * @return ==0 si OK, !=O si Pb
 */
typedef int (*rsmp_field_decode_t)(rsmp_var_t const *var, void *field);


typedef struct rsp_field_convert
{
    rsmp_field_decode_t  decode;
    rsmp_field_encode_t  encode;
} rsp_field_convert_t;


typedef struct rsmp_field_desc
{
  rsmp_field_desc_type_t   type;
  rsmp_fields_t            field;
  char const *             lib;
  size_t const             offset;
  union {
    struct rsmp_field_desc * array;
    rsp_field_convert_t convert;
  } ext;
} rsmp_field_desc_t;

#define rsmp_field_commun(_msg_, _field_) \
    .field  = _rsmp_field_ ## _field_ ## _,\
    .lib    = #_field_,\
    .offset = offsetof(_msg_, _field_)

#define rsmp_field_desc(msg, field) {\
    rsmp_field_commun(msg, field),\
    .type   = _rsmp_field_desc_field_mark_,\
    .ext.convert.encode = __rsmp_field_ ## field ## _encode,\
    .ext.convert.decode = __rsmp_field_ ## field ## _decode,\
}

#define rsmp_field_desc_table(msg, _field_) {\
    rsmp_field_commun(msg, _field_),\
    .type   = _rsmp_field_desc_table_mark_,\
    .ext.convert.encode = __rsmp_field_ ## _field_ ## _encode,\
    .ext.convert.decode = __rsmp_field_ ## _field_ ## _decode,\
}

#define rsmp_field_desc_array(msg, field, tab) {\
    rsmp_field_commun(msg, field),\
    .type   = _rsmp_field_desc_array_mark_,\
    .ext.array  = tab,\
}

#define rsmp_field_desc_object(msg, field, tab) {\
    rsmp_field_commun(msg, field),\
    .type   = _rsmp_field_desc_object_mark_,\
    .ext.array  = tab,\
}

#define rsmp_field_desc_end() {\
    .type   = _rsmp_field_desc_end_mark_,\
}

static inline rsmp_field_desc_t const *rsmp_message_desc_find_field_by_type(rsmp_field_desc_t const *fields, rsmp_fields_t target)
{
    if (fields)
    {
        rsmp_field_desc_t const *item = fields;
        while(item->type != _rsmp_field_desc_end_mark_)
        {
            if (item->field == target) return item;
            item++;
        }
    }
    return NULL;
}
int __rsmp_field_ack_encode(void const *field, rsmp_var_t **var);
int __rsmp_field_ack_decode(rsmp_var_t const *var, void *field);

/**@}*/
#endif //RSMP_FIELDS_FIELDS_H
