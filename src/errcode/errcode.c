/**
* @file errcode.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-07
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"


#define RSMP_ERRCODE_ERRCODE_C
#include "errcode.h"

/**
 * @ingroup errcode
 * @{
 */

#define rsmp_errcode_lib_max(_t_) (sizeof(_t_)/sizeof(*_t_))

#define rsmp_errcode_no_family(_e_) ((_e_) & ~_rsmp_errcode_family_mask_)
#define rsmp_errcode_entry(_f_, _e_, _m_)\
 [rsmp_errcode_no_family(_rsmp_errcod_ ## _f_ ## _ ## _e_ ## _)] = #_f_ "/" _m_

static char const *_errcode_gen_libs[rsmp_errcode_no_family(_rsmp_errcod_gen_end_)] = 
{
    rsmp_errcode_entry(gen, no_error, "no error"),
    rsmp_errcode_entry(gen, decode,   "decode error"),
    rsmp_errcode_entry(gen, encode,   "encode error"),
    rsmp_errcode_entry(gen, config,   "config"),
    rsmp_errcode_entry(gen, generic,  "error"),
} ;



static char const *_errcode_json_libs[rsmp_errcode_no_family(_rsmp_errcod_json_end_)] = 
{
    rsmp_errcode_entry(json, no_error, "no error"),
    rsmp_errcode_entry(json, generic,  "error"),
    rsmp_errcode_entry(json, unknown_msg, "message rsmp inconnu"),
    rsmp_errcode_entry(json, notfound, "object json not found"),
    rsmp_errcode_entry(json, decode,   "decode error"),
    rsmp_errcode_entry(json, parse,    "parse failure"),
    rsmp_errcode_entry(json, no_header,"no header"),
    rsmp_errcode_entry(json, encode,   "encode error"),
} ;



static char const *_errcode_sxl_libs[rsmp_errcode_no_family(_rsmp_errcod_sxl_end_)] = 
{
    rsmp_errcode_entry(sxl, no_error, "no error"),
    rsmp_errcode_entry(sxl, generic,  "error"),
    rsmp_errcode_entry(sxl, unknown_cmd, "unknown command"),
    rsmp_errcode_entry(sxl, not_available_cmd, "unavailable command"),
    rsmp_errcode_entry(sxl, no_arg_cmd, "command without arg"),
} ;




static char const *_errcode_cyber_libs[rsmp_errcode_no_family(_rsmp_errcod_cyber_end_)] = 
{
    rsmp_errcode_entry(cyber, no_error, "no error"),
    rsmp_errcode_entry(cyber, generic,  "error"),
    rsmp_errcode_entry(cyber, flux, "error on communication flux"),
} ;


static char const *_errcode_wdog_libs[rsmp_errcode_no_family(_rsmp_errcod_wdog_end_)] = 
{
    rsmp_errcode_entry(wdog, no_error, "no error"),
    rsmp_errcode_entry(wdog, no_response,  "watchdog sent without response"),
    rsmp_errcode_entry(wdog, no_receive, "no watchdog received for a while"),
    rsmp_errcode_entry(wdog, failure, "watchdog failure, close flux requested"),
} ;

static char const *_errcode_auth_libs[rsmp_errcode_no_family(_rsmp_errcod_auth_end_)] = 
{
    rsmp_errcode_entry(auth, no_error,  "no error"),
    rsmp_errcode_entry(auth, unknown,   "login unknown"),
    rsmp_errcode_entry(auth, failure,   "authentification failure"),
    rsmp_errcode_entry(auth, no_secret, "no secret found"),
    rsmp_errcode_entry(auth, outdated,  "timestamp failure"),
} ;

static char const *_errcode_cipher_libs[rsmp_errcode_no_family(_rsmp_errcod_cipher_end_)] = 
{
    rsmp_errcode_entry(cipher, no_error, "no error"),
    rsmp_errcode_entry(cipher, keygen,  "key generation failure"),
    rsmp_errcode_entry(cipher, decrypt,  "decrypt failure"),
    rsmp_errcode_entry(cipher, encrypt,  "encrypt failure"),
} ;


#define rsmp_errcode_libs(_t_) \
[_rsmp_errcod_family_ ## _t_ ## _] = {\
    .max = _rsmp_errcod_ ## _t_ ## _end_,\
    .libs = _errcode_ ## _t_ ## _libs,\
}   

typedef struct rsmp_libs_rec
{
    size_t max;
    char const **libs;
} rsmp_libs_rec_t ;


static rsmp_libs_rec_t _errcode_libs[_rsmp_errcod_family_end_] =
{
    rsmp_errcode_libs(gen),
    rsmp_errcode_libs(json),
    rsmp_errcode_libs(sxl),
    rsmp_errcode_libs(cyber),
    rsmp_errcode_libs(wdog),
    rsmp_errcode_libs(auth),
    rsmp_errcode_libs(cipher),
} ;
const size_t _errcode_libs_max = (sizeof(_errcode_libs)/sizeof(*_errcode_libs));

char const *rsmp_errcode2str(rsmp_errcode_t errcode)
{
    if (!errcode) return "no error";
    size_t f = rsmp_errcode_family(errcode);
    if (f < _errcode_libs_max)
    {
        size_t e = errcode & ~_rsmp_errcode_family_mask_;
        if (e < _errcode_libs[f].max)
        {
            return _errcode_libs[f].libs[e];
        }
    }
    return "unknown error code";
}

/**@}*/