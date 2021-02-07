#define _POSIX_SOURCE 1

#include "ezport.h"

#include <signal.h>
#include <errno.h>

#ifdef missing_sigset_objects

int sigaddset(set, signo)
	sigset_t *  set ;
	int         signo ;
{
	int mask = sigmask(signo) ;

	if ((signo <= 0) || (signo > 31)) {
		errno = EINVAL ;
		return(-1) ;
	}
	
	*set |=  mask ;

	return(0) ;
}

int sigdelset(set, signo)
	sigset_t *  set ;
	int         signo ;
{
	int mask = sigmask(signo) ;

	if ((signo <= 0) || (signo > 31)) {
		errno = EINVAL ;
		return(-1) ;
	}
	
	*set &=  ~ mask ;

	return(0) ;
}

int sigemptyset(set)
	sigset_t *  set ;
{
	*set = 0 ;
	return(0) ;
}

int sigfillset(set)
	sigset_t *  set ;
{
	*set = 0xffffffff ;
	return(0) ;
}

int sigismember(set, signo)
	const sigset_t *  set ;
	int         signo ;
{
	int mask = sigmask(signo) ;

	if ((signo <= 0) || (signo > 31)) {
		errno = EINVAL ;
		return(-1) ;
	}

	return(*set & mask) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
