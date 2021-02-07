#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_waitpid

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

extern int		wait4				PROTO ((
					int				pid,
					union wait *	statusp,
					int				options,
					struct rusage *	rusage
				));

pid_t waitpid(pid, statusp, options)
	int    pid ;
	int *  statusp ;
	int    options ;
{
	if (pid == -1) {
		pid = 0 ;
	}
	return  wait4(pid, (union wait *) statusp, options, 0) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
