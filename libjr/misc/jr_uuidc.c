#define _POSIX_SOURCE 1


#include "ezport.h"
#include <string.h>
#include "jr/uuid.h"
#include "jr/misc.h"


jr_int jr_UUIdCmp (void_ptr_1, void_ptr_2)
	const void *			void_ptr_1;
	const void *			void_ptr_2;
{
	const jr_UUIdStruct *	uuid_info_1				= void_ptr_1;
	const jr_UUIdStruct *	uuid_info_2				= void_ptr_2;

	jr_int					diff;


	diff	= memcmp (uuid_info_1->uuid_buf, uuid_info_2->uuid_buf, sizeof (uuid_info_1->uuid_buf));

	if (diff != 0) {
		return diff;
	}

	return 0;
}


jr_int jr_UUIdHash (void_ptr_1)
	const void *			void_ptr_1;
{
	const jr_UUIdStruct *	uuid_info_1				= void_ptr_1;

	jr_int					hash_value				= 0;

	char					uuid_buf [jr_UUID_STRING_LENGTH + 1];


	jr_UUIdToString (uuid_info_1, uuid_buf);

	hash_value	+= jr_strhash_pjw (uuid_buf);

	return hash_value;
}
