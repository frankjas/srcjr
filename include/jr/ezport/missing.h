#ifndef __MISSING_HEADER__
#define __MISSING_HEADER__



/******** Missing Types ********/

#ifdef missing_unix_sys_types
	typedef	jr_short        ino_t;
	typedef	jr_short	    dev_t;
	typedef	jr_int	        off_t;
	typedef	jr_int          uid_t;
	typedef	jr_int          gid_t;
	typedef	jr_int          pid_t;
#endif

#ifdef missing_mode_t
	typedef jr_int		    mode_t;
#endif

#ifdef missing_off_t
	typedef jr_int		    off_t;
#endif

#ifdef missing_ssize_type
	typedef jr_int			ssize_t;
#endif

#ifdef missing_addr_types
	typedef	jr_int	        daddr_t;
	typedef	char *	        caddr_t;
#endif

#ifdef missing_struct_iov

struct iovec {
	void *			iov_base;	/* Pointer to data.  */
	size_t			iov_len;	/* Length of data.  */
};
#define APR_IOVEC_DEFINED
/*
** 2-28-2013: APR may also add it, turn off the def. in APR header files
** Note won't affect build of APR since that doesn't include this header.
*/

#endif

#ifdef missing_struct_msghdr
#include <sys/types.h>

struct msghdr {
	caddr_t					msg_name;			/* optional address */
	jr_int					msg_namelen;		/* size of address */
	struct	iovec *			msg_iov;			/* scatter/gather array */
	jr_int					msg_iovlen;			/* # elements in msg_iov */
	caddr_t					msg_accrights;		/* access rights sent/received */
	jr_int					msg_accrightslen;
};

#endif

#ifdef missing_msghdr_accrights_fields
#	define msg_accrights		msg_control
#	define msg_accrightslen		msg_controllen
#endif

/******** Missing #defines ********/

#ifdef missing_seek_constants
#	define SEEK_SET 0
#	define SEEK_CUR 1
#	define SEEK_END 2
#endif

#ifdef missing_sigtrap
#	ifdef vendor_netbsd
#		if (__version__ <= 1100)
#			define SIGTRAP 5		 /* from <sys/signal.h> */

#		endif
#	endif
#endif

#ifdef missing_sigiot
#	define SIGIOT			SIGABRT		/* according to several sources */
#endif

#ifdef missing_sigpwr
#	define SIGPWR			SIGUSR1
#endif

/* .bp */

/******** Missing Functions/Macros ********/

#ifdef missing_strerror
	extern char *sys_errlist [];
#	define strerror(errnum)	(sys_errlist[errnum])
#endif

#ifdef	missing_ascii_in_ctype
#	define isascii(c)	((unsigned)(c)<=0177)
#	define toascii(c)	((int)((c)&0177))
#endif


#ifdef missing_strchr
	extern char *strchr      PROTO ((const char *str, int chr)) ;
#endif

#ifdef missing_strrchr
	extern char *strrchr     PROTO ((const char *str, int chr)) ;
#endif

#ifdef missing_strstr
	extern char *strstr      PROTO ((const char *str, const char *pat)) ;
#endif

#ifdef missing_strcasestr
	extern char *strcasestr	PROTO ((const char *str, const char *pat)) ;
#endif

#ifdef missing_strcasecmp
#	define missing_strcasecmp_extern
#endif

#ifdef missing_strncasecmp
#	define missing_strncasecmp_extern
#endif

#ifdef missing_strdup
#	define missing_strdup_extern
#endif

#ifdef missing_memcpy
#	define missing_memcpy_extern
#endif

#ifdef missing_memset
#	define missing_memset_extern
#endif

#ifdef missing_putenv
	extern int	putenv		PROTO((char *	name_equals_value_str)) ;
#endif

#ifdef missing_wifcore_macro
#	ifdef ostype_cygwin
#		define WIFCORE(x)     WCOREDUMP(x)
#	else
#		define WIFCORE(x)     ( (*((int *) &x)) & 0200)
#	endif
#endif
/* .bp */

#ifdef missing_bzero
#	define bzero(s,t,n)		memcpy (t,s,n)
#endif

#ifdef missing_roundf
#	define roundf(fval)		floorf( fval + 0.5)
#endif

/******** Missing Externs ********/

