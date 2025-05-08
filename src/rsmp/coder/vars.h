/**
* @file vars.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-05
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_CODER_VARS_H
#define RSMP_CODER_VARS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/**
 * @defgroup vars 
 * 
 * @brief 
 * 
 * @{
 */
typedef enum {
    _rsmp_var_bool_,
    _rsmp_var_string_,
    _rsmp_var_int_,
    _rsmp_var_uint_,
    _rsmp_var_double_,
    _rsmp_var_null_,
}rsmp_var_type_t;

typedef struct rsmp_string
{
    char * t;
    size_t l;
}rsmp_string_t;


typedef struct rsmp_var
{
    rsmp_var_type_t type;
    union {
        rsmp_string_t s;
        uint64_t      u;
        int64_t       i;
        double        d;
        bool          b;
    } data;
}rsmp_var_t;


#define rsmp_var_type_check(_v_, _t_)  ((_v_)->type == (_t_))
#define rsmp_var_is_boolean(_v_) rsmp_var_type_check(_v_, _rsmp_var_bool_)
#define rsmp_var_is_string(_v_)  rsmp_var_type_check(_v_, _rsmp_var_string_)
#define rsmp_var_is_int(_v_)     rsmp_var_type_check(_v_, _rsmp_var_int_)
#define rsmp_var_is_uint(_v_)    rsmp_var_type_check(_v_, _rsmp_var_uint_)
#define rsmp_var_is_double(_v_)  rsmp_var_type_check(_v_, _rsmp_var_double_)
#define rsmp_var_is_null(_v_)     rsmp_var_type_check(_v_, _rsmp_var_null_)

static inline void rsmp_var_data_free(rsmp_var_t *var)
{
    if (var)
    {
        if (rsmp_var_is_string(var))
        {
            if(var->data.s.t)   free(var->data.s.t);
        } 
        var->type=_rsmp_var_null_;
        memset(&var->data, 0, sizeof(var->data));
    }
}
static inline void rsmp_var_free(rsmp_var_t *var)
{
    if (var)
    {
        rsmp_var_data_free(var);
        free(var);
    }
}


static inline rsmp_var_t *rsmp_var_set_string(rsmp_var_t *var, const char *in)
{
    if (var)
    {
        if (rsmp_var_is_string(var) && var->data.s.t!=NULL) rsmp_var_data_free(var);
        var->type = _rsmp_var_string_;
        if (in)
        {
            var->data.s.t = strdup(in);
            if (var->data.s.t)
            {
                var->data.s.l = strlen(in);
            }
            else
            {
                var = NULL;
            }
        }
        else
        {
            var->data.s.t = NULL;
            var->data.s.l = 0;
        }
    }

    return var;
}

static inline rsmp_var_t *rsmp_var_set_string_len(rsmp_var_t *var, const char *in, size_t len)
{
    if (var)
    {
        if (rsmp_var_is_string(var) && var->data.s.t!=NULL) rsmp_var_data_free(var);
        var->type = _rsmp_var_string_;
        if (in)
        {
            var->data.s.t = calloc(len+1, sizeof(*var->data.s.t));
            if (var->data.s.t)
            {
                memcpy(var->data.s.t, in, len);
                var->data.s.l = len;
            }
            else
            {
                var = NULL;
            }
        }
        else
        {
            var->data.s.t = NULL;
            var->data.s.l = 0;
        }
    }

    return var;
}

static inline rsmp_var_t *rsmp_var_set_int(rsmp_var_t *var, int64_t in)
{
    if (var)
    {
        if (rsmp_var_is_string(var) && var->data.s.t!=NULL) rsmp_var_data_free(var);
        var->type = _rsmp_var_int_;
        var->data.i = in;
    }
    return var;
}

static inline rsmp_var_t *rsmp_var_set_uint(rsmp_var_t *var, uint64_t in)
{
    if (var)
    {
        if (rsmp_var_is_string(var) && var->data.s.t!=NULL) rsmp_var_data_free(var);
        var->type = _rsmp_var_uint_;
        var->data.u = in;
    }
    return var;
}

