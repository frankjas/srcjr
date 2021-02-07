#ifndef _jr_apr_h___
#define _jr_apr_h___

#include <stdarg.h>
#include <stddef.h>

#include "jr/long.h"
#include "jr/time.h"


/******** APR Support ********/

extern void				jr_apr_initialize (void);
extern void				jr_apr_terminate (void);
extern jr_int			jr_apr_is_initialized (void);
extern jr_int			jr_apr_initialize_count ();

extern const char *		jr_apr_strerror(
							jr_int						status,
							char *						error_buf);

/******** Permissions ********/

extern unsigned jr_int	jr_PermsToAprPerms (
							unsigned jr_int				perm_flags);

extern unsigned jr_int	jr_AprPermsToPerms (
							unsigned jr_int				apr_mode);

extern unsigned jr_int	jr_umask(
							unsigned jr_int				perm_flags);

extern unsigned jr_int	jr_umask_winnt(
							unsigned jr_int				perm_flags);
						/*
						** 2-25-2012: Under Unix use native umask. Under WinNt
						** use default umask of jr_PERM_RW_OWNER, and allow
						** programmatic change to this value. jr_umask_winnt()
						** is a no-op under Unix to allow for #ifdef-less
						** programming to change the default for WinNt.
						*/

#ifdef ostype_winnt

#define					jr_PermsToUmaskedAprPerms( perm_flags)					\
						jr_PermsToAprPerms( (perm_flags) & ~jr_WinNtUmask)

#define					jr_umask( perm_flags)									\
						jr_umask_winnt( perm_flags)

extern unsigned jr_int	jr_WinNtUmask;

#else
						/*
						** 2-25-2012: under Unix, umasking is builtin to file/dir creates.
						*/
#define					jr_PermsToUmaskedAprPerms( perm_flags)					\
						jr_PermsToAprPerms( perm_flags)

#endif

/******** stat ********/

typedef struct jr_stat {
	jr_ULong			file_size;
	jr_seconds_t		atime;
	jr_seconds_t		mtime;
	jr_seconds_t		ctime;

	unsigned jr_int		perms;

	unsigned			is_directory					: 1;
	unsigned			is_file							: 1;
	unsigned			is_link							: 1;	/* will only be set by jr_lstat() */
	unsigned			is_char_dev						: 1;
} jr_stat_t;

						/*
						** 2-24-2012: calling jr_stat_init() is unnecessary
						*/
extern void				jr_stat_init (
							jr_stat_t *					jr_stat_ptr);

extern jr_int			jr_stat_common (
							const char *				file_name,
							jr_stat_t *					opt_jr_stat_ptr,
							jr_int						flags,
							const char **				opt_user_name_ref,
							const char **				opt_group_name_ref,
							char *						error_buf);

#define					jr_stat_copy( dest_stat_ptr, src_stat_ptr)						\
						memcpy( dest_stat_ptr, src_stat_ptr, sizeof( jr_stat_t))

#define					jr_STAT_GET_LINK_INFO			0x0001
#define					jr_STAT_IGNORE_UID_ERRORS		0x0002

#define					jr_stat(file_name, opt_stat_ptr, error_buf)						\
						jr_stat_common (file_name, opt_stat_ptr, 0, 0, 0, error_buf)

#define					jr_lstat(file_name, opt_stat_ptr, error_buf)					\
						jr_stat_common (file_name, opt_stat_ptr, jr_STAT_GET_LINK_INFO, 0, 0, error_buf)

#define					jr_stat_with_user_name(											\
							file_name, opt_stat_ptr,									\
							opt_user_name_ref, opt_group_name_ref, error_buf			\
						)																\
						jr_stat_common (												\
							file_name, opt_stat_ptr, 0,									\
							opt_user_name_ref, opt_group_name_ref, error_buf			\
						)

#define					jr_lstat_with_user_name(										\
							file_name, opt_stat_ptr,									\
							opt_user_name_ref, opt_group_name_ref, error_buf			\
						)																\
						jr_stat_common (												\
							file_name, opt_stat_ptr, jr_STAT_GET_LINK_INFO,				\
							opt_user_name_ref, opt_group_name_ref, error_buf			\
						)

