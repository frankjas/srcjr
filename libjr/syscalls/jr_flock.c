#include "ezport.h"

#include <apr-1/apr_pools.h>
#include <apr-1/apr_file_io.h>

#include <setjmp.h>

#include "jr/apr.h"
#include "jr/error.h"

jr_int jr_file_lock (
	jr_file_t *					file_info,
	jr_int						lock_type,
	jr_int						time_out,
	char *						error_buf)
{
	jr_int						status;

	if (time_out > 0) {
		/*
		** 1/29/07: Don't have a usable timer mechanism, or signals with longjmp capabilities
		** under win32.
		*/
		jr_esprintf (error_buf, "blocking, timed locks not supported");
		status	= jr_INTERNAL_ERROR;
		goto return_status;
	}
	else {
		/*
		 * try a non-blocking lock
		 */
		jr_int					apr_type;

		if (lock_type == jr_FILE_LOCK_WRITE) {
			apr_type	= APR_FLOCK_EXCLUSIVE | APR_FLOCK_NONBLOCK;
		}
		else {
			apr_type	= APR_FLOCK_SHARED | APR_FLOCK_NONBLOCK;
		}

		status	= apr_file_lock (file_info->apr_file_ptr, apr_type);

		if (status != 0) {
			jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	status	= 0;

	return_status : {
	}
	return status;
}

jr_int jr_file_unlock (
	jr_file_t *					file_info,
	char *						error_buf)
{
	jr_int						status;

	status	= apr_file_unlock (file_info->apr_file_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}
