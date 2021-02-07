#ifndef __EZPORT_HEADER__
#define __EZPORT_HEADER__

/*
 * PROBLEM IDENTIFICATION SECTION
 * For every platform to which you intend to port your C code
 * Provide a group of customizing #defines surrounded by #ifdef
 */

#ifdef vendor_sun
#	ifdef ostype_sunos
#		if __version__ == 5400
#			define replace_sun_siglist_names
#			define missing_gethostname_extern
#			define missing_bzero
#			define missing_utmp_ut_host_field
#		endif

#		if __version__ <= 5400
#			define needs_POSIX_fixes

#			define has_streams_async_io
#			define missing_wifcore_macro
#		endif

#		if (__version__ <= 4130)
#			define has_sys_5_sem_shm
#			define has_netinet
#			define has_timelocal
#			define missing_seek_constants
#			define missing_errno_externs
#			define missing_ssize_type
#			define missing_regex_h

#			define missing_strerror
#			define missing_sys_siglist_extern
#		endif

#		if __version__ <= 3120
#			define replace_buggy_inet_ntoa
#		endif
#	endif
#endif


#ifdef vendor_borland
#	if (__version__ <= 3100)
#		define missing_strdup_extern
#		define missing_unix_sys_types
#		define missing_passwd_funcs
#		define missing_passwd_types
#		define missing_getuid
#		define missing_off_t
#	endif

#	include "jr/ezport/borland.h"
	/*
	 * use a separate include file since
	 * borland uses a #pragma not supported by
	 * non-ANSI C and will cause the pre-processor
	 * to fail on other platforms
	 */
#	ifdef ostype_win31
#		define missing_argv
#	endif
#endif

#ifdef ostype_winnt

#ifdef __MSC_check_prog
#include <stdio.h>

#	define __version__ (10 * _MSC_VER - 6000)

void main()
{
	printf( "==== _MSC_VER: %d\n", _MSC_VER);
	printf( "==== __version__: %d\n", __version__);
}

#endif

#	if (_MSC_VER <= 1400)
#		define missing_tar_h
#		define missing_crypt_extern
		/*
		** 1/25/07: accessing htonl() etc requires including windows header
		** files in "jr" headers which cause other compilation problems:
		** - can't 'extern "C"' jr headers (windows headers have templates)
		*/

#		define has_broken_sigio_on_pipes
#		define missing_regex
#		define needs_winnt_vsnprintf

#	endif
#	if (_MSC_VER <= 1500)
#		define missing_addr_types
#		define missing_getrusage
#		define missing_roundf
#		define missing_xdr_float
#		define missing_xdrmem_create

#		define missing_sigaction_types
		/*
		** 5-17-2011: temporary define to allow inclusion of tcpd.h
		** See also io/http_lib.h and the #define of _POSIX_SOURCE
		*/
#	endif

#	if (_MSC_VER <= 1600)
		#define missing_stdbool_header
#	endif

#	if (_MSC_VER <= 1800)
#		define needs_winnt_snprintf
#	endif

#	define missing_execinfo_h
#	define missing_file_descriptors
#	define missing_mode_t
#	define missing_strcasecmp
#	define missing_wifcore_macro

#	define missing_sys_siglist_extern
#	define missing_sys_siglist_objects


#	define reduce_winnt_compile
#	define needs_winnt_defines_for_APR
#	define needs_POSIX_fixes
#	define missing_addr_types
#	define missing_struct_iov
#	define missing_rpc_headers
#	define missing_strptime
#	define missing_getrusage

#	define replace_byte_order_funcs

#	include "jr/ezport/microsoft.h"

#endif

#ifdef vendor_sony
#	if (__version__ <= 4200)

#		define needs_POSIX_fixes
#		define has_netinet

#		define missing_regex_h
#		define missing_tar_h

#		define has_buggy_sscanf_hex
#		define missing_memmove

#		ifdef ostype_newsos_gnu
#			define __USE_FIXED_PROTOTYPES__ 1
#			include <string.h>
			/*
			 * an extern for strerror is builtin to GNU header files under C++
			 * or use-fixed-prototypes
			 * which conflicts with the fix triggered below
			 */
#			define missing_mktemp_extern
#		endif

#		ifdef ostype_newos
#			define missing_seek_constants

#			define missing_memcpy_extern
#			define missing_memset_extern