#define					jr_stat_with_user_name_if_available(							\
							file_name, opt_stat_ptr,									\
							opt_user_name_ref, opt_group_name_ref, error_buf			\
						)																\
						jr_stat_common (												\
							file_name, opt_stat_ptr, jr_STAT_IGNORE_UID_ERRORS,			\
							opt_user_name_ref, opt_group_name_ref, error_buf			\
						)

#define					jr_lstat_with_user_name_if_available(							\
							file_name, opt_stat_ptr,									\
							opt_user_name_ref, opt_group_name_ref, error_buf			\
						)																\
						jr_stat_common (												\
							file_name, opt_stat_ptr,									\
							jr_STAT_GET_LINK_INFO | jr_STAT_IGNORE_UID_ERRORS,			\
							opt_user_name_ref, opt_group_name_ref, error_buf			\
						)

extern jr_int			jr_stat_mtime_cmp (
							const void *				void_ptr_1,
							const void *				void_ptr_2);

#define					jr_stat_set_mtime(stat_ptr, v)	\
						((stat_ptr)->mtime = (v))

#define					jr_stat_file_lsize(stat_ptr)	\
						((stat_ptr)->file_size)

#define					jr_stat_file_size(stat_ptr)		\
						jr_ULongToUInt((stat_ptr)->file_size)

#define					jr_stat_atime(stat_ptr)			\
						((stat_ptr)->atime)

#define					jr_stat_mtime(stat_ptr)			\
						((stat_ptr)->mtime)

#define					jr_stat_ctime(stat_ptr)			\
						((stat_ptr)->ctime)

#define					jr_stat_perms(stat_ptr)			\
						((stat_ptr)->perms)

#define					jr_stat_is_file(stat_ptr)		\
						((stat_ptr)->is_file)

#define					jr_stat_is_directory(stat_ptr)	\
						((stat_ptr)->is_directory)

#define					jr_stat_is_link(stat_ptr)		\
						((stat_ptr)->is_link)

#define					jr_stat_is_char_dev(stat_ptr)	\
						((stat_ptr)->is_char_dev)

/******** Users and Groups ********/

/*
** 4/27/08: Use structs to prevent ordinary int comparisons, which
** won't work under NT.
*/

#ifdef ostype_winnt
	typedef struct {
		void *				id_value;
		void *				apr_pool_ptr;
	} jr_uid_t;

	typedef struct {
		void *				id_value;
		void *				apr_pool_ptr;
	} jr_gid_t;

#else
	typedef struct {
		jr_int				id_value;
		void *				apr_pool_ptr;
	} jr_uid_t;

	typedef struct {
		jr_int				id_value;
		void *				apr_pool_ptr;
	} jr_gid_t;
#endif

extern void				jr_uid_init (
							jr_uid_t *					uid_ptr);

extern void				jr_uid_undo (
							jr_uid_t *					uid_ptr);

extern void				jr_gid_init (
							jr_gid_t *					gid_ptr);

extern void				jr_gid_undo (
							jr_gid_t *					gid_ptr);


extern jr_int			jr_uid_cmp (
							const void *				void_ptr_1,
							const void *				void_ptr_2);

extern jr_int			jr_gid_cmp (
							const void *				void_ptr_1,
							const void *				void_ptr_2);

extern jr_int			jr_uid_init_current (
							jr_uid_t *					uid_ptr,
							char *						error_buf);

extern jr_int			jr_gid_init_current (
							jr_gid_t *					gid_ptr,
							char *						error_buf);

extern jr_int			jr_uid_init_from_name (
							jr_uid_t *					uid_ptr,
							const char *				user_name,
							char *						error_buf);

extern jr_int			jr_gid_init_from_name (
							jr_gid_t *					gid_ptr,
							const char *				group_name,
							char *						error_buf);

extern jr_int			jr_uid_get_name (
							jr_uid_t *					uid_ptr,
							const char **				user_name_ref,
							char *						error_buf);

extern jr_int			jr_gid_get_name (
							jr_gid_t *					gid_ptr,
							const char **				group_name_ref,
							char *						error_buf);

/******** Users and Groups ********/
extern jr_int			jr_password_check(
							const char *				password_str,
							const char *				hashed_str);

extern void				jr_password_hash( 
							const char *				password_str,
							const char *				salt_str,
							char *						hash_buf,
							jr_int						hash_buf_len);

