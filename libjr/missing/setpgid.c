#define _POSIX_SOURCE 1
#include "ezport.h"

#ifdef missing_setpgid

#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>

int setpgid(pid, pgrp)
	pid_t  pid ;
	pid_t  pgrp ;
{
	if (pgrp == 0) {
		pgrp = getpid() ;
	}
	if (pid == 0) {
		pid = getpid() ;
	}

	return setpgrp(pid, pgrp) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
