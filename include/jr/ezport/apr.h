#ifndef _jr_ezport_apr_h___
#define _jr_ezport_apr_h___

/*
** 11/21/06: Match apr-1 compiling options
** Need to define before any other files are included
*/

#ifdef ostype_linux

#	ifndef _GNU_SOURCE
#		define _GNU_SOURCE 1
#	endif

#	if !defined(LINUX) && (__version__ >= 2000)
#		define LINUX 2
#	endif

#	ifndef _REENTRANT
#		define _REENTRANT
#	endif

#	ifndef _LARGEFILE64_SOURCE
#		define _LARGEFILE64_SOURCE
#	endif
#endif

#ifdef ostype_winnt
#	define APR_DECLARE_STATIC
#	define APU_DECLARE_STATIC
#endif
#endif