#define					jr_PASSWORD_HASH_LEN			128

/******** IO Buffering For printf-like support ********/

typedef struct jr_aprbuf	jr_aprbuf_t;

typedef int	(*jr_aprbuf_writefn_t)( void *io_info, void *buf, int buf_len, char *error_buf );

extern void				jr_aprbuf_init (
							jr_aprbuf_t *				aprbuf_ptr,
							void *						io_info,
							int							(*write_fn)(
															void *		io_info,
															void *		buf,
															int			buf_len,
															char *		error_buf),
							jr_int						buf_size);

extern void				jr_aprbuf_undo (
							jr_aprbuf_t *				aprbuf_ptr);

extern jr_aprbuf_t *	jr_aprbuf_create (
							void *						io_info,
							int							(*write_fn)(
															void *		io_info,
															void *		buf,
															int			buf_len,
															char *		error_buf),
							jr_int						buf_size);

extern jr_int			jr_aprbuf_vformatter(
							jr_aprbuf_t *				aprbuf_ptr,
							const char *				control_string,
							va_list						arg_list);

extern void				jr_aprbuf_destroy (
							jr_aprbuf_t *				aprbuf_ptr);


extern void				jr_aprbuf_set_error_buf(
							jr_aprbuf_t *				aprbuf_ptr,
							char *						error_buf);

extern int				jr_aprbuf_status(
							jr_aprbuf_t *				aprbuf_ptr);

extern int				jr_aprbuf_needs_flush(
							jr_aprbuf_t *				aprbuf_ptr);

extern jr_int			jr_aprbuf_flush (
							jr_aprbuf_t *				aprbuf_ptr);


/******** directories ********/

typedef struct {
	void *				apr_pool_ptr;
	void *				apr_dir_ptr;
} jr_dir_t;

extern void				jr_dir_init (
							jr_dir_t *					dir_info);

extern void				jr_dir_undo (
							jr_dir_t *					dir_info);

extern jr_int			jr_dir_open (
							jr_dir_t *					dir_info,
							const char *				path,
							char *						error_buf);

extern void				jr_dir_close (
							jr_dir_t *					dir_info);

extern jr_int			jr_dir_read (
							jr_dir_t *					dir_info,
							const char **				opt_entry_name_ref,
							jr_stat_t *					opt_stat_info,
							char *						error_buf);


/******** Files ********/

typedef struct jr_file {
	void *				apr_file_ptr;
	void *				apr_pool_ptr;

	jr_aprbuf_t *		aprbuf_ptr;
} jr_file_t;

#define					jr_FILE_OPEN_READ				0x0001
#define					jr_FILE_OPEN_WRITE				0x0002
#define					jr_FILE_OPEN_APPEND				0x0004
#define					jr_FILE_OPEN_CREATE				0x0008
#define					jr_FILE_OPEN_TRUNCATE			0x0010
#define					jr_FILE_OPEN_BUFFERED			0x0020
#define					jr_FILE_OPEN_DEL_ON_CLOSE		0x0040
#define					jr_FILE_OPEN_EXCLUSIVE			0x0080
						/*
						** 4/17/08: Buffered I/O works like FILE under Unix.
						** For MS, the gets() functionality doesn't take advantage.
						*/

#define					jr_FILE_OPEN_RDWR				(jr_FILE_OPEN_READ | jr_FILE_OPEN_WRITE)

#define					jr_FILE_OPEN_WRITING			\
						(jr_FILE_OPEN_WRITE | jr_FILE_OPEN_CREATE | jr_FILE_OPEN_TRUNCATE)

#define					jr_FILE_OPEN_APPENDING			\
						(jr_FILE_OPEN_WRITE | jr_FILE_OPEN_CREATE )

extern void				jr_file_init (
							jr_file_t *					file_info);

extern void				jr_file_undo (
							jr_file_t *					file_info);

extern jr_int			jr_file_open (
							jr_file_t *					file_info,
							const char *				file_name,
							jr_int						open_flags,
							jr_int						file_perms,
							char *						error_buf);
							/*
							** 5/31/08: can reuse an already open jr_file_t,
							** but is better to close first, since otherwise
							** memory allocations keep accumulating.
							*/

