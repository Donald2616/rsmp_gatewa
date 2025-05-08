#ifndef RSMP_FIELDS_CTS_H
#define RSMP_FIELDS_CTS_H

#include "rsmp/vars/timestamp.h"

#ifdef RSMP_FIELDS_CTS_C
#define CTS_EXTERN
#define CTS_PRIVATE
#endif

#ifndef CTS_EXTERN
#define CTS_EXTERN extern
#endif

#ifndef CTS_PRIVATE
#define CTS_PRIVATE const
#endif

typedef timestamp_t cTS_t;

#define rsmp_cTS_to_string(ts, str, len)   rsmp_ts_to_string(ts, str, len)

#define rsmp_cTS_from_string(str, ts)      rsmp_ts_from_string(str, ts)
#define rsmp_cTS_set(ts)                   rsmp_ts_set(ts) 

#define __rsmp_field_cTS_decode __rsmp_field_ts_decode
#define __rsmp_field_cTS_encode __rsmp_field_ts_encode
#endif