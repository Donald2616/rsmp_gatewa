#ifndef RSMP_VARS_TIMESTAMP_H
#define RSMP_VARS_TIMESTAMP_H

#include <stdbool.h>
#include "horodate/horodate.h"
#include "rsmp/coder/vars.h"

#ifdef RSMP_VARS_TIMESTAMP_C
#define TIMESTAMP_EXTERN
#define TIMESTAMP_PRIVATE
#endif

#ifndef TIMESTAMP_EXTERN
#define TIMESTAMP_EXTERN extern
#endif

#ifndef TIMESTAMP_PRIVATE
#define TIMESTAMP_PRIVATE const
#endif

typedef HoroDate timestamp_t;

char *rsmp_ts_to_string(timestamp_t *ts, char *str, size_t len);


bool rsmp_ts_from_string(char const *str, timestamp_t *ts);
#define rsmp_ts_set(ts)                    setHorodate((ts))

/*******
 * // #define rsmp_ts_method(a,suffix) rsmp_##a##suffix
// #define stringzing(a) ##a


// #define rsmp_declare_timestamp(field) \
// typedef timestamp_t stringzing(field)_t;\
// #define rsmp_ts_method(field, _to_string)(ts, str, len)  rsmp_ts_to_string(ts, str, len) \
// #define rsmp_ts_method(field, _from_string)(str, ts) rsmp_ts_from_string(str, ts)\
// #define rsmp_ts_method(field, _set)(ts)                    rsmp_ts_set(ts)
*/

int __rsmp_field_ts_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_ts_decode(rsmp_var_t const *var, void *field);


#endif