extern jr_int			jr_file_close (
							jr_file_t *					file_info,
							char *						error_buf);

extern jr_int			jr_file_open_stdin (
							jr_file_t *					file_info,
							char *						error_buf);

extern jr_int			jr_file_open_stdout (
							jr_file_t *					file_info,
							char *						error_buf);

extern jr_int			jr_file_open_stderr (
							jr_file_t *					file_info,
							char *						error_buf);

extern jr_int			jr_file_open_pipe (
							jr_file_t *					read_file_info,
							jr_file_t *					write_file_info,
							char *						error_buf);
							/*
							** 5/8/08: The read file should be undone after the write.
							*/
extern jr_int			jr_file_open_tmp (
							jr_file_t *					file_info,
							char *						file_name,
							jr_int						open_flags,
							jr_int						file_perms,
							char *						error_buf);

extern int				jr_file_setbufsize( 
							jr_file_t *					file_info,
							jr_int						buf_size,
							char *						error_buf);

extern jr_int			jr_file_read (
							jr_file_t *					file_info,
							void *						buffer,
							unsigned jr_int				data_length,
							char *						error_buf);

extern jr_int			jr_file_write (
							jr_file_t *					file_info,
							const void *				data_buf,
							unsigned jr_int				data_length,
							char *						error_buf);

extern jr_int			jr_file_printf (
							jr_file_t *					file_info,
							const char *				control_string,
														...);

extern jr_int			jr_file_flush (
							jr_file_t *					file_info,
							char *						error_buf);

extern jr_int			jr_file_sync (
							jr_file_t *					file_info,
							char *						error_buf);

#define					jr_FILE_SEEK_SET				0
#define					jr_FILE_SEEK_CUR				1
#define					jr_FILE_SEEK_END				2

extern jr_int			jr_file_seek (
							jr_file_t *					file_info,
							jr_ULong					offset,
							jr_int						whence,
							char *						error_buf);

extern jr_int			jr_file_get_offset (
							jr_file_t *					file_info,
							jr_ULong *					offset_ref,
							char *						error_buf);

extern jr_int			jr_file_truncate (
							jr_file_t *					file_info,
							jr_ULong					offset,
							char *						error_buf);

extern jr_int			jr_file_gets (
							jr_file_t *					file_info,
							char *						buffer,
							jr_int						buffer_length,
							char *						error_buf);

extern jr_int			jr_file_copy(
							jr_file_t *					dest_file_info,
							jr_file_t *					src_file_info,
							char *						error_buf);

extern jr_int			jr_file_stat_common (
							jr_file_t *					file_info,
							jr_int						flags,
							jr_stat_t *					opt_jr_stat_ptr,
							const char **				opt_user_name_ref,
							const char **				opt_group_name_ref,
							char *						error_buf);

#define					jr_file_stat(file_info, opt_stat_ptr, error_buf)					\
						jr_file_stat_common (file_info, 0, opt_stat_ptr, 0, 0, error_buf)

#define					jr_file_stat_with_user_name(										\
							file_info, opt_stat_ptr,										\
							opt_user_name_ref, opt_group_name_ref, error_buf				\
						)																	\
						jr_file_stat_common (												\
							file_info, 0, opt_stat_ptr,										\
							opt_user_name_ref, opt_group_name_ref, error_buf				\
						)

#define					jr_file_stat_with_user_name_if_available(							\
							file_info, opt_stat_ptr,										\
							opt_user_name_ref, opt_group_name_ref, error_buf				\
						)																	\
						jr_file_stat_common (												\
							file_info, jr_STAT_IGNORE_UID_ERRORS, opt_stat_ptr,				\
							opt_user_name_ref, opt_group_name_ref, error_buf				\
						)

extern jr_int			jr_file_is_tape(
							jr_file_t *					file_info,
							char *						error_buf);

#define					jr_FILE_LOCK_READ				1
#define					jr_FILE_LOCK_WRITE				2
						/*
						** 4/17/08: for write locks, file must be open for writing.
						*/

extern jr_int			jr_file_lock (
							jr_file_t *					file_info,
							jr_int						lock_type,
							jr_int						time_out,
							char *						error_buf);

extern jr_int			jr_file_unlock (
							jr_file_t *					file_info,
							char *						error_buf);

