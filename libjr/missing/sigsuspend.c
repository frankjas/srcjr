#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_sigaction_objects

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int sigsuspend(set)
	const sigset_t *  set ;
{
	int mask = *set ;

	return  sigpause(mask) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
