#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_sysconf_objects

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

/* include something to get NGROUPS and NOFILE and MAXUPRC and NCARGS */
#include <sys/param.h>

/* include something to get CLK_TCK */
#include <time.h>

long sysconf(desired_value)
	int desired_value ;
{
	switch(desired_value) {

		case _SC_ARG_MAX     :
			/* maximum number of bytes total for exec args */
			return(NCARGS) ;	/* from sys/param.h */

		case _SC_CHILD_MAX   :
			/* maximum number of processes per user */
			return(MAXUPRC) ;	/* from sys/param.h */

		case _SC_CLK_TCK     :
			/* number of "ticks" per second measuring execution time */
			return(CLK_TCK) ;	/* from time.h */

		case _SC_NGROUPS_MAX :
			/* maximum number of groups that one user can be in */
			return(NGROUPS) ;	/* from sys/param.h */

		case _SC_OPEN_MAX    :
			/* maximum number of open file descriptors */
			return(NOFILE) ;	/* from sys/param.h */

		case _SC_JOB_CONTROL :
			/* does this machine support tcsetpgrp() */
			return(_POSIX_JOB_CONTROL) ;	/* from unistd.h */

		case _SC_SAVED_IDS   :
			/* does this machine "save" the real uid upon setuid calls */
			return(_POSIX_SAVED_IDS) ;	/* from unistd.h */

		case _SC_VERSION     :
			return(_POSIX_VERSION) ;	/* from unistd.h */
		
		default              : 
			errno = EINVAL ;
			return(-1) ;
	}
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
