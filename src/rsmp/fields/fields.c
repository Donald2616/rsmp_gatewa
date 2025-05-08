#include "config.h"

#include <stdio.h>

#define RSMP_FIELDS_FIELDS_C
#include "fields.h"

typedef struct rsmp_fields_tab
{
    const char *lib;
} rsmp_fields_tab_t;

static rsmp_fields_tab_t _rsmp_fields_tab[_rsmp_fields_no_] =
{
    [_rsmp_field_aCId_]  ={.lib="aCId"},
    [_rsmp_field_ack_]   ={.lib="ack"},
    [_rsmp_field_age_]   ={.lib="age"},
    [_rsmp_field_arg_]   ={.lib="arg"},
    [_rsmp_field_aS_]    ={.lib="aS"},
    [_rsmp_field_aSp_]   ={.lib="aSp"},
    [_rsmp_field_aSTS_]  ={.lib="aSTS"},
    [_rsmp_field_aTs_]   ={.lib="aTs"},
    [_rsmp_field_cat_]   ={.lib="cat"},
    [_rsmp_field_cCI_]   ={.lib="cCI"},
    [_rsmp_field_cId_]   ={.lib="cId"},
    [_rsmp_field_cO_]    ={.lib="cO"},
    [_rsmp_field_cTS_]   ={.lib="cTS"},
    [_rsmp_field_fP_]    ={.lib="fP"},
    [_rsmp_field_fS_]    ={.lib="fS"},
    [_rsmp_field_mId_]   ={.lib="mId"},
    [_rsmp_field_mType_] ={.lib="mType"},
    [_rsmp_field_n_]     ={.lib="n"},
    [_rsmp_field_ntsOId_]={.lib="ntsOId"},
    [_rsmp_field_oMId_]  ={.lib="oMId"},
    [_rsmp_field_pri_]   ={.lib="pri"},
    [_rsmp_field_q_]     ={.lib="q"},
    [_rsmp_field_rea_]   ={.lib="rea"},
    [_rsmp_field_RSMP_]  ={.lib="RSMP"},
    [_rsmp_field_rvs_]   ={.lib="rvs"},
    [_rsmp_field_s_]     ={.lib="s"},
    [_rsmp_field_sCI_]   ={.lib="sCI"},
    [_rsmp_field_se_]    ={.lib="se"},
    [_rsmp_field_sId_]   ={.lib="sId"},
    [_rsmp_field_siteId_]={.lib="siteId"},
    [_rsmp_field_sOc_]   ={.lib="sOc"},
    [_rsmp_field_sS_]    ={.lib="sS"},
    [_rsmp_field_sTs_]   ={.lib="sTs"},
    [_rsmp_field_SXL_]   ={.lib="SXL"},
    [_rsmp_field_type_]  ={.lib="type"},
    [_rsmp_field_uRt_]   ={.lib="uRt"},
    [_rsmp_field_v_]     ={.lib="v"},
    [_rsmp_field_vers_]  ={.lib="vers"},
    [_rsmp_field_wTs_]   ={.lib="wTs"},
    [_rsmp_field_xACId_] ={.lib="xACId"},
    [_rsmp_field_xNACId_]={.lib="xNACId"},
    [_rsmp_field_xNId_]  ={.lib="xNId"},
    [_rsmp_field_uId_]   ={.lib="uId"},
    [_rsmp_field_data_]  ={.lib="data"},
    [_rsmp_field_login_] ={.lib="login"},
    [_rsmp_field_cipher_]={.lib="cipher"},
};

char const *_rsmp_field_lib_get(rsmp_fields_t field)
{
    return (field < _rsmp_fields_no_)?_rsmp_fields_tab[field].lib:(char const *)NULL;
}

rsmp_fields_t _rsmp_field_from_lib(char const *lib)
{
    rsmp_fields_t i =_rsmp_field_aCId_;
    while(i<_rsmp_fields_no_)
    {
        if (strcmp(lib, _rsmp_fields_tab[i].lib)==0) return i;
        i++;
    }
    return i;
}

int rsmp_encode_bool(bool value, uint8_t *buffer, size_t size)
{
    if (size < 1) return -1; // Vérifie que le buffer est assez grand
    buffer[0] = value ? 1 : 0; // Encode le booléen comme 1 ou 0
    return 1; // Retourne la taille utilisée
}

int rsmp_decode_bool(bool *value, const uint8_t *buffer, size_t size)
{
    if (size < 1) return -1; // Vérifie que le buffer contient au moins 1 octet
    *value = buffer[0] != 0; // Décode le booléen (0 = false, tout autre valeur = true)
    return 1; // Retourne la taille utilisée
}

int rsmp_encode_string(const char *field, uint8_t *buffer, size_t size)
{
    size_t len = strlen(field);
    if (len >= size) return -1; // Vérifie que le buffer est assez grand
    memcpy(buffer, field, len);
    return len; // Retourne la taille utilisée
}

int rsmp_decode_string(char *field, const uint8_t *buffer, size_t size)
{
    if (size >= 256) size = 255; // Limite la taille à 255 caractères
    memcpy(field, buffer, size);
    field[size] = '\0'; // Ajoute un terminateur NULL
    return size; // Retourne la taille utilisée
}

/*
int __rsmp_field_ack_encode(void const *field, rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_bool(var, *(bool *)field) != NULL ? 0 : 1;
}

int __rsmp_field_ack_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    if (rsmp_var_is_bool(var))
    {
        *(bool *)field = var->data.b;
        return 0;
    }
    return 1;
}
*/