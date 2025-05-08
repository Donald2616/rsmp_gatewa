#ifndef RSMP_FIELDS_ACKMESSAGE_H
#define RSMP_FIELDS_ACKMESSAGE_H

#include <stddef.h>
#include "rsmp/coder/vars.h"

typedef char ackMessage_t[256]; // Limite à 256 caractères

int __rsmp_field_ackMessage_encode(void const *field, rsmp_var_t **var);
int __rsmp_field_ackMessage_decode(rsmp_var_t const *var, void *field);

#endif // RSMP_FIELDS_ACKMESSAGE_H