#define					jr_file_lock_read(file_info, time_out, error_buf)					\
						jr_file_lock (file_info, jr_FILE_LOCK_READ, time_out, error_buf)

#define					jr_file_lock_write(file_info, time_out, error_buf)					\
						jr_file_lock (file_info, jr_FILE_LOCK_WRITE, time_out, error_buf)

extern jr_int			jr_file_cmp (
							const void *				void_ptr_1,
							const void *				void_ptr_2);

extern jr_int			jr_file_hash (
							const void *				void_ptr);

#define					jr_file_apr_ptr(file_info)				\
						((file_info)->apr_file_ptr)

#define					jr_file_set_apr_ptr(file_info, v)		\
						((file_info)->apr_file_ptr = (v))


/******** Socket Operations *********/

#define					jr_SOCK_STREAM					1
#define					jr_SOCK_DGRAM					2

#define					jr_SOCKBUF_DEFAULT_SIZE			1024
						/*
						** 2/7/07: standard network packet/data size
						*/

typedef struct jr_socket {
	void *				apr_socket_ptr;
	void *				apr_pool_ptr;
	jr_int				socket_type;

	jr_aprbuf_t *		aprbuf_ptr;

	unsigned			is_nonblocking					: 1;
} jr_socket_t;

typedef struct jr_sockaddr {
	void *				apr_sockaddr_ptr;
	void *				apr_pool_ptr;
	const char *		ip_str;

	struct jr_sockaddr *next_sockaddr_ptr;

	unsigned			has_borrowed_pool				: 1;
} jr_sockaddr_t;


extern void				jr_sockaddr_init (
							jr_sockaddr_t *				sockaddr_info);

extern void				jr_sockaddr_undo (
							jr_sockaddr_t *				sockaddr_info);

extern jr_int			jr_sockaddr_lookup (
							jr_sockaddr_t *				sockaddr_info,
							const char *				host_name,
							jr_int						port_number,
							char *						error_buf);

extern jr_sockaddr_t *	jr_sockaddr_init_foreach (
							jr_sockaddr_t *		sockaddr_info);

						/*
						** 2-18-2013: fails if no pool can be allocated.
						*/
extern jr_int			jr_sockaddr_dup (
							const jr_sockaddr_t *		src_sockaddr_info,
							jr_sockaddr_t *				dest_sockaddr_info,
							char *						error_buf);

extern jr_int			jr_sockaddr_cmp (
							const jr_sockaddr_t *		sockaddr_info_1,
							const jr_sockaddr_t *		sockaddr_info_2);

extern void *			jr_sockaddr_ipaddr_ptr (
							const jr_sockaddr_t *		sockaddr_info);

extern jr_int			jr_sockaddr_ipaddr_len (
							const jr_sockaddr_t *		sockaddr_info);

extern jr_int			jr_sockaddr_port (
							const jr_sockaddr_t *		sockaddr_info);

extern jr_int			jr_sockaddr_network_number(
							const jr_sockaddr_t *		sockaddr_info);

extern jr_int			jr_sockaddr_host_number(
							const jr_sockaddr_t *		sockaddr_info);

extern jr_int			jr_sockaddr_ip_cmp (
							const jr_sockaddr_t *		sockaddr_info_1,
							const jr_sockaddr_t *		sockaddr_info_2);

extern const char *		jr_sockaddr_get_ip_str (
							jr_sockaddr_t *				sockaddr_info);

#define					jr_sockaddr_ip_str(sockaddr_info)				\
						((const char *) (sockaddr_info)->ip_str			\
							? (sockaddr_info)->ip_str					\
							: jr_sockaddr_get_ip_str( (jr_sockaddr_t*) sockaddr_info)	\
						)
						/*
						** 2-18-2013: strip "const" above, eventhough the ip_str
						** value is stored. Only non-const on the first call,
						** is that good enough?
						*/
						
#define					jr_sockaddr_next_addr_ptr( sockaddr_info)								\
						((sockaddr_info)->next_sockaddr_ptr)

#define					jr_sockaddr_foreach_addr_ptr(	sockaddr_info, sockaddr_ptr)			\
						for (	(sockaddr_ptr) = jr_sockaddr_init_foreach( sockaddr_info);		\
								(sockaddr_ptr) != 0;											\
								(sockaddr_ptr) = jr_sockaddr_next_addr_ptr( sockaddr_ptr))