static inline rsmp_var_t *rsmp_var_set_double(rsmp_var_t *var, long double in)
{
    if (var)
    {
        if (rsmp_var_is_string(var) && var->data.s.t!=NULL) rsmp_var_data_free(var);
        var->type = _rsmp_var_double_;
        var->data.d = in;
    }
    return var;
}

static inline rsmp_var_t *rsmp_var_set_bool(rsmp_var_t *var, bool in)
{
    if (var)
    {
        if (rsmp_var_is_string(var) && var->data.s.t!=NULL) rsmp_var_data_free(var);
        var->type = _rsmp_var_bool_;
        var->data.b = in;
    }
    return var;
}

static inline rsmp_var_t *rsmp_var_set_null(rsmp_var_t *var)
{
    if (var) rsmp_var_data_free(var);
    return var;
}


static inline rsmp_var_t *rsmp_var_new(void)
{
    rsmp_var_t *item=calloc(1, sizeof(rsmp_var_t));
    if (item) item->type = _rsmp_var_null_;
    return item;
}

static inline rsmp_var_t *rsmp_var_from_string(const char *in)
{
    rsmp_var_t *var = rsmp_var_new();
    if (var)
    {
        if (!rsmp_var_set_string(var, in))
        {
            free(var);
            var = NULL;
        }
    }

    return var;
}

static inline rsmp_var_t *rsmp_var_from_string_len(const char *in, size_t len)
{
    rsmp_var_t *var = rsmp_var_new();
    if (var)
    {
        if (!rsmp_var_set_string_len(var, in, len))
        {
            free(var);
            var = NULL;
        }
    }
    return var;
}

static inline rsmp_var_t *rsmp_var_from_int(int64_t in)
{
    rsmp_var_t *var = rsmp_var_new();
    if (var)
    {
        rsmp_var_set_int(var, in);
    }
    return var;
}

static inline rsmp_var_t *rsmp_var_from_uint(uint64_t in)
{
    rsmp_var_t *var = rsmp_var_new();
    if (var)
    {
        rsmp_var_set_uint(var, in);
    }
    return var;
}

static inline rsmp_var_t *rsmp_var_from_double(long double in)
{
    rsmp_var_t *var = rsmp_var_new();
    if (var)
    {
        rsmp_var_set_double(var, in);
    }
    return var;
}

static inline rsmp_var_t *rsmp_var_from_bool(bool in)
{
    rsmp_var_t *var = rsmp_var_new();
    if (var)
    {
        rsmp_var_set_bool(var, in);
    }
    return var;
}

static inline rsmp_var_t *rsmp_var_from_null(void)
{
    return rsmp_var_new();
}





static inline rsmp_var_t *rsmp_var_auto_string(rsmp_var_t **var, const char *in)
{
    assert(var != NULL);
    if (!*var) return (*var = rsmp_var_from_string(in));
    return rsmp_var_set_string(*var, in);
}

static inline rsmp_var_t *rsmp_var_auto_string_len(rsmp_var_t **var, const char *in, size_t len)
{
    assert(var != NULL);
    if (!*var) return (*var = rsmp_var_from_string_len(in, len));
    return rsmp_var_set_string_len(*var, in, len);
}

static inline rsmp_var_t *rsmp_var_auto_int(rsmp_var_t **var, int64_t in)
{
    assert(var != NULL);
    if (!*var) return (*var = rsmp_var_from_int(in));
    return rsmp_var_set_int(*var, in);
}

static inline rsmp_var_t *rsmp_var_auto_uint(rsmp_var_t **var, uint64_t in)
{
    assert(var != NULL);
    if (!*var) return (*var = rsmp_var_from_uint(in));
    return rsmp_var_set_uint(*var, in);
}

static inline rsmp_var_t *rsmp_var_auto_double(rsmp_var_t **var, long double in)
{
    assert(var != NULL);
    if (!*var) return (*var = rsmp_var_from_double(in));
    return rsmp_var_set_double(*var, in);
}

static inline rsmp_var_t *rsmp_var_auto_bool(rsmp_var_t **var, bool in)
{
    assert(var != NULL);
    if (!*var) return (*var = rsmp_var_from_bool(in));
    return rsmp_var_set_bool(*var, in);
}

/**@}*/
#endif //CODER_VARS_H