#include "ezport.h"

#ifndef ostype_winnt

#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <apr-1/apr_pools.h>
#include <apr-1/apr_portable.h>

#include "jr/apr.h"
#include "jr/error.h"


jr_int jr_socket_connect_unix (
	jr_socket_t *				socket_info,
	const char *				socket_path,
	char *						error_buf)
{
	apr_os_sock_info_t			apr_sock_info[1];

	struct sockaddr_un			sockaddr_un[1];
	unsigned jr_int				max_path_length		= sizeof( sockaddr_un->sun_path) - 1;
	jr_int						sd					= -1;
	jr_int						status;

	if (strlen( socket_path)  >  max_path_length) {
		jr_esprintf( error_buf, "path too long (%d): max. is %u",
			(jr_int) strlen( socket_path), max_path_length
		);
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	if (socket_info->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &socket_info->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	memset( sockaddr_un, 0, sizeof( *sockaddr_un));
	sockaddr_un->sun_family	= AF_LOCAL;
	snprintf( sockaddr_un->sun_path, max_path_length, "%s", socket_path);

	memset( apr_sock_info, 0, sizeof( *apr_sock_info));

	apr_sock_info->family	= sockaddr_un->sun_family;
	apr_sock_info->type		= SOCK_STREAM;
	apr_sock_info->protocol	= 0;


	sd	= socket( apr_sock_info->family, apr_sock_info->type, apr_sock_info->protocol);

	if (sd < 0) {
		jr_esprintf( error_buf, "couldn't create socket: %s", strerror( errno));
		status	= jr_ConvertErrno (status);
		goto return_status;
	}

	status	= connect( sd, (struct sockaddr *) sockaddr_un, sizeof( *sockaddr_un));

	if (status != 0) {
		jr_esprintf( error_buf, "couldn't connect socket: %s", strerror( errno));
		status	= jr_ConvertErrno (status);
		goto return_status;
	}

	apr_sock_info->os_sock	= (apr_os_sock_t *) &sd;

	/*
	** 6-5-2012: re-allocates the apr socket.
	*/
	status	= apr_os_sock_make (
				(apr_socket_t **) &socket_info->apr_socket_ptr,
				apr_sock_info,
				socket_info->apr_pool_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	if (socket_info->is_nonblocking) {
		/*
		** 2/27/07: The default is blocking.
		*/
		status	= jr_socket_set_nonblock (socket_info, 1, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}

	status	= 0;
	sd		= -1;

	return_status : {
		if (sd >= 0) {
			close( sd);
		}
	}
	return status;
}


#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
#endif