extern void				jr_socket_init (
							jr_socket_t *				socket_info,
							jr_int						socket_type);

extern void				jr_socket_undo (
							jr_socket_t *				socket_info);

extern jr_socket_t *	jr_socket_create (
							jr_int						socket_type);

extern void				jr_socket_destroy (
							jr_socket_t *				socket_info);

extern jr_int			jr_socket_is_open(
							jr_socket_t *				socket_info);

extern jr_int			jr_socket_open_generic (
							jr_socket_t *				socket_info,
							jr_int						opt_os_sock,
							char *						error_buf);

#ifdef ostype_winnt
#define					jr_INVALID_OS_SOCKET			INVALID_SOCKET
#else
#define					jr_INVALID_OS_SOCKET			-1
#endif

#define					jr_socket_open( socket_info, error_buf)			\
						jr_socket_open_generic( socket_info, jr_INVALID_OS_SOCKET, error_buf)

#define					jr_socket_open_from_fd( socket_info, socket_fd, error_buf)			\
						jr_socket_open_generic( socket_info, socket_fd, error_buf)

extern jr_int			jr_socket_close (
							jr_socket_t *				socket_info,
							char *						error_buf);

extern jr_int			jr_socket_connect (
							jr_socket_t *				socket_info,
							const char *				host_name,
							jr_int						port_number,
							char *						error_buf);

extern jr_int			jr_socket_open_dgram (
							jr_socket_t *				socket_info,
							const char *				local_host_name,
							jr_int						port_number,
							char *						error_buf);

extern jr_int			jr_socket_connect_unix (
							jr_socket_t *				socket_info,
							const char *				socket_path,
							char *						error_buf);

extern jr_int			jr_socket_offer_service (
							jr_socket_t *				socket_info,
							const char *				host_name,
							jr_int						port_number,
							char *						error_buf);

extern jr_int			jr_socket_accept (
							jr_socket_t *				accept_socket_info,
							jr_socket_t *				client_socket_info,
							char *						error_buf);

extern jr_int			jr_socket_shutdown (
							jr_socket_t *				socket_info,
							char *						error_buf);

extern jr_int			jr_socket_set_nonblock (
							jr_socket_t *				socket_info,
							jr_int						value,
							char *						error_buf);

extern jr_int			jr_socket_port(
							jr_socket_t *				socket_info,
							char *						error_buf);

extern jr_int			jr_socket_recv (
							jr_socket_t *				socket_info,
							void *						data_buf,
							jr_int						data_buf_len,
							char *						error_buf);

extern jr_int			jr_socket_send (
							jr_socket_t *				socket_info,
							const void *				data_buf,
							jr_int						data_buf_len,
							char *						error_buf);

struct iovec;

extern jr_int			jr_socket_sendto_iov (
							jr_socket_t *				socket_info,
							jr_sockaddr_t *				sockaddr_info,
							struct iovec *				iov,
							jr_int						iovlen,
							char *						error_buf);

extern jr_int			jr_socket_recvfrom (
							jr_socket_t *				socket_info,
							jr_sockaddr_t *				sockaddr_info,
							void *						data_buf,
							jr_int						data_buf_len,
							char *						error_buf);
					
						/*
						** 11-30-2011: need to call jr_socket_flush() after jr_socket_printf()s
						*/
extern jr_int			jr_socket_printf (
							jr_socket_t *				socket_info,
							const char *				control_string, ...);

extern jr_int			jr_socket_flush (
							jr_socket_t *				socket_info,
							char *						error_buf);

extern jr_int			jr_socket_cmp (
							const void *				void_ptr_1,
							const void *				void_ptr_2);

extern jr_int			jr_socket_hash (
							const void *				void_ptr);

#define					jr_socket_apr_ptr(socket_info)	\
						((socket_info)->apr_socket_ptr)

/******** Process Spawning ********/

typedef struct jr_process {
	void *		apr_pool_ptr;
	void *		apr_proc_ptr;
	void *		apr_procattr_ptr;

	jr_file_t	input_wfi[1];
	jr_file_t	output_rfi[1];
	jr_file_t	error_rfi[1];

	unsigned	do_debug				: 1;
} jr_process_t;

						/*
						** 4/19/09: NOTE: Undo()ing a running process will first send SIGTERM,
						** wait 3 seconds, then send SIGKILL, then wait()
						*/

