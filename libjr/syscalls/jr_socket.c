#include "ezport.h"

#include <string.h>

#include <apr-1/apr_pools.h>
#include <apr-1/apr_network_io.h>
#include <apr-1/apr_lib.h>
#include <apr-1/apr_portable.h>

#include "jr/apr.h"
#include "jr/error.h"
#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/nettype.h"

extern void				jr_sockaddr_set_pool (
							jr_sockaddr_t *				sockaddr_info,
							void *						apr_pool_ptr);

extern jr_int			jr_sockaddr_init_pool (
							jr_sockaddr_t *				sockaddr_info,
							char *						error_buf);

extern jr_int			jr_sockaddr_reset (
							jr_sockaddr_t *				sockaddr_info,
							char *						error_buf);

extern jr_int			jr_sockaddr_alloc (
							jr_sockaddr_t *				sockaddr_info,
							char *						error_buf);

void jr_socket_init (
	jr_socket_t *				socket_info,
	jr_int						socket_type)
{
	memset (socket_info, 0, sizeof (jr_socket_t));

	socket_info->socket_type	= socket_type;

	jr_apr_initialize();
}

void jr_socket_undo (
	jr_socket_t *				socket_info)
{
	if (socket_info->apr_socket_ptr) {
		apr_socket_close (socket_info->apr_socket_ptr);
	}
	if (socket_info->apr_pool_ptr) {
		apr_pool_destroy (socket_info->apr_pool_ptr);
	}
	if (socket_info->aprbuf_ptr) {
		jr_aprbuf_destroy (socket_info->aprbuf_ptr);
	}
}

jr_socket_t *jr_socket_create (
	jr_int						socket_type)
{
	jr_socket_t *				socket_info		= jr_malloc (sizeof (*socket_info));

	jr_socket_init (socket_info, socket_type);

	return socket_info;
}

void jr_socket_destroy (
	jr_socket_t *				socket_info)
{
	jr_socket_undo (socket_info);
	jr_free (socket_info);
}

jr_int jr_socket_is_open(
	jr_socket_t *				socket_info)
{
	if (socket_info->apr_socket_ptr) {
		return 1;
	}
	return 0;
}

jr_int jr_socket_init_pool (
	jr_socket_t *				socket_info,
	char *						error_buf)
{
	jr_int						status;

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
	status	= 0;

	return_status : {
	}
	return status;
}

void jr_sockaddr_init (
	jr_sockaddr_t *				sockaddr_info)
{
	memset (sockaddr_info, 0, sizeof (jr_sockaddr_t));

	jr_apr_initialize();
}

void jr_sockaddr_undo (
	jr_sockaddr_t *				sockaddr_info)
{
	if (sockaddr_info->apr_pool_ptr  &&  !sockaddr_info->has_borrowed_pool) {
		apr_pool_destroy (sockaddr_info->apr_pool_ptr);
	}
}

void jr_sockaddr_set_pool (
	jr_sockaddr_t *				sockaddr_info,
	void *						apr_pool_ptr)
{
	if (sockaddr_info->apr_pool_ptr  &&  !sockaddr_info->has_borrowed_pool) {
		apr_pool_destroy (sockaddr_info->apr_pool_ptr);
	}
	sockaddr_info->apr_pool_ptr			= apr_pool_ptr;
	sockaddr_info->has_borrowed_pool	= 1;
}

