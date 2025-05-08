#ifndef RSMP_FIELDS_ASTS_H
#define RSMP_FIELDS_ASTS_H

#include "rsmp/vars/timestamp.h"

#ifdef RSMP_FIELDS_ASTS_C
#define ASTS_EXTERN
#define ASTS_PRIVATE
#endif

#ifndef ASTS_EXTERN
#define ASTS_EXTERN extern
#endif

#ifndef ASTS_PRIVATE
#define ASTS_PRIVATE const
#endif

typedef timestamp_t aSTS_t;

#define rsmp_aSTS_to_string(ts, str, len)   rsmp_ts_to_string(ts, str, len)

#define rsmp_aSTS_from_string(str, ts)      rsmp_ts_from_string(str, ts)
#define rsmp_aSTS_set(ts)                   rsmp_ts_set(ts) 

#define __rsmp_field_aSTS_decode __rsmp_field_ts_decode
#define __rsmp_field_aSTS_encode __rsmp_field_ts_encode
#endif