#ifdef missing_bind_extern
#	include <sys/types.h>
#	include <sys/socket.h>

	extern int bind PROTO ((int socket, struct sockaddr *addr_ptr, int addr_len));
#endif

#ifdef missing_byte_order_externs
#	ifdef	has_machine_endian_h
#		include <machine/endian.h>
#	else
		extern unsigned long	htonl	PROTO ((unsigned long hostlong));
		extern unsigned long	ntohl	PROTO ((unsigned long hostlong));
		extern unsigned short	htons	PROTO ((unsigned short hostshort));
		extern unsigned short	ntohs	PROTO ((unsigned short hostshort));
#	endif
#endif

#ifdef missing_bzero_extern
	/*
	 * used in macro FD_ZERO
	 */
	 extern void	bzero		PROTO ((void *ptr, unsigned int nbytes));
#endif

#ifdef missing_connect_extern
#	include <sys/types.h>
#	include <sys/socket.h>

	extern int connect PROTO ((int socket, struct sockaddr *addr_ptr, int addr_len));
#endif

#ifdef missing_crypt_extern
	extern char * crypt PROTO ((const char *key, const char *salt));
#endif

#ifdef missing_errno_externs
	extern int errno ;
	extern char *sys_errlist [];
#endif

#ifdef missing_fclose_extern
#	include <stdio.h>
	extern int	fclose	PROTO ((FILE *fp));
#endif

#ifdef missing_fflush_extern
#	include <stdio.h>
	extern int	fflush	PROTO ((FILE *fp));
#endif

#ifdef missing__filbuf_extern
#	include <stdio.h>
	extern int	_filbuf	PROTO ((FILE *fp));
#endif

#ifdef missing_flock_extern
	extern int		flock		PROTO ((int fd, int lock_type));
#endif

#ifdef missing__flsbuf_extern
#	include <stdio.h>
	extern int	_flsbuf	PROTO ((unsigned int c, FILE *fp));
#endif

#ifdef missing_fsync_extern
	extern int			fsync PROTO ((int fd));
#endif

#ifdef missing_ftruncate_extern
#	include <sys/types.h>

	extern int 			ftruncate PROTO ((int fd, off_t length));
#endif

#ifdef missing_gethostname_extern
	extern int gethostname PROTO ((char *name_buffer, int max_length));
#endif

#ifdef missing_getsockname_extern
#	include <sys/types.h>
#	include <sys/socket.h>

	extern int getsockname PROTO ((int socket, struct sockaddr *addr_ptr, int *addr_len_ptr));
#endif

#ifdef missing_getwd_extern
	extern char * getwd PROTO ((char *pathname));
#endif

#ifdef missing_ioctl_extern
	extern int ioctl PROTO ((int fd, unsigned long request, ...));
#endif

#ifdef missing_listen_extern
	extern int listen PROTO ((int socket, int backlog));
#endif

#ifdef missing_memset_extern
	extern void *memset      PROTO ((
		void *ptr, int value, unsigned long numbytes
	)) ;
#endif

#ifdef missing_memcpy_extern
	extern void *memcpy      PROTO ((
		void *dest, const void *source, unsigned long numbytes
	)) ;
#endif

#ifdef missing_memmove_extern
	extern void *memmove      PROTO ((
		void *dest, const void *source, unsigned long numbytes
	)) ;
#endif

#ifdef missing_mktemp_extern
	extern char *		mktemp		PROTO ((char *));
#endif

#ifdef missing_nx_function_externs
	extern void NXChangeBuffer PROTO((void * /*  NXStream *   */)) ;
#endif

#ifdef missing_perror_extern
	extern int		perror		PROTO ((char *message));
#endif

#ifdef missing_printf_externs
#	include <stdio.h>

	extern int	printf	PROTO ((const char *format, ...));
	extern int	fprintf	PROTO ((FILE *wfp, const char *format, ...));
	extern int	sprintf	PROTO ((char *str, const char *format, ...));
	extern int	scanf	PROTO ((const char *format, ...));
	extern int	fscanf	PROTO ((FILE *rfp, const char *format, ...));
	extern int	sscanf	PROTO ((const char *str, const char *format, ...));
	extern int	fseek	PROTO ((FILE *fp, long offset, int whence));
	extern int	fread	PROTO ((void *buf, int item_size, int nitems, FILE *fp));
	extern int	fwrite	PROTO ((const void *buf, int item_size, int nitems, FILE *fp));
