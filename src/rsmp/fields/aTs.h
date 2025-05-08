#ifndef RSMP_FIELDS_ATS_H
#define RSMP_FIELDS_ATS_H

#include "rsmp/vars/timestamp.h"

#ifdef RSMP_FIELDS_ATS_C
#define ATS_EXTERN
#define ATS_PRIVATE
#endif

#ifndef ATS_EXTERN
#define ATS_EXTERN extern
#endif

#ifndef ATS_PRIVATE
#define ATS_PRIVATE const
#endif

typedef timestamp_t aTs_t;

#define rsmp_aTs_to_string(ts, str, len)   rsmp_ts_to_string(ts, str, len)

#define rsmp_aTs_from_string(str, ts)      rsmp_ts_from_string(str, ts)
#define rsmp_aTs_set(ts)                   rsmp_ts_set(ts) 

#define __rsmp_field_aTs_decode __rsmp_field_ts_decode
#define __rsmp_field_aTs_encode __rsmp_field_ts_encode
#endif