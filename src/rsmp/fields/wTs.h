#ifndef RSMP_FIELDS_WTS_H
#define RSMP_FIELDS_WTS_H

#include "rsmp/vars/timestamp.h"

#ifdef RSMP_FIELDS_WTS_C
#define WTS_EXTERN
#define WTS_PRIVATE
#endif

#ifndef WTS_EXTERN
#define WTS_EXTERN extern
#endif

#ifndef WTS_PRIVATE
#define WTS_PRIVATE const
#endif

typedef timestamp_t wTs_t;

#define rsmp_wTs_to_string(ts, str, len)   rsmp_ts_to_string(ts, str, len)

#define rsmp_wTs_from_string(str, ts)      rsmp_ts_from_string(str, ts)
#define rsmp_wTs_set(ts)                   rsmp_ts_set(ts) 

#define __rsmp_field_wTs_decode __rsmp_field_ts_decode
#define __rsmp_field_wTs_encode __rsmp_field_ts_encode
#endif