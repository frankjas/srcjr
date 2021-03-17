#ifndef __JR_INETLIB_H__
#define __JR_INETLIB_H__

#include "ezport.h"

#include <sys/types.h>

#ifdef ostype_winnt
#	include <winsock2.h>
#else
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#endif


#define jr_RCV_DISALLOW				0	/* shutdown modes */
#define jr_SND_DISALLOW				1	/* shutdown modes */
#define jr_ALL_DISALLOW				2	/* shutdown modes */

#define jr_MAX_IOVLEN				15
		/*
		 * 16 is the documented maximum, but 15 is the largest
		 * size that doesn't generate an error with sendmsg
		 */

#define jr_RANDOM_PORT				0

#define jr_CLASS_A_MASK				0xff000000
#define jr_CLASS_B_MASK				0xffff0000
#define jr_CLASS_C_MASK				0xffffff00

#define jr_INET_SYSTEM_ERROR		(jr_MIN_ERROR - 1)
#define jr_INET_TIMED_OUT			(jr_MIN_ERROR - 2)
#define jr_INET_HOST_NOT_FOUND		(jr_MIN_ERROR - 3)
#define jr_INET_BAD_ADDRESS_FAMILY	(jr_MIN_ERROR - 4)
#define jr_INET_CONNECTION_REFUSED	(jr_MIN_ERROR - 5)
#define jr_INET_UNREACHABLE			(jr_MIN_ERROR - 6)

#define jr_INET_MIN_ERROR			jr_INET_UNREACHABLE


extern jr_int		jr_InetAddressInit			PROTO ((
						struct sockaddr_in *	sa,
						const char *			hostname,
						jr_int    				portnum,
						char *					error_buf
					));

extern char *		jr_InetGetHostErrorString	PROTO ((
						jr_int					gethost_errno
					));

extern jr_int		jr_InetGetHostErrorTranslate	PROTO ((
						jr_int					gethost_errno
					));

extern jr_int		jr_InetGetPortNumber		PROTO ((
						jr_int					socket_fd,
						char *					opt_error_buf
					));

extern jr_int		jr_InetConnectToService		PROTO ((
						const char *			hostname,
						jr_int  				portnum,
						char *					error_buf
					));

extern jr_int		jr_InetConnectToServiceWithTimeOut	PROTO ((
						const char *			hostname,
						jr_int					portnum,
						jr_int					time_out_seconds,
						char *					error_buf
					));

extern jr_int		jr_InetOfferService			PROTO ((
						jr_int					socket_type,
						struct sockaddr_in *	socket_addr_ptr,
						char *					error_buf
					));


extern jr_int		jr_InetGetDGramSocket		PROTO ((
						const char *			localhostname,
						jr_int					port_number,
						char *					error_buf
					));


extern char *		jr_InetAddressToString		PROTO ((
						struct sockaddr_in *	in_addr,
						char *					string
					));

extern char *		jr_InetNumberToString		PROTO ((
						unsigned long			ip_number
					));

extern jr_int		jr_sockaddr_in_cmp			PROTO ((
						const void *			void_arg_1,
						const void *			void_arg_2
					));


#define				jr_SOCKET_READ_ERROR	-1			/* errno contains read() error */
#define				jr_SOCKET_READ_NO_SPACE	-2			/* buffer filled up before newline */
														/* in this case, buffer contains a string */
extern jr_int		jr_socket_readlines		PROTO ((
						jr_int				socket_rfd,
						char *				line_buffer,
						jr_int				line_buffer_size
					));
					/*
					 * Reads what's available, blocks if newline not seen.
					 */

/******** RPC Interface ********/

#define jr_RPC_PROTO_UDP		1
#define jr_RPC_PROTO_TCP		2

#define jr_RPC_MIN_PROGRAM_NUMBER	0x20000000

extern jr_int		jr_RPC_RegisterPort (
						jr_int				program_number,
						jr_int				version_number,
						jr_int				protocol,
						jr_int				port_number,
						char *				error_buf);

extern jr_int		jr_RPC_UnRegisterPort (
						jr_int				program_number,
						jr_int				version_number,
						char *				error_buf);

extern jr_int		jr_RPC_LookupPort (
						const char *		host_name,
						jr_int				program_number,
						jr_int				version_number,
						jr_int				protocol,
						char *				error_buf);


#endif