#define					jr_PROCESS_EXITTED				0x0001
#define					jr_PROCESS_WAS_SIGNALLED		0x0002
#define					jr_PROCESS_HAS_CORE				0x0004

#define					jr_PROCESS_WAIT_BLOCK			0x0000
#define					jr_PROCESS_WAIT_NON_BLOCK		0x0010

#define					jr_PROCESS_NO_PIPE				0
#define					jr_PROCESS_BLOCKING_PIPE		1
#define					jr_PROCESS_NONBLOCKING_PIPE		2

extern void				jr_process_init (
							jr_process_t *				process_ptr);

extern void				jr_process_undo (
							jr_process_t *				process_ptr);

						/*
						** 5/4/08: Set the child's stdin to come from child_rfi.
						** If that is the read side of a pipe, pass the write side in as well,
						** otherwise pass a null
						*/
extern jr_int			jr_process_set_input (
							jr_process_t *				process_ptr,
							jr_file_t *					child_rfi,
							jr_file_t *					opt_parent_pipe_wfi,
							char *						error_buf);

						/*
						** 5/4/08: Set the child's stdout to go to child_wfi.
						** If that is the write side of a pipe, pass the read side in as well,
						** otherwise pass a null.
						** Note: the wfi will be dup()ed, so if it's the write end of a pipe,
						** the value passed in will need to closed.
						*/
extern jr_int			jr_process_set_output (
							jr_process_t *				process_ptr,
							jr_file_t *					child_wfi,
							jr_file_t *					opt_parent_pipe_rfi,
							char *						error_buf);

extern jr_int			jr_process_set_error (
							jr_process_t *				process_ptr,
							jr_file_t *					child_wfi,
							jr_file_t *					opt_parent_pipe_rfi,
							char *						error_buf);

						/*
						** 4/30/09: each call to make_pipes() overwrites the previous
						** pipes. So if you make an input pipe then an output pipe,
						** the input pipe will be erase.
						*/
extern jr_int			jr_process_make_pipes(
							jr_process_t *				process_ptr,
							jr_int						input_kind,
							jr_int						output_kind,
							jr_int						error_kind,
							char *						error_buf);

#define					jr_process_make_input_pipe( process_ptr, pipe_kind, error_buf)			\
						jr_process_make_pipes(													\
							process_ptr, pipe_kind, jr_PROCESS_NO_PIPE, jr_PROCESS_NO_PIPE,		\
							error_buf															\
						)

#define					jr_process_make_output_pipe( process_ptr, pipe_kind, error_buf)			\
						jr_process_make_pipes(													\
							process_ptr, jr_PROCESS_NO_PIPE, pipe_kind, jr_PROCESS_NO_PIPE,		\
							error_buf															\
						)

extern jr_int			jr_process_spawn (
							jr_process_t *				process_ptr,
							const char *				prog_name,
							const char **				opt_argv,
							const char **				opt_envp,
							char *						error_buf);

extern jr_int			jr_process_wait (
							jr_process_t *				process_ptr,
							jr_int						flags,
							jr_int *					exit_status_ref,
							jr_int *					exit_why_ref,
							char *						error_buf);

extern jr_int			jr_process_kill (
							jr_process_t *				process_ptr,
							jr_int						signum_status,
							char *						error_buf);
							/*
							** 11-29-2011: in MS the signal is used for the exit status?
							*/

extern jr_int			jr_process_id (
							jr_process_t *				process_ptr);

#define					jr_process_set_do_debug(process_ptr, v)		\
						((process_ptr)->do_debug = (v) != 0)

#define					jr_process_input_wfi(process_ptr)			\
						((process_ptr)->input_wfi)

#define					jr_process_output_rfi(process_ptr)			\
						((process_ptr)->output_rfi)

#define					jr_process_error_rfi(process_ptr)			\
						((process_ptr)->error_rfi)


/******** Threads/Mutexes ********/

#if defined(ostype_macosx)
#ifdef jr_IS_64_BIT
#	define				jr_THREAD_OSX_ONCE_SIZE		8
#else
#	define				jr_THREAD_OSX_ONCE_SIZE		4
#endif
#endif

