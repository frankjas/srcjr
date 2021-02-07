#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef replace_bsd_getpgrp

#include <sys/types.h>

pid_t jr_getpgrp()
{
	return jr_bsd_getpgrp(0) ;
}

#undef getpgrp

int jr_bsd_getpgrp ()
{
	/*
	 * bsd's version takes a process id
	 * 0 means current process
	 */
	extern int getpgrp ();

	return getpgrp (0);
}


#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}

#endif
