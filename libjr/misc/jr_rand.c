#include "ezport.h"

#include <apr-1/apr_general.h>

#include "jr/misc.h"
#include "jr/apr.h"
#include "jr/error.h"


jr_int jr_random_bytes (
	void *						bytes_ptr,
	jr_int						bytes_length,
	char *						opt_error_buf)
{
	jr_int						status;

	jr_apr_initialize();

	status	= apr_generate_random_bytes (bytes_ptr, bytes_length);

	if (status != 0) {
		if (opt_error_buf) {
			jr_esprintf (opt_error_buf, "%s", apr_strerror (status, opt_error_buf, jr_ERROR_LEN));
		}
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}
