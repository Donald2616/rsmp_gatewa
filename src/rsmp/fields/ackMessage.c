#include "ackMessage.h"
#include <string.h>

int __rsmp_field_ackMessage_encode(void const *field, rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    return rsmp_var_auto_string(var, (char *)field) != NULL ? 0 : 1;
}

int __rsmp_field_ackMessage_decode(rsmp_var_t const *var, void *field)
{
    assert(var != NULL);
    assert(field != NULL);
    if (rsmp_var_is_string(var))
    {
        strncpy((char *)field, var->data.s.t, 256); // Copie avec une limite de 256 caractères
        return 0;
    }
    return 1;
}