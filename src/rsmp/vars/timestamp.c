#include "config.h"

#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#define RSMP_VARS_TIMESTAMP_C

#include "timestamp.h"

#define TIMESTAMP_FORMAT_DEF "%Y-%m-%dT%H:%M:%S.#mlZ"


HoroDate *str2ZuluHorodate(char const *str, char const *format, HoroDate *horodate)
{
    assert(horodate != NULL);
    char *sbis = NULL;
    char *sstr = (char *)str;
    char *dot = NULL;
    unsigned long ml = 0;
    
    if (!format) format="%Y-%m-%dT%H:%M:%SZ";
    if (strstr(format,".#ml") != NULL)
    {
        sstr = sbis = strdup(str);
        dot = strrchr(sbis, '.');
        if (dot)
        {
            errno=0;
            char*next;
            ml = strtoul(dot+1, &next, 10);
            ml *= 1000000; //-- Passage en nanoseconde
            if (errno) 
            {
                //Failure...
                ml = 0;
            }
            else 
            {
                strcpy(dot, next);
            }
        }
    }
    if (sstr)
    {
        struct tm tm_info;
        strptime(str, format, &tm_info);
        tm_info.tm_isdst = -1;
        setSecond(horodate, mktime(&tm_info));
        setNanoSecond(horodate, ml);
        localtime_r(&horodate->tv_sec, &tm_info);
    	horodate->tv_sec += tm_info.tm_gmtoff;
    }

    if (sbis) free(sbis);

    return horodate;
}




char *rsmp_ts_to_string(timestamp_t *ts, char *str, size_t len)  
{
    assert(ts != NULL);
    if (!str)
    {   
        len = 50;
        str=calloc(len, sizeof(*str));
        if (!str)
        {
            _log_err("calloc failed");
            return NULL;
        }
    }
    return strGmtHorodate(TIMESTAMP_FORMAT_DEF, 0, str, len, ts);
}  

int __rsmp_field_ts_encode(void const *field,rsmp_var_t **var)
{
    assert(field != NULL);
    assert(var != NULL);
    char out[50] = "";
    rsmp_ts_to_string((timestamp_t *)field, out, sizeof(out));
    return rsmp_var_auto_string(var, out) != NULL ? 0 : 1;
}


bool rsmp_ts_from_string(char const *str, timestamp_t *ts)
{
    assert(str != NULL);
    assert(ts != NULL);
    str2ZuluHorodate(str, TIMESTAMP_FORMAT_DEF, ts);
    return ts->tv_sec >0;
}

int __rsmp_field_ts_decode(rsmp_var_t const *var, void *field)
{
    assert(field != NULL);
    assert(var != NULL);
    int ret = 1;
    if (rsmp_var_is_string(var))
    {
        ret = !rsmp_ts_from_string(var->data.s.t, (timestamp_t *)field);
    }
    return ret;
}
