#ifndef RSMP_FIELDS_STS_H
#define RSMP_FIELDS_STS_H

#include "rsmp/vars/timestamp.h"

#ifdef RSMP_FIELDS_STS_C
#define STS_EXTERN
#define STS_PRIVATE
#endif

#ifndef STS_EXTERN
#define STS_EXTERN extern
#endif

#ifndef STS_PRIVATE
#define STS_PRIVATE const
#endif

typedef timestamp_t sTs_t;

#define rsmp_sTs_to_string(ts, str, len)   rsmp_ts_to_string(ts, str, len)

#define rsmp_sTs_from_string(str, ts)      rsmp_ts_from_string(str, ts)
#define rsmp_sTs_set(ts)                   rsmp_ts_set(ts) 

#define __rsmp_field_sTs_decode __rsmp_field_ts_decode
#define __rsmp_field_sTs_encode __rsmp_field_ts_encode
#endif