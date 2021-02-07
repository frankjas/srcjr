#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_tcpgrp

#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>

int tcsetpgrp(fd, pgrp)
	int    fd ;
	pid_t  pgrp ;
{
	return ioctl(fd, TIOCSPGRP, &pgrp) ;
}

pid_t tcgetpgrp(fd)
	int fd ;
{
	jr_int status ;
	pid_t pgrp ;
	
	status = ioctl(fd, TIOCGPGRP, &pgrp) ;

	if (status == -1) return(status) ;

	return(pgrp) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
