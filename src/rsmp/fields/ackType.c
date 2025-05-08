/**
 * @file ackType.c
 * @brief Implémentation des encodeurs et décodeurs pour le type ack_t.
 * @version 0.1
 * @date 2023-07-04
 * 
 */

 #include "config.h"
 #include <assert.h>
 #include "rsmp/coder/vars.h"
 #include "ackType.h"
 
 
 /**
  * @brief Encode un champ de type ack_t en rsmp_var_t.
  */
 int __rsmp_field_ackType_encode(void const *field, rsmp_var_t **var)
 {
     assert(field != NULL);
     assert(var != NULL);
     return rsmp_var_auto_bool(var, *(ack_t *)field) != NULL ? 0 : 1;
 }
 
 int __rsmp_field_ackType_decode(rsmp_var_t const *var, void *field)
 {
     assert(var != NULL);
     assert(field != NULL);
     if (rsmp_var_is_boolean(var))
     {
         *(ack_t *)field = var->data.b;
         return 0;
     }
     return 1;
 }