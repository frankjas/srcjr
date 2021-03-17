#ifndef __ezport_h__
#define __ezport_h__

#ifdef __cplusplus
extern "C" {
#endif

/*
** 4-11-2011: the jr header files are all straight C
*/



#ifdef WIN32
#	ifndef ostype_winnt
#	define ostype_winnt
#	endif
#endif

#ifdef __linux
#	ifndef ostype_linux
#	define ostype_linux
#	endif
#endif

#ifdef __APPLE__
#	ifndef ostype_macosx
#	define ostype_macosx
#	endif
#endif

#ifdef ostype_winnt
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
#		define missing_stdbool_header
#	endif

#	if (_MSC_VER <= 1800)
#		define needs_winnt_snprintf
#	endif

#	define reduce_winnt_compile
#	define needs_winnt_defines_for_APR
#	define needs_POSIX_fixes
#	define missing_struct_iov
#	define missing_rpc_headers
#	define missing_strptime

#	define missing_execinfo_h
#	define missing_file_descriptors
#	define missing_mode_t
#	define missing_strcasecmp
#	define missing_wifcore_macro

#	define replace_byte_order_funcs
#	define missing_sys_siglist_extern
#	define missing_sys_siglist_objects

#	define missing_unistd_h
	/*
	** from jr/posix/ezport.h
	*/
#	include "jr/ezport/microsoft.h"
#endif

#ifdef ostype_linux
#	ifdef _POSIX_SOURCE
#		define missing_strcasestr
#	endif
#endif

#ifdef ostype_macosx
#	define missing_strcasecmp_extern
#endif

#include "jr/ezport/apr.h"
#include "jr/ezport/types.h"
#include "jr/ezport/ansi.h"
#include "jr/ezport/missing.h"

#ifdef __cplusplus
}
#endif

/*
 * This is an example ezport.h header file.
 * In order to be able to compile code provided
 * by JR International, you need to create a file
 * named ezport.h
 *
 * If you encounter problems compiling JR code,
 * then you should add a platform specifier
 * corresponding to your platform in this file
 *
 * Create a #define which identifies the porting
 * problem (see jr/ezport.h for an example).
 * Then create a conditionally compiled solution
 * for that problem (see jr/ezport/missing.h for
 * an example).
 *
 * You may want to look through jr/ezport/ files
 * since the problem you encountered may already
 * have a solution provided by JR International.
 */

#define jr_pp_string_value(v) #v
#define jr_pp_value(v) pp_string_value(v)

/*
** For debugging macro values (MS Visual Studio only) use:
**
** #pragma message ("Version is " jr_pp_value( __version__))
*/

#endif
