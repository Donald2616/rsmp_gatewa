#ifndef RSMP_FIELDS_UID_H
#define RSMP_FIELDS_UID_H

#include "rsmp/vars/uuid.h"

#ifdef RSMP_FIELDS_UID_C
#define UID_EXTERN
#define UID_PRIVATE
#endif

#ifndef UID_EXTERN
#define UID_EXTERN extern
#endif

#ifndef UID_PRIVATE
#define UID_PRIVATE const
#endif

typedef uuid_t uId_t;

#define rsmp_uId_new(_uId_)             uuid_generate((_uId_))
#define rsmp_uId_copy(_dest_,_src_)     uuid_copy((_dest_),(_src_))
#define rsmp_uId_cmp(_d1_,_d2_)         uuid_compare((_d1_),(_d2_))
#define rsmp_uId_clear(_id_)            uuid_clear((_id_))
#define rsmp_uId_is_null(_id_)          uuid_is_null((_id_))
#define rsmp_uId_to_string(_id_, _s_)   uuid_unparse((_id_), (_s_))
#define rsmp_uId_from_string(_s_, _id_) uuid_parse((_s_),(_id_))
#define rsmp_uId_set_null(_id_)         uuid_clear((_id_))

#define __rsmp_field_uId_encode   __rsmp_field_uuid_encode
#define __rsmp_field_uId_decode __rsmp_field_uuid_decode

#endif