jr_int jr_sockaddr_init_pool (
	jr_sockaddr_t *				sockaddr_info,
	char *						error_buf)
{
	jr_int						status;

	if (sockaddr_info->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &sockaddr_info->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	status = 0;

	return_status : {
	}
	return status;
}

jr_int jr_sockaddr_reset (
	jr_sockaddr_t *				sockaddr_info,
	char *						error_buf)
{
	apr_pool_t *				borrowed_pool_ptr		= 0;
	jr_int						status;

	if (sockaddr_info->has_borrowed_pool) {
		borrowed_pool_ptr	= sockaddr_info->apr_pool_ptr;
	}
	jr_sockaddr_undo( sockaddr_info);
	jr_sockaddr_init( sockaddr_info);

	if (borrowed_pool_ptr) {
		jr_sockaddr_set_pool( sockaddr_info, borrowed_pool_ptr);
	}
	else {
		status	= jr_sockaddr_init_pool( sockaddr_info, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}

	sockaddr_info->apr_sockaddr_ptr	 = 0;

	status = 0;

	return_status : {
	}
	return status;
}

jr_int jr_sockaddr_alloc (
	jr_sockaddr_t *				sockaddr_info,
	char *						error_buf)
{
	jr_int						status;

	status	= jr_sockaddr_reset( sockaddr_info, error_buf);

	if (status != 0) {
		goto return_status;
	}

	sockaddr_info->apr_sockaddr_ptr	 = apr_palloc( sockaddr_info->apr_pool_ptr, sizeof (apr_sockaddr_t));

	status = 0;

	return_status : {
	}
	return status;
}

jr_int jr_sockaddr_lookup (
	jr_sockaddr_t *				sockaddr_info,
	const char *				host_name,
	jr_int						port_number,
	char *						error_buf)
{
	jr_int						addr_family			= APR_INET;
	jr_int						status;

	status	= jr_sockaddr_reset( sockaddr_info, error_buf);

	if (status != 0) {
		goto return_status;
	}


	if (host_name == 0  ||  strcmp (host_name, "0.0.0.0") == 0) {
		host_name	= 0;
	}

	status	= apr_sockaddr_info_get (
				(apr_sockaddr_t **) &sockaddr_info->apr_sockaddr_ptr,
				host_name,
				addr_family,
				port_number,
				0,								/* 2/23/05: could be: APR_IPV4_ADDR_OK */
				sockaddr_info->apr_pool_ptr
			);

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

jr_sockaddr_t *jr_sockaddr_init_foreach (
	jr_sockaddr_t *		sockaddr_info)
{
	jr_sockaddr_t *				curr_sockaddr_ptr		= sockaddr_info;
	jr_sockaddr_t *				next_sockaddr_ptr;
	apr_sockaddr_t *			next_apr_sockaddr_ptr;

	for (	next_apr_sockaddr_ptr = ((apr_sockaddr_t *)sockaddr_info->apr_sockaddr_ptr)->next;
			next_apr_sockaddr_ptr;
			next_apr_sockaddr_ptr = next_apr_sockaddr_ptr->next
	) {
		next_sockaddr_ptr	 = apr_palloc( sockaddr_info->apr_pool_ptr, sizeof (jr_sockaddr_t));

		jr_sockaddr_init( next_sockaddr_ptr);
		jr_sockaddr_set_pool( next_sockaddr_ptr, sockaddr_info->apr_pool_ptr);

		next_sockaddr_ptr->apr_sockaddr_ptr = next_apr_sockaddr_ptr;

		curr_sockaddr_ptr->next_sockaddr_ptr	= next_sockaddr_ptr;
		curr_sockaddr_ptr						= next_sockaddr_ptr;
	}
	return sockaddr_info;
}

jr_int jr_sockaddr_dup (
	const jr_sockaddr_t *		src_sockaddr_info,
	jr_sockaddr_t *				dest_sockaddr_info,
	char *						error_buf)
{
	jr_int						status;


	status = jr_sockaddr_alloc( dest_sockaddr_info, error_buf);

	if (status != 0) {
		/*
		** 2-18-2013: fails if no pool can be allocated.
		*/
		return status;
	}

	apr_sockaddr_dup(
		(apr_sockaddr_t *)src_sockaddr_info->apr_sockaddr_ptr,
		(apr_sockaddr_t *)dest_sockaddr_info->apr_sockaddr_ptr
	);

	return 0;
}

jr_int jr_sockaddr_cmp (
	const jr_sockaddr_t *		sockaddr_info_1,
	const jr_sockaddr_t *		sockaddr_info_2)
{
	jr_int						is_equal;
	jr_int						port_1;
	jr_int						port_2;

	is_equal	= apr_sockaddr_equal( 
					(apr_sockaddr_t *) sockaddr_info_1->apr_sockaddr_ptr,
					(apr_sockaddr_t *) sockaddr_info_2->apr_sockaddr_ptr
				);

	if (!is_equal) {
		return 1;
	}

	port_1		= jr_sockaddr_port( sockaddr_info_1);
	port_2		= jr_sockaddr_port( sockaddr_info_1);

	if (port_1 < port_2) {
		return -1;
	}
	if (port_1 > port_2) {
		return 1;
	}

	return 0;
}

void *jr_sockaddr_ipaddr_ptr (
	const jr_sockaddr_t *		sockaddr_info)
{
	return ((apr_sockaddr_t *) sockaddr_info->apr_sockaddr_ptr)->ipaddr_ptr;
}

jr_int jr_sockaddr_ipaddr_len (
	const jr_sockaddr_t *		sockaddr_info)
{
	return ((apr_sockaddr_t *) sockaddr_info->apr_sockaddr_ptr)->ipaddr_len;
}

jr_int jr_sockaddr_port (
	const jr_sockaddr_t *		sockaddr_info)
{
	return ((apr_sockaddr_t *) sockaddr_info->apr_sockaddr_ptr)->port;
}

jr_int jr_sockaddr_network_number(
	const jr_sockaddr_t *		sockaddr_info)
{
	return jr_inet_network_number( *(unsigned jr_int *) jr_sockaddr_ipaddr_ptr( sockaddr_info));
}

jr_int jr_sockaddr_host_number(
	const jr_sockaddr_t *		sockaddr_info)
{
	return jr_inet_host_number( *(unsigned jr_int *) jr_sockaddr_ipaddr_ptr( sockaddr_info));
}

jr_int jr_sockaddr_ip_cmp (
	const jr_sockaddr_t *		sockaddr_info_1,
	const jr_sockaddr_t *		sockaddr_info_2)
{
	jr_int						is_equal;

	is_equal	= apr_sockaddr_equal( 
					(apr_sockaddr_t *) sockaddr_info_1->apr_sockaddr_ptr,
					(apr_sockaddr_t *) sockaddr_info_2->apr_sockaddr_ptr
				);

	if (!is_equal) {
		return 1;
	}
	return 0;
}

const char *jr_sockaddr_get_ip_str (
	jr_sockaddr_t *				sockaddr_info)
{
	jr_int						status;

	if (sockaddr_info->ip_str == 0) {
		jr_int					addr_str_len;

		/*
		** 2-18-2013: could also initialize a pool and ignore the failure case
		*/
		addr_str_len			= ((apr_sockaddr_t *) sockaddr_info->apr_sockaddr_ptr)->addr_str_len;
		sockaddr_info->ip_str	= apr_palloc( sockaddr_info->apr_pool_ptr, addr_str_len);

		status = apr_sockaddr_ip_getbuf(
					(char *) sockaddr_info->ip_str,
					addr_str_len,
					(apr_sockaddr_t *) sockaddr_info->apr_sockaddr_ptr
				);

		if (status != 0) {
			return "<unsupported address family>";
		}
	}
	return sockaddr_info->ip_str;
}

jr_int jr_socket_open_generic (
	jr_socket_t *				socket_info,
	jr_int						opt_os_sock,
	char *						error_buf)
{
	jr_int						status;
	jr_int						apr_family			= APR_INET;
	jr_int						apr_type;
	jr_int						apr_protocol		= APR_UNSPEC;

	status	= jr_socket_init_pool( socket_info, error_buf);

	if (status != 0) {
		goto return_status;
	}

	switch (socket_info->socket_type) {
		case jr_SOCK_STREAM			: apr_type = SOCK_STREAM; break;
		case jr_SOCK_DGRAM			: apr_type = SOCK_DGRAM; break;
		default : {
			jr_esprintf (error_buf, "bad socket type %d", socket_info->socket_type);
			status	= jr_MISUSE_ERROR;
			goto return_status;
		}
	}

	if (opt_os_sock != jr_INVALID_OS_SOCKET) {
		apr_os_sock_info_t			os_sock_info[1];
		apr_os_sock_t				os_sock				= opt_os_sock;

		memset( os_sock_info, 0, sizeof( *os_sock_info));

		os_sock_info->os_sock		= &os_sock;
		os_sock_info->family		= apr_family;
		os_sock_info->type			= apr_type;
		os_sock_info->protocol		= apr_protocol;

		status	= apr_os_sock_make(
					(apr_socket_t **) &socket_info->apr_socket_ptr,
					os_sock_info,
					socket_info->apr_pool_ptr
				);

		if (status != 0) {
			jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}
	else {

		status	= apr_socket_create (
					(apr_socket_t **) &socket_info->apr_socket_ptr,
					apr_family,
					apr_type,
					apr_protocol,
					socket_info->apr_pool_ptr
				);

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


jr_int jr_socket_close (
	jr_socket_t *				socket_info,
	char *						error_buf)
{
	jr_int						status;


	if (socket_info->aprbuf_ptr  &&  jr_aprbuf_needs_flush (socket_info->aprbuf_ptr)) {
		status	= jr_socket_flush (socket_info, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}

	if (socket_info->apr_socket_ptr) {
		status	= apr_socket_close (socket_info->apr_socket_ptr);

		if (status != 0) {
			jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	status	= 0;

	return_status : {
		if (socket_info->apr_pool_ptr) {
			apr_pool_destroy (socket_info->apr_pool_ptr);
		}

		socket_info->apr_socket_ptr		= 0;
		socket_info->apr_pool_ptr		= 0;
	}
	return status;
}

jr_int jr_socket_connect (
	jr_socket_t *				socket_info,
	const char *				host_name,
	jr_int						port_number,
	char *						error_buf)
{
	jr_sockaddr_t				sockaddr_info[1];
	jr_int						opened_socket			= 0;
	jr_int						status;


	jr_sockaddr_init (sockaddr_info);

	if (socket_info->socket_type != jr_SOCK_STREAM) {
		jr_esprintf (error_buf, "non-stream socket for connect()");
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}

	status	= jr_sockaddr_lookup (sockaddr_info, host_name, port_number, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't find host '%.32s': %s", host_name, error_buf);
		goto return_status;
	}

	status	= jr_socket_open (socket_info, error_buf);

	if (status != 0) {
		goto return_status;
	}
	else {
		opened_socket	= 1;
	}

	status	= apr_socket_connect (
				socket_info->apr_socket_ptr,
				sockaddr_info->apr_sockaddr_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	if (socket_info->is_nonblocking) {
		/*
		** 2/27/07: The default is blocking.
		** 2/10/13: Set non-blocking after the connect since a blocking poll() on windows
		** will hang on a non-blocking connect if there's no listener.
		*/
		status	= jr_socket_set_nonblock (socket_info, 1, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}
	status	= 0;

	return_status : {
		jr_sockaddr_undo (sockaddr_info);

		if (status != 0  &&  opened_socket) {
			jr_socket_close (socket_info, 0);
		}
	}
	return status;
}

jr_int jr_socket_open_dgram (
	jr_socket_t *				socket_info,
	const char *				local_host_name,
	jr_int						port_number,
	char *						error_buf)
{
	jr_int						status;


	status	= jr_socket_offer_service( socket_info, local_host_name, port_number, error_buf);

	if (status != 0) {
		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}

jr_int jr_socket_offer_service (
	jr_socket_t *				socket_info,
	const char *				host_name,
	jr_int						port_number,
	char *						error_buf)
{
	jr_sockaddr_t				sockaddr_info[1];
	jr_int						opened_socket			= 0;
	jr_int						status;


	jr_sockaddr_init (sockaddr_info);

	/*
	** 2-22-2013: need to initialize the pool before borrowing it for the sockaddr.
	*/
	status	= jr_socket_init_pool( socket_info, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't init pool: %s", error_buf);
		goto return_status;
	}

	jr_sockaddr_set_pool( sockaddr_info, socket_info->apr_pool_ptr);
	/*
	** 2-22-2013: use the socket's pool since apr_socket_bind() takes the
	** apr_sockaddr_t and points to it.
	*/

	status	= jr_sockaddr_lookup (sockaddr_info, host_name, port_number, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't find host '%.32s': %s", host_name, error_buf);
		goto return_status;
	}

	status	= jr_socket_open (socket_info, error_buf);

	if (status != 0) {
		goto return_status;
	}
	else {
		opened_socket	= 1;
	}

	status	= apr_socket_bind (
				socket_info->apr_socket_ptr,
				sockaddr_info->apr_sockaddr_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't bind: %s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	if (socket_info->socket_type == jr_SOCK_STREAM) {
		status	= apr_socket_listen (socket_info->apr_socket_ptr, 3);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't set backlog: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	if (socket_info->is_nonblocking) {
		status	= jr_socket_set_nonblock (socket_info, 1, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}

	status	= 0;

	return_status : {
		jr_sockaddr_undo (sockaddr_info);

		if (status != 0 && opened_socket) {
			jr_socket_close (socket_info, 0);
		}
	}
	return status;
}

jr_int jr_socket_accept (
	jr_socket_t *				accept_socket_info,
	jr_socket_t *				client_socket_info,
	char *						error_buf)
{
	jr_int						status;

	if (accept_socket_info->socket_type != jr_SOCK_STREAM) {
		jr_esprintf (error_buf, "non-stream accept socket for accept()");
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}
	if (client_socket_info->socket_type != jr_SOCK_STREAM) {
		jr_esprintf (error_buf, "non-stream client socket for accept()");
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}

	status	= jr_socket_init_pool( client_socket_info, error_buf);

	if (status != 0) {
		goto return_status;
	}

	status	= apr_socket_accept (
				(apr_socket_t **) &client_socket_info->apr_socket_ptr,
				accept_socket_info->apr_socket_ptr,
				client_socket_info->apr_pool_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));

#		ifdef ostype_winnt
		{
			status	= jr_ConvertLastError (APR_TO_OS_ERROR(status));
		}
#		else
		{
			status	= jr_ConvertErrno (APR_TO_OS_ERROR(status));
		}
#		endif

		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}


jr_int jr_socket_shutdown (
	jr_socket_t *				socket_info,
	char *						error_buf)
{
	jr_int						status;

	if (socket_info->aprbuf_ptr  &&  jr_aprbuf_needs_flush (socket_info->aprbuf_ptr)) {
		status	= jr_socket_flush (socket_info, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}

	if (socket_info->apr_socket_ptr) {
		status	= apr_socket_shutdown (socket_info->apr_socket_ptr, APR_SHUTDOWN_READWRITE);

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

jr_int jr_socket_set_nonblock (
	jr_socket_t *				socket_info,
	jr_int						value,
	char *						error_buf)
{
	jr_int						status;

	socket_info->is_nonblocking	= value != 0;

	if (socket_info->apr_socket_ptr) {
		
		if (socket_info->is_nonblocking) {
			status	= apr_socket_opt_set (socket_info->apr_socket_ptr, APR_SO_NONBLOCK, 1);

			if (status != 0) {
				jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
				status	= jr_ConvertAprErrno (status);
				goto return_status;
			}

			status	= apr_socket_timeout_set (socket_info->apr_socket_ptr, 0);

			if (status != 0) {
				jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
				status	= jr_ConvertAprErrno (status);
				goto return_status;
			}
		}
		else {
			status	= apr_socket_opt_set (socket_info->apr_socket_ptr, APR_SO_NONBLOCK, 0);

			if (status != 0) {
				jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
				status	= jr_ConvertAprErrno (status);
				goto return_status;
			}

			status	= apr_socket_timeout_set (socket_info->apr_socket_ptr, -1);

			if (status != 0) {
				jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
				status	= jr_ConvertAprErrno (status);
				goto return_status;
			}
		}
	}

	status = 0;

	return_status : {
	}
	return status;
}

jr_int jr_socket_port(
	jr_socket_t *				socket_info,
	char *						error_buf)
{
	jr_int						status;
	apr_sockaddr_t *			sockaddr_ptr;

	status	= apr_socket_addr_get( &sockaddr_ptr, APR_LOCAL, socket_info->apr_socket_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= sockaddr_ptr->port;

	return_status : {
	}
	return status;
}

jr_int jr_socket_recv (
	jr_socket_t *				socket_info,
	void *						data_buf,
	jr_int						data_buf_len,
	char *						error_buf)
{
	jr_int						status;
	apr_size_t					apr_length			= data_buf_len;

	if (socket_info->socket_type != jr_SOCK_STREAM) {
		jr_esprintf (error_buf, "socket type not 'stream'");
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}
	if (data_buf_len > jr_INT_MAX) {
		jr_esprintf (error_buf, "recv length %d too large, max is %d", data_buf_len, jr_INT_MAX);
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}

	status	= apr_socket_recv (
				socket_info->apr_socket_ptr,
				data_buf,
				&apr_length
			);

	if (APR_STATUS_IS_EOF (status)) {
		status	= 0;
	}
	else if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status = (jr_int) apr_length;

	return_status : {
	}
	return status;
}

jr_int jr_socket_send (
	jr_socket_t *				socket_info,
	const void *				data_buf,
	jr_int						data_buf_len,
	char *						error_buf)
{
	jr_int						status;
	apr_size_t					apr_length			= data_buf_len;

	if (socket_info->socket_type != jr_SOCK_STREAM) {
		jr_esprintf (error_buf, "socket type not 'stream'");
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}
	if (data_buf_len > jr_INT_MAX) {
		jr_esprintf (error_buf, "send length %d too large, max is %d", data_buf_len, jr_INT_MAX);
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}

	if (socket_info->aprbuf_ptr  &&  jr_aprbuf_needs_flush (socket_info->aprbuf_ptr)) {
		/*
		** 3/29/08: allow abitrary combinations of jr_socket_send() and jr_socket_printf()
		*/
		status	= jr_socket_flush (socket_info, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}

	status	= apr_socket_send (
				socket_info->apr_socket_ptr,
				data_buf,
				&apr_length
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status = (jr_int) apr_length;

	return_status : {
	}
	return status;
}

jr_int jr_socket_sendto_iov (
	jr_socket_t *				socket_info,
	jr_sockaddr_t *				sockaddr_info,
	struct iovec *				iov,
	jr_int						iovlen,
	char *						error_buf)
{
	jr_int						flags				= 0;
	jr_int						status;
	apr_size_t					apr_length;

	if (socket_info->socket_type == jr_SOCK_STREAM) {
		if (socket_info->aprbuf_ptr  &&  jr_aprbuf_needs_flush (socket_info->aprbuf_ptr)) {
			/*
			** 3/29/08: allow abitrary combinations of jr_socket_send() and jr_socket_printf()
			*/
			status	= jr_socket_flush (socket_info, error_buf);

			if (status != 0) {
				goto return_status;
			}
		}
	}

	status	= apr_socket_sendto_iov (
				socket_info->apr_socket_ptr,
				sockaddr_info->apr_sockaddr_ptr,
				flags, iov, iovlen,
				&apr_length
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status = (jr_int) apr_length;

	return_status : {
	}
	return status;
}

jr_int jr_socket_recvfrom (
	jr_socket_t *				socket_info,
	jr_sockaddr_t *				sockaddr_info,
	void *						data_buf,
	jr_int						data_buf_len,
	char *						error_buf)
{
	jr_int						flags				= 0;
	jr_int						status;
	apr_size_t					apr_length			= data_buf_len;

	if (socket_info->socket_type != jr_SOCK_DGRAM) {
		jr_esprintf (error_buf, "socket type not 'dgram'");
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}

	status	= jr_sockaddr_alloc( sockaddr_info, error_buf);

	if (status != 0) {
		goto return_status;
	}

	status	= apr_socket_recvfrom (
				sockaddr_info->apr_sockaddr_ptr,
				socket_info->apr_socket_ptr,
				flags,
				data_buf,
				&apr_length
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status = (jr_int) apr_length;

	return_status : {
	}
	return status;
}

jr_int jr_socket_printf (
	jr_socket_t *				socket_info,
	const char *				control_string,
								...)
{
	va_list						arg_list;
	jr_int						nbytes;
	jr_int						status;

	va_start (arg_list, control_string);

	if (socket_info->aprbuf_ptr == 0) {
		socket_info->aprbuf_ptr	= jr_aprbuf_create (
									socket_info,
									(jr_aprbuf_writefn_t) jr_socket_send,
									jr_SOCKBUF_DEFAULT_SIZE
								);
	}

	jr_aprbuf_set_error_buf (socket_info->aprbuf_ptr, 0);

	if (socket_info->socket_type != jr_SOCK_STREAM) {
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}

	status	= jr_aprbuf_vformatter ( socket_info->aprbuf_ptr, control_string, arg_list );

	if (status < 0) {
		/*
		** 2/7/07: only fails if flush() fails, so use that status, error_buf already filled in
		*/
		status	= jr_ConvertAprErrno (jr_aprbuf_status (socket_info->aprbuf_ptr));
		goto return_status;
	}
	else {
		nbytes	= status;
	}

	/*
	** 3/29/08: Not all data may be written out, need to call jr_socket_flush()
	** to flush any buffered data.
	*/
	
	status	= nbytes;

	return_status : {
		va_end (arg_list);
	}

	return status;
}

jr_int jr_socket_flush (
	jr_socket_t *				socket_info,
	char *						error_buf)
{
	jr_int						status;

	if (socket_info->aprbuf_ptr  &&  jr_aprbuf_needs_flush (socket_info->aprbuf_ptr)) {

		jr_aprbuf_set_error_buf (socket_info->aprbuf_ptr, error_buf);

		status	= jr_aprbuf_flush (socket_info->aprbuf_ptr);

		if (status != 0) {
			goto return_status;
		}
	}
	status = 0;

	return_status : {
	}
	return status;
}

jr_int jr_socket_cmp (
	const void *				void_ptr_1,
	const void *				void_ptr_2)
{
	const jr_socket_t *			socket_ptr_1			= void_ptr_1;
	const jr_socket_t *			socket_ptr_2			= void_ptr_2;

	return jr_ptrcmp (socket_ptr_1->apr_socket_ptr, socket_ptr_2->apr_socket_ptr);
}

jr_int jr_socket_hash (
	const void *				void_ptr)
{
	const jr_socket_t *			socket_ptr				= void_ptr;

	return jr_ptrhash (socket_ptr->apr_socket_ptr);
}
