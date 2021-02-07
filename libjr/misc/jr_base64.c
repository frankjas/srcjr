#include "ezport.h"

#include <apr-1/apr_base64.h>

#include "jr/misc.h"

jr_int jr_base64_encode (
	char *				encoded_dest,
	const char *		plain_src,
	jr_int				plain_src_length)
{
	jr_int				status;

	status	= apr_base64_encode (encoded_dest, plain_src, plain_src_length);

	return status;
}

jr_int jr_base64_encode_binary (
	char *					encoded_dest,
	const unsigned char *	plain_src,
	jr_int					plain_src_length)
{
	jr_int					status;

	status	= apr_base64_encode_binary (encoded_dest, plain_src, plain_src_length);

	return status;
}
