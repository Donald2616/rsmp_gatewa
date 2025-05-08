/**
* @file errcode.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-07
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_ERRCODE_ERRCODE_H
#define RSMP_ERRCODE_ERRCODE_H
#include <stdint.h>
/**
 * @defgroup errcode 
 * 
 * @brief 
 * 
 * @ingroup rsmp
 * @{
 */

#ifdef RSMP_ERRCODE_ERRCODE_C
#define ERRCODE_EXTERN
#define ERRCODE_PRIVATE
#endif

#ifndef ERRCODE_EXTERN
#define ERRCODE_EXTERN extern
#endif

#ifndef ERRCODE_PRIVATE
#define ERRCODE_PRIVATE const
#endif


#define _rsmp_errcode_family_mask_ 0xFFFF0000


#define rsmp_errcode_family(_e_)  (((_e_) & _rsmp_errcode_family_mask_)>>16)

#define rsmp_errcode_value(_f_, _e_) (((_f_)<<16) | ((_e_) & ~_rsmp_errcode_family_mask_))


typedef enum rsmp_errcode_family{
    _rsmp_errcod_family_gen_ = 0,
    _rsmp_errcod_family_json_,
    _rsmp_errcod_family_sxl_,
    _rsmp_errcod_family_cyber_,
    _rsmp_errcod_family_wdog_,
    _rsmp_errcod_family_auth_,
    _rsmp_errcod_family_cipher_,
    _rsmp_errcod_family_transport_,
    
    _rsmp_errcod_family_end_
} rsmp_errcode_family_t;



typedef enum rsmp_errcode {
    //-- Famille générique
    _rsmp_errcod_gen_no_error_ = rsmp_errcode_value(_rsmp_errcod_family_gen_, 0),
    _rsmp_errcod_gen_generic_,
    _rsmp_errcod_gen_config_,
    _rsmp_errcod_gen_decode_,
    _rsmp_errcod_gen_encode_,
    _rsmp_errcod_gen_end_, //-- Marqueur de fin de fammille

    //-- Famille json
    _rsmp_errcod_json_no_error_ = rsmp_errcode_value(_rsmp_errcod_family_json_, 0),
    _rsmp_errcod_json_generic_,
    _rsmp_errcod_json_unknown_msg_,
    _rsmp_errcod_json_notfound_,
    _rsmp_errcod_json_decode_,
    _rsmp_errcod_json_parse_,
    _rsmp_errcod_json_no_header_,
    _rsmp_errcod_json_encode_,
    _rsmp_errcod_json_end_,

    //-- Famille sxl
    _rsmp_errcod_sxl_no_error_ = rsmp_errcode_value(_rsmp_errcod_family_sxl_, 0),
    _rsmp_errcod_sxl_generic_,
    _rsmp_errcod_sxl_unknown_cmd_,
    _rsmp_errcod_sxl_not_available_cmd_,
    _rsmp_errcod_sxl_no_arg_cmd_,
    _rsmp_errcod_sxl_end_,

    //-- Famille cyber
    _rsmp_errcod_cyber_no_error_ = rsmp_errcode_value(_rsmp_errcod_family_cyber_, 0),
    _rsmp_errcod_cyber_generic_,
    _rsmp_errcod_cyber_flux_,
    _rsmp_errcod_cyber_end_,

    //-- Famille watchdog
    _rsmp_errcod_wdog_no_error_ = rsmp_errcode_value(_rsmp_errcod_family_wdog_, 0),
    _rsmp_errcod_wdog_no_response_,
    _rsmp_errcod_wdog_no_receive_,
    _rsmp_errcod_wdog_failure_,
    _rsmp_errcod_wdog_end_,

    //-- Famille auth
    _rsmp_errcod_auth_no_error_ = rsmp_errcode_value(_rsmp_errcod_family_auth_, 0),
    _rsmp_errcod_auth_failing_input_,
    _rsmp_errcod_auth_unknown_,
    _rsmp_errcod_auth_failure_,
    _rsmp_errcod_auth_no_secret_,
    _rsmp_errcod_auth_outdated_,
    _rsmp_errcod_auth_end_,

    //-- Famille cipher
    _rsmp_errcod_cipher_no_error_ = rsmp_errcode_value(_rsmp_errcod_family_cipher_, 0),
    _rsmp_errcod_cipher_keygen_,
    _rsmp_errcod_cipher_decrypt_,
    _rsmp_errcod_cipher_encrypt_,
    _rsmp_errcod_cipher_end_,

    //-- Famille transport
    _rsmp_errcod_transport_no_error_ = rsmp_errcode_value(_rsmp_errcod_family_transport_, 0),
    _rsmp_errcod_transport_generic_,
    _rsmp_errcod_transport_connection_,
    _rsmp_errcod_transport_listener_,
    _rsmp_errcod_transport_end_,

} rsmp_errcode_t;

char const *rsmp_errcode2str(rsmp_errcode_t errcode);

#define _rsmp_no_error_ _rsmp_errcod_gen_no_error_
#define _rsmp_err_is_ok(a) ((a)==_rsmp_no_error_)


#define _rsmp_log_err(err, f,...)   _log_err("(%08x)%s - " f "", err, rsmp_errcode2str(err), ##__VA_ARGS__)
#define _rsmp_log_errno(err, f,...) _log_errno("(%08x)%s - " f "", err, rsmp_errcode2str(err), ##__VA_ARGS__)

/**@}*/
#endif //ERRCODE_ERRCODE_H