#endif

#ifdef missing_random_extern
	extern long random PROTO ((void));
#endif

#ifdef missing_recvmsg_extern
	#include <sys/types.h>
	#include <sys/socket.h>

	extern int		recvmsg				PROTO ((
						int				socket_fd,
						struct msghdr	msg[],
						int				flags
					));
#endif

#ifdef missing_rewind_extern
#	include <stdio.h>
	extern int	rewind	PROTO ((FILE *fp));
#endif

#ifdef missing_select_extern
	#include <sys/types.h>
	#include <sys/time.h>

	extern int		select					PROTO ((
						int					num_fds,
						fd_set *			rdfds,
						fd_set *			wrdfs,
						fd_set *			exfds,
						struct timeval *	wait_interval
					));
#endif

#ifdef missing_sendmsg_extern
	#include <sys/types.h>
	#include <sys/socket.h>

	extern int		sendmsg				PROTO ((
						int				socket_fd,
						struct msghdr	msg[],
						int				flags
					));
#endif

#ifdef missing_setpgrp_extern
	extern int setpgrp PROTO ((int pid, int pgrp));
#endif

#ifdef missing_setvbuf_extern
	extern int setvbuf PROTO ((FILE *stream, char *buf, int mode, size_t size));
#endif

#ifdef missing_shutdown_extern
	extern int			shutdown	PROTO ((int fd, int how));
#endif

#ifdef missing_socket_extern
	extern int socket PROTO ((int addr_family, int type, int protocol_family));
#endif

#ifdef missing_srandom_extern
	extern jr_int srandom PROTO ((long));
#endif

#ifdef missing_strdup_extern
	extern char *strdup      PROTO ((const char *str)) ;
#endif

#ifdef missing_strcasecmp_extern
	extern int strcasecmp      PROTO ((const char *str1, const char *str2));
#endif

#ifdef missing_strncasecmp_extern
	extern int strncasecmp      PROTO ((const char *str1, const char *str2, size_t n)) ;
#endif

#ifdef missing_sys_siglist_extern
	extern char *		sys_siglist[];
#endif

#ifdef missing_time_extern
	extern long time PROTO ((long *tloc));
#endif

#ifdef missing_truncate_extern
#	include <sys/types.h>

	extern int 			truncate PROTO ((char *pathname, off_t length));
#endif

#ifdef missing_ungetc_extern
#	include <stdio.h>
	extern int	ungetc	PROTO ((int c, FILE *fp));
#endif

#ifdef missing_utimes_extern
#	include <sys/time.h>

	extern int	utimes	PROTO ((char *file, struct timeval tvp[2]));
#endif


#ifdef missing_res_query_extern
		extern int		res_query					PROTO ((
							const char *			dname,
							int						class,
							int						type,
							unsigned char *			answer,
							int						anslen
						));
#endif

#ifdef missing_dn_skipname_extern
		extern int		dn_skipname					PROTO ((
							const unsigned char *	answer_ptr,
							const unsigned char *	answer_end_ptr
						));
#endif

#ifdef missing_backtrace_extern
	extern int	backtrace PROTO ((void **pc_array, int pc_array_size));
#endif

#ifdef missing_extra_regex_errors
#	define REG_EEND			-1
#	define REG_ESIZE		-2
#	define REG_ERPAREN		-3
#endif

#ifdef missing_inet_addr_extern
	extern unsigned long inet_addr (const char *);
#endif

#ifdef missing_inet_ntoa_extern
	extern char *inet_ntoa (unsigned long);
#endif

#ifdef missing_malloc_usable_size_extern
#	include <sys/types.h>
	extern size_t malloc_usable_size (void *ptr);
#endif

#ifdef missing_apr_is_enobufs_macro
#	define APR_STATUS_IS_ENOBUFS(status)			((status) == ENOBUFS)
#endif

#ifdef missing_stdbool_header
#	if !defined(__bool_true_false_are_defined) && !defined(__cplusplus)
#		define bool jr_int
#		define true 1
#		define false 0
#	endif
#endif

#ifdef missing_siglist_nsig
#	ifdef ostype_macosx
#		define NSIG __DARWIN_NSIG
#	endif
#endif

#endif
