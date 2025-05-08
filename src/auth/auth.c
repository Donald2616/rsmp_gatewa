#define AUTH_AUTH_C
#include "auth/auth.h"

#include "memory/checkmem.h"
#include "errcode/errcode.h"
#include "log/log.h"
#include "assert.h"
#include "auth-db.h"
#include "auth-db-to.h"
#include "auth-db-from.h"

static int auth_gen_new_pwd(auth_t *auth, size_t len)
{
    int ret = _rsmp_no_error_;
    assert(len >= AUTH_PWD_LEN_MIN && len <= AUTH_PWD_LEN_MAX);
    
    assert(auth);

    if(auth->new_pwd_pending)
    {
        _log_info("New password already generated");
        return ret;
    }
    _free(auth->new_pwd.buf);
    auth->new_pwd.len = len;
    auth->new_pwd.buf = calloc(len, sizeof(*auth->new_pwd.buf));
    if(!auth->new_pwd.buf)
    {
        _log_err("Failed to allocate memory for new password");
        auth->new_pwd.len = 0;
        auth->new_pwd_pending = 0;
        return _rsmp_errcod_cyber_generic_;
    }
    
    // generer le nombre de caractères aléatoires en excluant les codes ascii inférieurs à 32 et supérieurs à 126
    for (size_t i = 0; i < len; i++)
    {
        auth->new_pwd.buf[i] = rand() % (127 - 32) + 32;
        // \todo remplacer rand par RAND_bytes
    }
    _log_dump_info(LOG_MARK_DEBUG, auth->new_pwd.buf, auth->new_pwd.len, "New password generated");

    return ret;
}


// retourne new_pwd en base64
// l'appelant doit libérer la sortie
int auth_get_new_pwd_b64(auth_t *auth, char **out)
{
    
    int ret = _rsmp_no_error_;

    assert(auth);
    if(!auth->active){
        _log_err("auth=%p auth->active=%d", auth, auth->active);
        return _rsmp_errcod_cyber_generic_;
    } 
    assert(auth_to_is(auth)); // vérifier que la base est de type TO
    if(!auth->new_pwd_pending) {
        ret = auth_gen_new_pwd(auth, auth->pwd.len); // demande nouveau mot de passe de la même longueur
        if(!_rsmp_err_is_ok(ret)){
            _log_err("fail gen new pw");
            return ret;
        }
        auth->new_pwd_pending = true;
    }
    if(0 != base64_encode((unsigned char *)auth->new_pwd.buf, auth->new_pwd.len, out)) {
        _log_err("fail encoding b64");
        return _rsmp_errcod_cyber_generic_;
    }

    auth_db_to_file("auth.db"); // \todo faire une accroche dans cyber priv ou/et par paramètre de configuration

    return ret;
}

int auth_set_pwd_b64(auth_t *auth, const char *in)
{
    int ret = _rsmp_no_error_;

    assert(auth);
    if(!auth->active){
        _log_err("auth=%p auth->active=%d", auth, auth->active);
        return _rsmp_errcod_cyber_generic_;
    } 
    assert(auth_from_is(auth)); // vérifier que la base est de type FROM

    if(0 != base64_decode(in, &auth->pwd.buf, &auth->pwd.len))
    {
        _log_err("fail b64 decoding");
        return _rsmp_errcod_cyber_generic_;
    }
    setHorodate(&auth->timestamp);

    auth_db_to_file("auth.db"); // \todo faire une accroche dans cyber priv ou/et par paramètre de configuration

    return ret;
}

int auth_switch_to_new_pwd(auth_t *auth)
{
    int ret = _rsmp_no_error_;
    

    assert(auth);
    if(!auth->active){
        _log_err("auth=%p auth->active=%d", auth, auth->active);
        return _rsmp_errcod_cyber_generic_;
    } 
    assert(auth_to_is(auth));
    if(!auth->new_pwd_pending)
    {
        _log_err("no new pwd to activate");
        return _rsmp_errcod_cyber_generic_;
    }
    assert(auth->new_pwd.buf && auth->new_pwd.len);
    _free(auth->pwd.buf);
    auth->pwd.buf = calloc(auth->new_pwd.len, sizeof(*auth->new_pwd.buf));
    if(!auth->pwd.buf) {
        _log_err("fail calloc");
        return _rsmp_errcod_cyber_generic_;
    }
    memcpy(auth->pwd.buf, auth->new_pwd.buf, auth->new_pwd.len);
    auth->pwd.len = auth->new_pwd.len;
    setHorodate(&auth->timestamp);
    
    ret = auth_forget_new_pwd(auth);

    auth_db_to_file("auth.db"); // \todo faire une accroche dans cyber priv ou/et par paramètre de configuration

    return ret;
}

int auth_forget_new_pwd(auth_t *auth)
{
    int ret = _rsmp_no_error_;
    
    assert(auth);
    if(!auth->active){
        _log_err("auth=%p auth->active=%d", auth, auth->active);
        return _rsmp_errcod_cyber_generic_;
    } 
    assert(auth_to_is(auth));
    auth->new_pwd.len =  0;
    _free(auth->new_pwd.buf);
    auth->new_pwd_pending = 0;

    return ret;
}