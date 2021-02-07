#ifndef __jr_uuid_h___
#define __jr_uuid_h___

#include "ezport.h"

#include "jr/misc.h"

typedef struct {
	unsigned char		uuid_buf [16];
} jr_UUIdStruct;

/*
 * A UUID is a 128 bit quantity, or 16 bytes.
 *
 * Under Linux, the uuid_t type is 16 byte char array typedef.
 * So the uuid_buf field above is type compatible with that definition.
 *
 *
 * jr_malloc():     use   jr_UUIdAvoidMallocLeak()
 *
 * The system uuid_generate() function may leave a file open (/dev/urandom).
 * To account for this file, call jr_UUIdAvoidMallocLeak()
 * before malloc checking is turned on.
 *
 */


#define					jr_UUID_STRING_LENGTH		36


#define					jr_UUIdAsgn(dest_uuid, src_uuid)			\
						memcpy (dest_uuid, src_uuid, sizeof (jr_UUIdStruct))

extern void				jr_UUIdAsgnRandom			PROTO ((
							jr_UUIdStruct *			uuid_info
						));

extern void				jr_UUIdAsgnTime				PROTO ((
							jr_UUIdStruct *			uuid_info
						));

extern void				jr_UUIdAsgnFromString		PROTO ((
							jr_UUIdStruct *			uuid_info,
							const char *			string_buf
						));

extern char *			jr_UUIdToString				PROTO ((
							const jr_UUIdStruct *	uuid_info,
							char *					string_buf
						));

extern jr_int			jr_UUIdCmp					PROTO ((
							const void *			void_ptr_1,
							const void *			void_ptr_2
						));

extern jr_int			jr_UUIdHash					PROTO ((
							const void *			void_ptr_1
						));

extern void				jr_UUIdAvoidMallocLeak		PROTO ((void));


#define					jr_UUIdAsgnNull(dest_uuid)					\
						memset (dest_uuid, 0, sizeof (jr_UUIdStruct))

#define					jr_UUIdIsNull(dest_uuid)					\
						jr_memisset (dest_uuid, 0, sizeof (jr_UUIdStruct))


#endif