#			define missing_printf_externs
#			define missing_fclose_extern
#			define missing__flsbuf_extern
#			define missing__filbuf_extern
#			define missing_ungetc_extern
#			define missing_fflush_extern
#			define missing_perror_extern

#			define missing_time_extern
#		endif

#		define missing_wifcore_macro

#		define missing_setvbuf_objects

#		define missing_strdup
#		define missing_strerror
#		define missing_strncasecmp_extern
#		define missing_strstr

#		define missing_bind_extern
#		define missing_byte_order_externs
#		define missing_bzero_extern
#		define missing_connect_extern
#		define missing_crypt_extern
#		define missing_flock_extern
#		define missing_fsync_extern
#		define missing_ftruncate_extern
#		define missing_gethostname_extern
#		define missing_getsockname_extern
#		define missing_ioctl_extern
#		define missing_listen_extern
#		define missing_random_extern
#		define missing_recvmsg_extern
#		define missing_select_extern
#		define missing_sendmsg_extern
#		define missing_shutdown_extern
#		define missing_socket_extern
#		define missing_srandom_extern
#		define missing_sys_siglist_extern


#		define has_machine_endian_h
		/*
		 * used to define byte order externs
		 */

#		define has_language_c_define
		/*
		 * refers to default #defines made by the
		 * compiler and used in header files
		 */

#	endif

#endif

#ifdef vendor_sgi
#	ifdef ostype_irix
#		if __version__ == 6200
#			define missing_sys_siglist_extern
#			define missing_wifcore_macro
#			define missing_mktemp_extern
#			define needs_POSIX_fixes
#		endif
#	endif
#endif

#ifdef vendor_isi
#	ifdef ostype_bsd
#		define has_netinet
#		define missing_seek_constants
#		define missing_errno_externs
#		define needs_POSIX_fixes
#		define missing_strchr
#		define missing_strrchr
#		define missing_strstr
#		define missing_strdup
#	endif
#endif

#ifdef ostype_linux
	/*
	 * The vendor will be RedHat, Slackware, etc.
	 * Use the kernel version number, not the vendor's
	 * version number
	 *
	 * wifcore goes here because it is not POSIX,
	 * neither is mktemp
	 */
#	define has_gnu_system_include_directory
#	define has_gnu_extensions_in_standard_includes

#	define has_broken_sigio_on_pipes

#	define sendmsg_returns_conn_refused
#	define has_malloc_size
#	define missing_malloc_usable_size_extern

#	if (__version__ <= 1159)
#		define missing_sgtty
#	endif
#	if (__version__ <= 1213)
#		define missing_sendmsg
#		define missing_recvmsg
#		define missing_struct_msghdr

#		define missing_crypt_extern
#		define missing_fsync_extern
#		define missing_ftruncate_extern
#		define missing_gethostname_extern
#		define missing_mktemp_extern
#		define missing_random_extern
#		define missing_wifcore_macro
#	endif

#	if (__version__ >= 2000)
#		define _GNU_SOURCE	1

#		define missing_wifcore_macro
#		define missing_msghdr_accrights_fields
#	endif

#	ifdef _POSIX_SOURCE
#		ifndef __cplusplus
#			define missing_strcasestr
#		endif
#	endif


#	ifdef vendor_linuxppc
#		if (__version__ == 2210) 
#			define has_glibc_v2
#			define replace_buggy_getenv
#		endif

#		define has_va_list_with_struct
#	endif

#	if (__version__ <= 2600)
#		define missing_epoll_in_kernel
#	endif
#endif


#ifdef ostype_cygwin
#	if (__version__ == 4000)
#		define needs_POSIX_fixes

#		define missing_tar_h
#		define missing_wifcore_macro

#		define missing_select_extern
#		define missing_crypt_extern
#		define missing_flock_extern
#		define missing_flock_objects
#		define missing_sys_siglist_extern
#		define missing_sys_siglist_objects
#	endif
#	if (__version__ <= 5100)
#		define missing_execinfo_h
#		define missing_extra_regex_errors
#		define missing_wifcore_macro
#		define missing_sys_siglist_extern
#		define missing_sys_siglist_objects
#		define missing_sigiot
#		define missing_sigpwr
#		define missing_rpc_headers
#		define missing_epoll_in_kernel
#		define missing_epoll_h
#	endif
#	if (__version__ <= 6100)
#		define missing_execinfo_h
#		define missing_extra_regex_errors
#		define missing_wifcore_macro

