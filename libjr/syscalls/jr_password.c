#include "ezport.h"

#include <apr-1/apr_md5.h>
#include <apr-1/apr_sha1.h>

#include "jr/error.h"

jr_int jr_password_check(
	const char *			password_str,
	const char *			hashed_str)
{
	apr_status_t			status;

	status	= apr_password_validate( password_str, hashed_str);

	if (status == 0) {
		return 0;
	}
	return jr_PERMISSION_ERROR;
}

void jr_password_hash( 
	const char *			password_str,
	const char *			salt_str,
	char *					hash_buf,
	jr_int					hash_buf_len)
{
	apr_md5_encode( password_str, salt_str, hash_buf, hash_buf_len);
}
