#ifndef __EZPORT_REPLACE_H__
#define __EZPORT_REPLACE_H__



#ifdef replace_buggy_inet_ntoa
#	define inet_ntoa(in_addr)	jr_inet_ntoa(in_addr)

	extern char *jr_inet_ntoa	PROTO ((struct in_addr sin_addr));
#endif

#ifdef replace_buggy_getenv
#	define getenv(name)			jr_system_getenv (name)
	
	extern char *jr_system_getenv		PROTO ((const char *name));
	/*
	 * There is a jr_getenv() in jr/string.h
	 */
#endif

#ifdef replace_sun_siglist_names
#	define sys_siglist		_sys_siglistp
#	define NSIG				_sys_siglistn
#endif

#ifdef missing_utmp_ut_host_field
#	define jr_UTmpHasHostArray(utmp_ptr)			(0)
#	define jr_UTmpHostArraySize(utmp_ptr)			(0)
#	define jr_UTmpHostArrayPtr(utmp_ptr)			(0)
#else
#	define jr_UTmpHasHostArray(utmp_ptr)			(1)
#	define jr_UTmpHostArraySize(utmp_ptr)			sizeof ((utmp_ptr)->ut_host)
#	define jr_UTmpHostArrayPtr(utmp_ptr)			((utmp_ptr)->ut_host)
#endif

#endif