typedef struct jr_thread_once {
#	ifdef ostype_winnt
		volatile long			control_value;
#	elif defined(ostype_cygwin)
		struct {
			void *				mutex;
			int					state;
		} control_value;
#	elif defined(ostype_macosx)
		struct {
			long				__sig;
			char				__opaque[jr_THREAD_OSX_ONCE_SIZE];
		} control_value;
#	else
		int						control_value;
#	endif
} jr_thread_once_t;


#	define				jr_thread_once_declare_and_init(var_name)					\
						static jr_thread_once_t var_name	= jr_PTHREAD_ONCE_INIT

extern void				jr_thread_once_call (
							jr_thread_once_t *			once_control_ptr,
							void						(*some_func)());


/******** Mutexes ********/

/*
** 3/2/07: Can't use APR for mutexes because:
** - library functions need to initialize mutex
** -- APR mutex allocates memory
** -- so need a mutex to protect initialization
** - also, jr_apr_terminate() would free library mutexes
** -- need to re-initialize
** -- need a global datastructure of thread_once structures to reset
** -- need a mutex to keep track of this.
*/

#ifdef jr_IS_64_BIT
#	define				jr_SIZEOF_PTHREAD_MUTEX_T		64
#	define				jr_SIZEOF_MICROSOFT_MUTEX_T		64
#else
#	define				jr_SIZEOF_PTHREAD_MUTEX_T		32
#	define				jr_SIZEOF_MICROSOFT_MUTEX_T		32
#endif

typedef struct jr_mutex {
	union {
		char			pthread_data[jr_SIZEOF_PTHREAD_MUTEX_T];
		char			microsoft_data[jr_SIZEOF_MICROSOFT_MUTEX_T];
	} arch_data;

	unsigned			is_recursive					: 1;
} jr_mutex_t;

extern void				jr_mutex_init (
							jr_mutex_t *				mutex_ptr,
							jr_int						is_recursive);

extern void				jr_mutex_undo					(
							jr_mutex_t *				mutex_ptr);

extern void				jr_mutex_lock					(
							jr_mutex_t *				mutex_ptr);
						
extern void				jr_mutex_unlock					(
							jr_mutex_t *				mutex_ptr);


typedef struct jr_thread {
	void *				apr_pool_ptr;
	void *				apr_thread_ptr;
	void				(*thread_fn)(struct jr_thread *, void * arg_ptr);
	void *				arg_ptr;
} jr_thread_t;

extern void				jr_thread_init (
							jr_thread_t *				thread_ptr);

extern void				jr_thread_undo (
							jr_thread_t *				mutex_ptr);

extern jr_int			jr_thread_spawn (
							jr_thread_t *				thread_ptr,
							void						(*thread_fn)(
															jr_thread_t *	thread_ptr,
															void *			arg_ptr
														),
							void *						arg_ptr,
							char *						error_buf);

extern jr_int			jr_thread_detach (
							jr_thread_t *				thread_ptr,
							char *						error_buf);

extern void				jr_thread_exit( 
							jr_thread_t *				thread_ptr,
							jr_int						exit_status);

extern jr_int			jr_thread_wait( 
							jr_thread_t *				thread_ptr,
							jr_int *					exit_status_ref,
							char *						error_buf);

typedef struct jr_pool {
	void *				apr_pool_ptr;
	struct jr_pool *	parent_pool_ptr;
} jr_pool_t;

extern void				jr_pool_init (
							jr_pool_t *					pool_ptr,
							jr_pool_t *					opt_parent_pool_ptr);

extern void				jr_pool_undo (
							jr_pool_t *					pool_ptr);

extern jr_pool_t *		jr_pool_create(
							jr_pool_t *					opt_parent_pool_ptr);

extern void				jr_pool_destroy (
							jr_pool_t *					pool_ptr);

extern void *			jr_pool_alloc(
							jr_pool_t *					pool_ptr,
							size_t						mem_size);

extern char *			jr_pool_strdup( 
							jr_pool_t *					pool_ptr,
							const char *				src_str);

#define					jr_pool_apr_pool_ptr( pool_ptr)	\
						((pool_ptr)->apr_pool_ptr)
#endif