#		ifndef APR_STATUS_IS_ENOBUFS
#			define missing_apr_is_enobufs_macro
#		endif
#	endif
#endif


#ifdef vendor_netbsd
#	if (__version__ <= 1100)
#		define missing_tar_h
#		define missing_wifcore_macro

#		define missing_gethostname_extern
#		define missing_byte_order_externs
#		define missing_fsync_extern
#		define missing_select_extern

#		define missing_crypt_extern
#		define missing_flock_extern
#		define missing_random_extern
#		define missing_ftruncate_extern

#		ifdef _POSIX_SOURCE
#			define missing_bzero_extern
#		endif

#		define needs_POSIX_fixes
#	endif
#endif

#ifdef vendor_symantec
#	if __version__ <= 7100
#		define missing_strdup
#	endif
#endif

#ifdef vendor_next
#	if __version__ == 3300
#		define needs_POSIX_fixes

#		define missing_nx_function_externs

#		define mach
#		define has_netinet
#		define has_malloc_good_size
#		define has_malloc_size
#		define missing_putenv
#		define missing_ascii_in_ctype
#		define missing_strdup
		/*
		 * The following are non-POSIX system calls
		 */
#		define missing_flock_extern
#		define missing_fsync_extern
#		define missing_ftruncate_extern
#		define missing_gethostname_extern
#		define missing_getwd_extern
#		define missing_mktemp_extern
#		define missing_random_extern
#		define missing_select_extern
#		define missing_setpgrp_extern
#		define missing_shutdown_extern
#		define missing_wifcore_macro
#		define missing_sys_siglist_extern

#		define missing_res_query_extern
#		define missing_dn_skipname_extern

#		ifdef ostype_bsd_gnu
#			define missing_select_extern
#		endif

#		define links_on_unresolved_functions_only
#	endif

#	if __version__ <= 3200
#		define needs_POSIX_fixes

#		define missing_nx_function_externs

#		define mach
#		define has_netinet
#		define has_malloc_good_size
#		define has_malloc_size
#		define missing_putenv
#		define missing_ascii_in_ctype
#		define missing_strdup
		/*
		 * The following are non-POSIX system calls
		 */
#		define missing_crypt_extern
#		define missing_flock_extern
#		define missing_fsync_extern
#		define missing_ftruncate_extern
#		define missing_gethostname_extern
#		define missing_getwd_extern
#		define missing_ioctl_extern
#		define missing_mktemp_extern
#		define missing_random_extern
#		define missing_select_extern
#		define missing_setpgrp_extern
#		define missing_shutdown_extern
#		define missing_truncate_extern
#		define missing_wifcore_macro
#		define missing_sys_siglist_extern

#		define missing_res_query_extern
#		define missing_dn_skipname_extern

#		ifdef ostype_bsd_gnu
#			define missing_select_extern
#		endif

#		define links_on_unresolved_functions_only
#	endif

#	if (__version__ < 2000)
#		define missing_memcpy
#		define missing_memset
#		define missing_strchr
#		define missing_strrchr
#		define missing_strstr
#	endif

#	if (__version__ >= 3000)
#		define has_bsd_system_include_directory
#		define has_ansi_system_include_directory
#		define has_architecture_define
#		define has_next_source_define
#	endif

#	if __version__ <= 3200
#	endif
#endif

#ifdef ostype_macosx
#	define missing_extra_regex_errors

#	if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE)
#		define missing_sys_siglist_extern
#		define missing_siglist_nsig
#	endif

#	if __version__ <= 1004
#		define missing_wifcore_macro
#		define missing_strcasecmp_extern
#	endif
#endif

/*
 * PROBLEM SOLUTION SECTION
 * For each problem identified above provide
 * a replacement for buggy functions or defines
 * OR provide the missing code where appropriate.
 */

#include "jr/ezport/apr.h"
#include "jr/ezport/types.h"
#include "jr/ezport/ansi.h"

/*
 * The POSIX compliance code is in its own directory
 * Do this before ezport/missing and ezport/replace, since the posix
 * files may turn on missing defines that are used
 * in ezport/missing
 */
#include "jr/posix/ezport.h"

#include "jr/ezport/replace.h"
#include "jr/ezport/missing.h"

#endif
