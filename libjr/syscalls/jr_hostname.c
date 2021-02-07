#include "ezport.h"

#include <apr-1/apr_pools.h>
#include <apr-1/apr_network_io.h>

#include "jr/apr.h"
#include "jr/error.h"

jr_int jr_gethostname(
	char *						host_name,
	jr_int						buf_length,
	char *						error_buf)
{
	apr_pool_t *				apr_pool_ptr			= 0;
	jr_int						status;

	jr_apr_initialize();

	apr_pool_create (&apr_pool_ptr, NULL);

	status		= apr_gethostname( host_name, buf_length, apr_pool_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status		= 0;

	return_status : {
		if (apr_pool_ptr) {
			apr_pool_destroy (apr_pool_ptr);
		}
	}

	return status;
}
