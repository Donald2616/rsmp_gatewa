#ifndef RSMP_FIELDS_MID_H
#define RSMP_FIELDS_MID_H

#include "rsmp/vars/uuid.h"

#ifdef RSMP_FIELDS_MID_C
#define MID_EXTERN
#define MID_PRIVATE
#endif

#ifndef MID_EXTERN
#define MID_EXTERN extern
#endif

#ifndef MID_PRIVATE
#define MID_PRIVATE const
#endif

typedef uuid_t mId_t;

#define rsmp_mId_new(_mId_)             uuid_generate((_mId_))
#define rsmp_mId_copy(_dest_,_src_)     uuid_copy((_dest_),(_src_))
#define rsmp_mId_cmp(_d1_,_d2_)         uuid_compare((_d1_),(_d2_))
#define rsmp_mId_clear(_id_)            uuid_clear((_id_))
#define rsmp_mId_is_null(_id_)          uuid_is_null((_id_))
#define rsmp_mId_to_string(_id_, _s_)   uuid_unparse((_id_), (_s_))
#define rsmp_mId_from_string(_s_, _id_) uuid_parse((_s_),(_id_))
#define rsmp_mId_set_null(_id_)         uuid_clear((_id_))

#define __rsmp_field_mId_encode   __rsmp_field_uuid_encode
#define __rsmp_field_mId_decode __rsmp_field_uuid_decode

#endif