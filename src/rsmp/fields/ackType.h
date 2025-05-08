#ifndef RSMP_FIELDS_ACKTYPE_H
#define RSMP_FIELDS_ACKTYPE_H

#include <stdbool.h>
#include "rsmp/coder/vars.h"

/**
 * @brief Type pour représenter un acquittement booléen.
 */
typedef bool ack_t;

/**
 * @brief Encode un champ de type ack_t en rsmp_var_t.
 * 
 * @param field Pointeur vers le champ à encoder.
 * @param var Pointeur vers la variable RSMP où encoder.
 * @return int 0 si l'encodage réussit, 1 sinon.
 */
int __rsmp_field_ackType_encode(void const *field, rsmp_var_t **var);

/**
 * @brief Décode un champ de type ack_t depuis rsmp_var_t.
 * 
 * @param var Pointeur vers la variable RSMP à décoder.
 * @param field Pointeur vers le champ où stocker la valeur décodée.
 * @return int 0 si le décodage réussit, 1 sinon.
 */
int __rsmp_field_ackType_decode(rsmp_var_t const *var, void *field);

#endif // RSMP_FIELDS_ACKTYPE_H



