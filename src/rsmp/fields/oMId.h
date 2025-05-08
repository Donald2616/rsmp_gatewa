#ifndef RSMP_FIELDS_OMID_H
#define RSMP_FIELDS_OMID_H

#include "rsmp/vars/uuid.h"

#ifdef RSMP_FIELDS_OMID_C
#define OMID_EXTERN
#define OMID_PRIVATE
#endif

#ifndef OMID_EXTERN
#define OMID_EXTERN extern
#endif

#ifndef OMID_PRIVATE
#define OMID_PRIVATE const
#endif

typedef uuid_t oMId_t;

#define rsmp_oMId_new(_oMId_)            uuid_generate((_oMId_))
#define rsmp_oMId_copy(_dest_,_src_)     uuid_copy((_dest_),(_src_))
#define rsmp_oMId_cmp(_d1_,_d2_)         uuid_compare((_d1_),(_d2_))
#define rsmp_oMId_clear(_id_)            uuid_clear((_id_))
#define rsmp_oMId_is_null(_id_)          uuid_is_null((_id_))
#define rsmp_oMId_to_string(_id_, _s_)   uuid_unparse((_id_), (_s_))
#define rsmp_oMId_from_string(_s_, _id_) uuid_parse((_s_),(_id_))
#define rsmp_oMId_set_null(_id_)         uuid_clear((_id_))

#define __rsmp_field_oMId_encode   __rsmp_field_uuid_encode
#define __rsmp_field_oMId_decode __rsmp_field_uuid_decode

#endif