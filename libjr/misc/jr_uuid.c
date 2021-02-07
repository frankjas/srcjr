#define _POSIX_SOURCE 1


#include "ezport.h"

#include <apr-1/apr_uuid.h>

#include "jr/uuid.h"


void jr_UUIdAsgnRandom (uuid_info)
	jr_UUIdStruct *			uuid_info;
{
	apr_uuid_get ((apr_uuid_t *) uuid_info);
}

void jr_UUIdAsgnTime (uuid_info)
	jr_UUIdStruct *			uuid_info;
{
	apr_uuid_get ((apr_uuid_t *) uuid_info);
}

void jr_UUIdAsgnFromString (uuid_info, string_buf)
	jr_UUIdStruct *			uuid_info;
	const char *			string_buf;
{
	apr_uuid_parse ((apr_uuid_t *) uuid_info, string_buf);
}


char *jr_UUIdToString (uuid_info, string_buf)
	const jr_UUIdStruct *	uuid_info;
	char *					string_buf;
{
	apr_uuid_format (string_buf, (apr_uuid_t *) uuid_info);

	return string_buf;
}


void jr_UUIdAvoidMallocLeak ()
{
	apr_uuid_t				tmp_uuid[1];

	apr_uuid_get (tmp_uuid);
}

